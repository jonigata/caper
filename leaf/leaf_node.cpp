// 2008/08/13 Naoyuki Hirayama

#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Assembly/Writer.h>

#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include "leaf_node.hpp"
#include "leaf_ast.hpp"
#include "leaf_error.hpp"

namespace leaf {

////////////////////////////////////////////////////////////////
// CompileEnv
Symbol*
CompileEnv::intern( const std::string& s )
{
    Symbol* sym;

    symdic_type::const_iterator i = symdic.find( s );
    if( i != symdic.end() ) {
        sym = (*i).second;
    } else {
        sym = cage.allocate<Symbol>( s );
        symdic[s] = sym;
    }
    return sym;
}

Symbol* CompileEnv::gensym()
{
    char buffer[256];
    sprintf( buffer, "$gensym%d", idseed++ );
    return intern( buffer );
}

////////////////////////////////////////////////////////////////
// Environment
template < class T >
class Environment : public boost::noncopyable {
public:
    typedef std::map< Symbol*, T > dic_type;

public:
    Environment(){}
    ~Environment(){}

    void push()
    {
        scope_.resize( scope_.size()+1 );
        //std::cerr << "push" << std::endl;
    }
    void pop()
    {
        scope_.pop_back();
        //std::cerr << "pop" << std::endl;
    }
    void fix()
    {
        scope_.back().modified = false;
    }
    bool modified()
    {
        return scope_.back().modified;
    }
    void update( Symbol* ident, const T& v )
    {
        //std::cerr << "update<" << scope_.size() << ">" << std::endl;
        for( int i = int(scope_.size()) - 1 ; 0 <= i ; i-- ) {
            typename dic_type::iterator j = scope_[i].m.find( ident );
            if( j != scope_[i].m.end() ) {
                if( (*j).second != v ) {
                    (*j).second = v;
                    scope_[i].modified = true;
                }
                return;
            }
        }

        scope_.back().m[ident] = v;
    }
    void bind( Symbol* ident, const T& v )
    {
        scope_.back().m[ident] = v;
    }
    T find( Symbol* ident )
    {
        //std::cerr << "find<" << scope_.size() << ">" << std::endl;
        for( int i = int(scope_.size()) - 1 ; 0 <= i ; i-- ) {
            typename dic_type::const_iterator j = scope_[i].m.find( ident );
            if( j != scope_[i].m.end() ) {
                return (*j).second;
            }
        }
        return T();
    }
    T find_in_toplevel( Symbol* ident )
    {
        typename dic_type::const_iterator j = scope_[0].m.find( ident );
        if( j != scope_[0].m.end() ) {
            return (*j).second;
        }
        return T();
    }

    void print( std::ostream& os )
    {
        for( int i = 0 ; i < int(scope_.size()) ; ++i ) {
            //os << "scope_ " << i << ":" << std::endl;
            for( typename dic_type::const_iterator j = scope_[i].m.begin() ;
                 j != scope_[i].m.end() ;
                 ++j ) {
                os << "  " << (*j).first->s << "(" << (*j).first << ")"
                   << std::endl;
            }
        }
    }

    void refer( Symbol* ident, type_t t )
    {
        // 自由変数の参照宣言
        // findと統一したほうが性能はよいが、
        // わかりやすさのため別メソッドにする

        for( int i = int(scope_.size()) - 1 ; 0 <= i ; i-- ) {
            typename dic_type::const_iterator j = scope_[i].m.find( ident );
            if( j != scope_[i].m.end() ) {
                return;
            } else {
                scope_[i].freevars[ident] = t;
            }
        }
    }

    symmap_t& freevars()
    {
        return scope_.back().freevars;
    }

private:
    struct Scope {
        std::map< Symbol*, T >  m;
        symmap_t                freevars;
        bool                    modified;
    };
    std::vector< Scope > scope_;

    void dump()
    {
        for( int i = 0 ; i < int( scope_.size() ) ; i++ ) {
            //std::cerr << "level " << (i+1) << std::endl;
            for( typename dic_type::const_iterator j = scope_[i].m.begin() ;
                 j != scope_[i].m.end() ;
                 ++j ) {
                //std::cerr << "  " << (*j).first->s << " => " << (*j).second
                //        << std::endl;
                
            }
        }

    }

};

////////////////////////////////////////////////////////////////
// Reference
struct Reference {
    llvm::Value* v;             // 値
    type_t       t;             // 型
    symmap_t     c;             // 自由変数(関数呼び出しのときだけ使う)

    Reference() { v = 0; t = 0; }
    Reference( llvm::Value* av, type_t at, const symmap_t& ac )
    {
        v = av;
        t = at;
        c = ac;
    }

    bool operator==( const Reference& x )
    {
        return v == x.v && t == x.t && c == x.c;
    }
    bool operator!=( const Reference& x ) { return !(*this==x); }

};

std::ostream& operator<<( std::ostream& os, const Reference& v )
{
    os << "{ v = " << v.v << ", t = " << Type::getDisplay( v.t )  << " }";
    return os;
}

////////////////////////////////////////////////////////////////
// Value
class Value {
public:
    Value() { m_ = false; x_ = NULL; t_ = NULL; }
    ~Value() {}
    Value& operator=( const Value& v )
    {
        m_ = v.m_;
        x_ = v.x_;
        t_ = v.t_;
        v_ = v.v_;
        return *this;
    }

    bool empty() const
    {
        return !m_ && !x_ && !t_ && v_.empty();
    }

    void clear()
    {
        m_ = false;
        x_ = NULL;
        t_ = NULL;
        v_.clear();
    }

    void add( const Value& v )
    {
        assert( !x_ );
        m_ = true;
        v_.push_back( v );
    }

    bool isMultiple() const { return m_; }
    llvm::Value* getx() const { return x_; }
    type_t gett() const { return t_; }
    const std::vector< Value >& getv() const { return v_; }

    void assign( llvm::Value* x, type_t t ) { x_ = x; t_ = t; }
    void assign( const std::vector< Value >& v ) { v_ = v; }

    int size() const { if( !m_ ) { return 1; } else { return v_.size(); } }
    const Value& operator[] ( int i ) const
    {
        assert( m_ );
        return v_[i];
    }


private:
    bool                    m_;
    llvm::Value*            x_;
    type_t                  t_;
    std::vector< Value >    v_;

};

std::ostream& operator<<( std::ostream& os, const Value& v )
{
    os << "{ m = " << v.isMultiple() << ", x = " << v.getx()
       << ", t = " << Type::getDisplay( v.gett() ) << ", v = { ";
    if( v.isMultiple() ) {
        for( int i = 0 ; i < v.size() ; i++ ) {
            os << v[i];
            if( i != v.size() - 1 ) {
                os << ", ";
            }
        }
    }
    os << " } }" << std::endl;
    return os;
}

////////////////////////////////////////////////////////////////
// EncodeContext
struct EncodeContext : public boost::noncopyable {
    llvm::Module*               m;
    llvm::Function*             f;
    llvm::BasicBlock*           bb;
    Environment< Reference >    env;
};

////////////////////////////////////////////////////////////////
// EntypeContext
struct EntypeContext : public boost::noncopyable {
    EntypeContext( CompileEnv& ace ) : ce(ace) {}
    
    CompileEnv&                 ce;
    Environment< type_t >       env;
};

inline
void make_reg( char* s, int id )
{
    sprintf( s, "reg%d", id );
}

inline
llvm::Value* check_value_1( const Value& v )
{
    if( v.isMultiple() ) {
        assert( v.size() != 1 );
        return check_value_1( v[0] );
    }
    return v.getx();
}

template < class T > inline void
encode_int_compare(
    T& x, const char* reg, llvm::ICmpInst::Predicate p,
    EncodeContext& cc, Value& value )
{
    x.lhs->encode( cc, false, value );
    llvm::Value* lhs_value = check_value_1( value );
    value.clear();

    x.rhs->encode( cc, false, value );
    llvm::Value* rhs_value = check_value_1( value );
    value.clear();

    value.assign( 
        new llvm::ICmpInst(
            p,
            lhs_value,
            rhs_value,
            reg,
            cc.bb ),
        Type::getBoolType() );
}

template < class LHS, class RHS >
inline
void
binary_operator(
    int                             id,
    EncodeContext&                  cc,
    LHS*                            lhs,
    RHS*                            rhs,
    llvm::Instruction::BinaryOps    op,
    Value&                          value )
{
    char reg[256]; make_reg( reg, id );

    lhs->encode( cc, false, value );
    llvm::Value* v0 = check_value_1( value );
    value.clear();

    rhs->encode( cc, false, value );
    llvm::Value* v1 = check_value_1( value );
    value.clear();

    llvm::Instruction* inst = llvm::BinaryOperator::create(
        op, v0, v1, reg );
    cc.bb->getInstList().push_back(inst);

    value.assign( inst, Type::getIntType() );
}

inline
void update_type( EntypeContext& tc, Header& h, type_t t )
{
    if( t && h.t != t ) { h.t = t; }
}

inline
void check_empty( const Value& value )
{
    assert( value.empty() );
}

inline
void check_bool_expr_type( const Header& h, type_t t )
{
    if( t && t != Type::getBoolType() ) {
        throw context_mismatch( h.beg, Type::getDisplay( t ), "<bool>" );
    }
}

inline
void check_int_expr_type( const Header& h, type_t t )
{
    if( t && t != Type::getIntType() ) {
        throw context_mismatch( h.beg, Type::getDisplay( t ), "<int>" );
    }
}

template < class T > inline
void entype_int_compare( EntypeContext& tc, T& x )
{
    x.lhs->entype( tc, false, Type::getIntType() );
    x.rhs->entype( tc, false, Type::getIntType() );
    update_type( tc, x.h, Type::getBoolType() );
}

template < class T > inline
void entype_int_binary_arithmetic( EntypeContext& te, T& x )
{
    x.lhs->entype( te, false, Type::getIntType() );
    x.rhs->entype( te, false, Type::getIntType() );
    update_type( te, x.h, Type::getIntType() );
}

inline
const llvm::Type* getLLVMType( type_t t, bool add_env = false )
{
    assert( t );

    switch( t->tag() ) {
    case Type::TAG_BOOL: return llvm::Type::Int1Ty;
    case Type::TAG_CHAR: return llvm::Type::Int8Ty;
    case Type::TAG_SHORT: return llvm::Type::Int16Ty;
    case Type::TAG_INT: return llvm::Type::Int32Ty;
    case Type::TAG_LONG: return llvm::Type::Int64Ty;
    case Type::TAG_TUPLE:
        {
            int n = Type::getTupleSize( t );
            if( n == 0 ) {
                return llvm::Type::VoidTy;
            } else if( n == 1 ) {
                assert(0); // ありえない
            } else {
                std::vector< const llvm::Type* > v;
                for( int i = 0 ; i < n ; i++ ) {
                    v.push_back( getLLVMType( t->getElement(i) ) );
                }
                return llvm::StructType::get( v );
            }
        }
    case Type::TAG_FUNCTION:
        {
            type_t a = t->getArgumentType();
            int n = Type::getTupleSize( a );
            std::vector< const llvm::Type* > v;
            if( add_env ) {
                v.push_back( llvm::PointerType::getUnqual(
                                 llvm::Type::Int8Ty ) );
            }
            if( 0 < n ) {
                for( int i = 0 ; i < n ; i++ ) {
                    v.push_back( getLLVMType( a->getElement( i ) ) );
                }
            }

            return llvm::FunctionType::get(
                getLLVMType( t->getReturnType() ), v, /* not vararg */ false );
        }
    case Type::TAG_CLOSURE:
        {
            
            std::vector< const llvm::Type* > v;
            v.push_back( llvm::PointerType::getUnqual(
                             getLLVMType( t->getRawFunc(), true ) ) );
            v.push_back( llvm::Type::Int8Ty );
            return llvm::PointerType::getUnqual( llvm::StructType::get( v ) );
        }
    default:
        return llvm::Type::Int32Ty;
    }
}

static type_t
typeexpr_to_type( TypeExpr* t )
{
	if( TypeRef* tt = dynamic_cast< TypeRef* >( t ) ) {
		return tt->t;
	}
	if( Types* tt = dynamic_cast< Types* >( t ) ) {
		typevec_t v;
		for( size_t i = 0 ; i < tt->v.size() ; i++ ) {
			v.push_back( typeexpr_to_type( tt->v[i] ) );
		}
		return Type::getTupleType( v );
	}
	assert(0);
	return NULL;
}

static type_t
formalargs_to_type( FormalArgs* formalargs )
{
	typevec_t v;
    for( size_t i = 0 ; i < formalargs->v.size() ; i++ ) {
        if( !formalargs->v[i]->t ) {
			return NULL;
        }
        v.push_back( typeexpr_to_type( formalargs->v[i]->t ) );
    }
	return Type::getTupleType( v );
}

inline
void freevars_to_typevec( const symmap_t& freevars, typevec_t& v )
{
    for( symmap_t::const_iterator i = freevars.begin() ;
         i != freevars.end() ;
         ++i ) {
        v.push_back( (*i).second );
    }
}

inline llvm::Function*
encode_function(
    EncodeContext&  cc,
    bool,
    const Header&   h,
    Symbol*         funcname,
    FormalArgs*     formal_args,
    TypeExpr*       result_type,
    Block*          body,
    const symmap_t& freevars )
{
    // signature

    // ...arguments
    std::vector< const llvm::Type* > atypes;
    for( symmap_t::const_iterator i = freevars.begin() ;
         i != freevars.end() ;
         ++i ) {
        atypes.push_back( getLLVMType( (*i).second ) );
    }
    for( size_t i = 0 ; i < formal_args->v.size() ; i++ ) {
        atypes.push_back(
			getLLVMType( typeexpr_to_type( formal_args->v[i]->t ) ) );
    }

    // ...result
    type_t leaf_rtype = typeexpr_to_type( result_type );
    if( Type::isFunction( leaf_rtype ) ) {
        leaf_rtype = Type::getClosureType( leaf_rtype );
    }
    const llvm::Type* rtype = getLLVMType( leaf_rtype );

    // function type
    llvm::FunctionType* ft =
        llvm::FunctionType::get(
            rtype, atypes, /* not vararg */ false );

    //std::cerr << "fundef encode: " << *ft << std::endl;

    // function
    llvm::Function* f =
        llvm::Function::Create(
            ft, llvm::Function::ExternalLinkage,
            funcname->s,
            cc.m );

    //std::cerr << "fundef bind: " << h.t << std::endl;
    cc.env.bind( funcname, Reference( NULL, h.t, freevars ) );

    // get actual arguments
    cc.env.push();
    {
        symmap_t::const_iterator env_iterator = freevars.begin();
        std::vector< FormalArg* >::const_iterator arg_iterator =
            formal_args->v.begin();
        for( llvm::Function::arg_iterator i = f->arg_begin();
             i != f->arg_end() ;
             ++i ) {
            if( env_iterator != freevars.end() ) {
                i->setName( "env_" + (*env_iterator).first->s );
                cc.env.bind(
                    (*env_iterator).first,
                    Reference( i, (*env_iterator).second, symmap_t() ) );
                env_iterator++;
            } else {
                i->setName( "arg_" + (*arg_iterator)->name->s->s );
                cc.env.bind(
                    (*arg_iterator)->name->s,
                    Reference( i, (*arg_iterator)->h.t, symmap_t() ) );
                arg_iterator++;
            }
        }
    }

    // basic block
    llvm::BasicBlock* bb = llvm::BasicBlock::Create("ENTRY", f);

    std::swap( cc.bb, bb );

    cc.f = f;
    Value value;
    body->encode( cc, false, value );

    cc.env.pop();

    if( h.t->getReturnType() == Type::getVoidType() ) {
        llvm::ReturnInst::Create( cc.bb );
    }  else {
        assert( value.size() == Type::getTupleSize( h.t->getReturnType() ) );

        if( value.size() == 1 ) {
            llvm::ReturnInst::Create( value.getx(), cc.bb );
        } else {
            std::cerr << value << std::endl;
            std::vector< const llvm::Type* > tv;
            for( int i = 0 ; i < value.size() ; i++ ) {
                tv.push_back( getLLVMType( value[i].gett() ) );
            }
            llvm::Type* return_type = llvm::StructType::get( tv );

            llvm::Value* undef = llvm::UndefValue::get( return_type ); 
			llvm::Value* prev = undef;
            for( int i = 0 ; i < value.size() ; i++ ) {
                char reg[256];
                sprintf( reg, "insval%d_%d", h.id, i );
                prev = llvm::InsertValueInst::Create(
                    prev, value[i].getx(), i, reg, cc.bb ) ;
            }

            llvm::ReturnInst::Create( prev, cc.bb );
        }
    }

    std::swap( cc.bb, bb );

    return f;
}

inline void
entype_function(
    EntypeContext&  tc,
    bool,
    type_t          t,
    Header&         h,
    Symbol*         funcname,
    FormalArgs*     formal_args,
    TypeExpr*       result_type,
    Block*          body,
    symmap_t&       freevars )
{
	if( !result_type ) {
		throw inexplicit_return_type( h.beg );
	}

	// 返り値の型
    type_t rttype = typeexpr_to_type( result_type );
    if( !Type::isComplete( rttype ) ) {
		// あり得ないはず
		assert(0);
        throw imcomplete_return_type( h.beg );
    }

    if( Type::isFunction( rttype ) ) {
        rttype = Type::getClosureType( rttype );
    }

    // 引数の型
	type_t attype = formalargs_to_type( formal_args );
	if( !Type::isComplete( attype ) ) {
		throw inexplicit_argument_type( h.beg );
	}

    // 再帰関数のために本体より先にbind
    update_type( tc, h, Type::getFunctionType( rttype, attype ) );
    tc.env.bind( funcname, h.t );

    tc.env.push();
    for( size_t i = 0 ; i < formal_args->v.size() ; i++ ) {
        tc.env.bind(
			formal_args->v[i]->name->s,
			typeexpr_to_type( formal_args->v[i]->t ) );
    }
    tc.env.fix();

  retry:
    // 関数の戻り値になるコンテキストでは正規化
    body->entype( tc, false, rttype );

    // 環境が変更されていたらリトライ
    if( tc.env.modified() ) {
        tc.env.fix();
        goto retry;
    }

    if( !body->h.t ) {
        throw noreturn( h.end, Type::getDisplay( rttype ) );
    }

    // 自由変数
    //std::cerr << "freevars: ";
    for( symmap_t::iterator i = tc.env.freevars().begin();
         i != tc.env.freevars().end() ;
         ++i ) {
        Symbol* s = (*i).first;
        if( !tc.env.find_in_toplevel( s ) ) {
            freevars[s] = (*i).second;
            //std::cerr << (*i)->s << ", ";
        }
    }
    //std::cerr << std::endl;

    // 引数の型をアップデート
    for( size_t i = 0 ; i < formal_args->v.size() ; i++ ) {
        type_t t = tc.env.find( formal_args->v[i]->name->s );
        if( !t ) {
            // TODO: error
        }
        formal_args->v[i]->h.t = t;
    }

    tc.env.pop();
}

type_t make_tuple_tree_from_vardeclelems( VarDeclElems* f )
{
    typevec_t v;
    for( size_t i = 0 ; i < f->v.size() ; i++ ) {
        VarDeclElem* e = f->v[i];

        VarDeclElems* ev = dynamic_cast<VarDeclElems*>(e);
        if( ev ) {
            v.push_back( make_tuple_tree_from_vardeclelems( ev ) );
            continue;
        }

        VarDeclIdentifier* ei = dynamic_cast<VarDeclIdentifier*>(e);
        if( ei ) {
            if( ei->t ) {
                v.push_back( typeexpr_to_type( ei->t ) );
            } else {
                v.push_back( NULL );
            }
            continue;
        }

        assert(0);
    }

    return Type::getTupleType( v );
}

////////////////////////////////////////////////////////////////
// Node
void Node::encode( llvm::Module* m )
{
    EncodeContext cc;
    cc.m = m;
    cc.f = NULL;
    cc.bb = NULL;
    cc.env.push();
    Value value;
    encode( cc, false, value );
    cc.env.pop();
}
void Node::entype( CompileEnv& ce )
{
    EntypeContext tc( ce );
    tc.env.push();
    entype( tc, false, NULL );
    tc.env.pop();
}

////////////////////////////////////////////////////////////////
// Module
void Module::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    topelems->encode( cc, false, value );
}
void Module::entype( EntypeContext& tc, bool, type_t t )
{
    topelems->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopElems
void TopElems::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->encode( cc, false, value );
        if( i != v.size() - 1 ) {
            value.clear();
        }
    }
}
void TopElems::entype( EntypeContext& tc, bool, type_t t )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->entype( tc, false, t );
    }
}

////////////////////////////////////////////////////////////////
// TopElem
void TopElem::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void TopElem::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Require
void Require::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    module->encode( cc, false, value );
    value.clear();
}
void Require::entype( EntypeContext& tc, bool, type_t t )
{
    module->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDecl
void TopLevelFunDecl::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    fundecl->encode( cc, false, value );
    value.clear();
}
void TopLevelFunDecl::entype( EntypeContext& tc, bool, type_t t )
{
    fundecl->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDef
void TopLevelFunDef::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    fundef->encode( cc, false, value );
    value.clear();
}
void TopLevelFunDef::entype( EntypeContext& tc, bool, type_t t )
{
    fundef->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// Block
void Block::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    statements->encode( cc, false, value );
}
void Block::entype( EntypeContext& tc, bool, type_t t )
{
    //std::cerr << "block " << Type::getDisplay( t ) << std::endl;
    statements->entype( tc, false, t );

    if( !statements->v.empty() ) {
        update_type( tc, h, statements->v.back()->h.t );
    }
}

////////////////////////////////////////////////////////////////
// Statements
void Statements::encode( EncodeContext& cc, bool drop_value, Value& value )
{
    check_empty( value );

    for( size_t i = 0 ; i < v.size() ; i++ ) {
        bool this_drop_value = drop_value || i != v.size() - 1;
        v[i]->encode( cc, this_drop_value, value );
        if( this_drop_value ) {
            value.clear();
        }
    }
}
void Statements::entype( EntypeContext& tc, bool, type_t t )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        if( i != v.size() - 1 ) {
            v[i]->entype( tc, true, NULL );
        } else {
            v[i]->entype( tc, false, t );
        }
    }
}

////////////////////////////////////////////////////////////////
// Statement
void Statement::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Statement::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FunDecl
void FunDecl::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    // signature
    std::vector< const llvm::Type* > types;
    for( size_t i = 0 ; i < sig->fargs->v.size() ; i++ ) {
        types.push_back( llvm::Type::Int32Ty );
    }

    llvm::FunctionType* ft =
        llvm::FunctionType::get(
            llvm::Type::Int32Ty, types, /* not vararg */ false );

    // function
    llvm::Function::Create(
        ft, llvm::Function::ExternalLinkage,
        sig->name->s->s,
        cc.m );

    //std::cerr << "fundecl bind: " << h.t << std::endl;
    cc.env.bind( sig->name->s, Reference( NULL, h.t, symmap_t() ) );
}
void FunDecl::entype( EntypeContext& tc, bool, type_t t )
{
    // 戻り値の型
    type_t result_type = typeexpr_to_type( sig->result_type );
	if( !Type::isComplete( result_type ) ) {
		throw imcomplete_return_type( h.beg );
	}

    // 引数の型
    type_t args_type = formalargs_to_type( sig->fargs );
	if( !Type::isComplete( args_type ) ) {
		throw inexplicit_argument_type( h.beg );
	}

    update_type( tc, h, Type::getFunctionType( result_type, args_type ) );
    tc.env.bind( sig->name->s, h.t );
}

////////////////////////////////////////////////////////////////
// FunDef
void FunDef::encode( EncodeContext& cc, bool drop_value, Value& value )
{
    check_empty( value );

    encode_function(
        cc,
        drop_value,
        h,
        sig->name->s,
        sig->fargs,
        sig->result_type,
        body,
        freevars );
}
void FunDef::entype( EntypeContext& tc, bool drop_value, type_t t )
{
    entype_function(
        tc,
        drop_value,
        t,
        h,
        sig->name->s,
        sig->fargs,
        sig->result_type,
        body,
        freevars );
}

////////////////////////////////////////////////////////////////
// FunSig
void FunSig::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void FunSig::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArgs
void FormalArgs::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void FormalArgs::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArg
void FormalArg::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void FormalArg::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDecl
void foo( EncodeContext& cc, VarDeclElem* f, const Value& a )
{
    VarDeclElems* fv = dynamic_cast<VarDeclElems*>(f);
    if( fv ) {
        if( a.size() == 1 ) {
            foo( cc, fv->v[0], a );
        } else {
            for( size_t i = 0 ; i < fv->v.size() ; i++ ) {
                foo( cc, fv->v[i], a[i] );
            }
        }
        return;
    }

    VarDeclIdentifier* fi = dynamic_cast<VarDeclIdentifier*>(f);
    if( fi ) {
        cc.env.bind(
            fi->name->s, Reference( a.getx(), a.gett(), symmap_t() ) );
        return;
    }

    assert(0);
}

void VarDecl::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    this->value->encode( cc, false, value );

    foo( cc, this->varelems, value );
    value.clear();
}
void VarDecl::entype( EntypeContext& tc, bool drop_value, type_t )
{
    if( !drop_value ) {
        throw unused_variable( h.beg, "@@@@" );
    }

    varelems->entype( tc, drop_value, NULL );
    type_t ft = make_tuple_tree_from_vardeclelems( varelems );

  retry:
    value->entype( tc, false, ft );
    type_t at = value->h.t;
    
    if( ft != at ) {
        type_t ut = Type::unify( ft, at );
        if( !ut ) {
            throw type_mismatch(
                h.beg,
                Type::getDisplay( ft ) + " at variable" ,
                Type::getDisplay( at ) + " at value" );
        }
        varelems->entype( tc, false, ut );
        value->entype( tc, false, ut );
        ft = ut;
        goto retry;
    }
}

////////////////////////////////////////////////////////////////
// VarDeclElem
void VarDeclElem::encode( EncodeContext&, bool, Value& )
{
    assert(0);
}
void VarDeclElem::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDeclElems
void VarDeclElems::encode( EncodeContext&, bool, Value& )
{
    assert(0);
}
void VarDeclElems::entype( EntypeContext& tc, bool, type_t t )
{
    if( v.size() == 1 ) {
        v[0]->entype( tc, false, t );
        update_type( tc, h, v[0]->h.t );
        return;
    }

    if( t && int( v.size() ) != Type::getTupleSize( t ) ) {
        throw type_mismatch(
            h.beg,
            Type::getDisplay( h.t ) + " at variable" ,
            Type::getDisplay( t ) + " at value" );
    }

    typevec_t tv;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->entype( tc, false, Type::getElementType( t, i ) );
        tv.push_back( v[i]->h.t );
    }
    update_type( tc, h, Type::getTupleType( tv ) );
}

////////////////////////////////////////////////////////////////
// VarDeclIdentifier
void VarDeclIdentifier::encode( EncodeContext&, bool, Value& )
{
    assert(0);
}
void VarDeclIdentifier::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && h.t && t != h.t ) {
        throw type_mismatch(
            h.beg,
            Type::getDisplay( h.t ) + " at variabne" ,
            Type::getDisplay( t ) + " at value" );
    }
    update_type( tc, h, t );
    tc.env.bind( name->s, h.t );
}

////////////////////////////////////////////////////////////////
// IfThenElse
void IfThenElse::encode( EncodeContext& cc, bool drop_value, Value& value )
{
    check_empty( value );

    cond->encode( cc, false, value );
    llvm::Value* cond_value = check_value_1( value );
    value.clear();

    char if_r[256]; sprintf( if_r, "if_r%d", h.id );
    char if_v[256]; sprintf( if_v, "if_v%d", h.id );

    char if_t_label[256]; sprintf( if_t_label, "IF_T%d", h.id );
    char if_f_label[256]; sprintf( if_f_label, "IF_F%d", h.id );
    char if_j_label[256]; sprintf( if_j_label, "IF_J%d", h.id );

    llvm::BasicBlock* tbb = llvm::BasicBlock::Create(
        if_t_label, cc.f );
    llvm::BasicBlock* fbb = llvm::BasicBlock::Create(
        if_f_label, cc.f );
    llvm::BasicBlock* jbb = llvm::BasicBlock::Create(
        if_j_label, cc.f );

    llvm::BranchInst::Create( tbb, fbb, cond_value, cc.bb );

    cc.bb = tbb;

    Value tvalue;
    iftrue->encode( cc, false, tvalue );

    llvm::BranchInst::Create( jbb, cc.bb );
    
    cc.bb = fbb;

    Value fvalue;
    iffalse->encode( cc, false, fvalue );
    
    llvm::BranchInst::Create( jbb, cc.bb );

    cc.bb = jbb;

    llvm::PHINode* phi = NULL;
    const llvm::Type* iftype = getLLVMType( iftrue->h.t );

    if( !drop_value && iftrue->h.t != Type::getVoidType() ) {
        int n = tvalue.size();
        if( n == 1 ) {
            char reg[256];
            sprintf( reg, "phi_%d", h.id );

            phi = llvm::PHINode::Create( iftype, reg, cc.bb );
            phi->addIncoming( tvalue.getx(), tbb );
            phi->addIncoming( fvalue.getx(), fbb );
    
            value.assign( phi, iftrue->h.t ); // TODO: 決めうち
        } else {
            for( int i = 0 ; i < n ; i++ ) {
                char reg[256];
                sprintf( reg, "phi_%d_%d", h.id, i );
                
                phi = llvm::PHINode::Create( iftype, reg, cc.bb );
                phi->addIncoming( tvalue.getx(), tbb );
                phi->addIncoming( fvalue.getx(), fbb );

                Value av; av.assign(
                    phi, Type::getElementType( iftrue->h.t, i ) );
                value.add( av );
            }
        }
    } else {
        value.assign( NULL, iftrue->h.t );
    }
}
void IfThenElse::entype( EntypeContext& tc, bool drop_value, type_t t )
{
    cond->entype( tc, false, Type::getBoolType() );

    iftrue->entype( tc, false, t );
    iffalse->entype( tc, false, t );

    if( !drop_value && !t ) {
        if( !iftrue->h.t && !iffalse->h.t ) {

        } else if( iftrue->h.t && iffalse->h.t ) {
            if( iftrue->h.t != iffalse->h.t ) {
                throw type_mismatch(
                    h.beg,
                    Type::getDisplay( iftrue->h.t ) + " at true-clause" ,
                    Type::getDisplay( iffalse->h.t ) + " at false-clause" );
            }
        } else if( iftrue->h.t && !iffalse->h.t ) {
            iffalse->entype( tc, false, iftrue->h.t );
        } else if( !iftrue->h.t && iffalse->h.t ) {
            iftrue->entype( tc, false, iffalse->h.t );
        }
    }

    update_type( tc, h, iftrue->h.t );
}

////////////////////////////////////////////////////////////////
// TypeExpr
void TypeExpr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void TypeExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Types
void Types::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Types::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// TypeRef
void TypeRef::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void TypeRef::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MultiExpr
void MultiExpr::encode( EncodeContext& cc, bool drop_value, Value& value )
{
    check_empty( value );

    if( v.size() == 1 ) {
        v[0]->encode( cc, drop_value, value );
    } else {
        for( size_t i = 0 ; i < v.size() ; i++ ) {
            Value v;
            this->v[i]->encode( cc, drop_value, v );
            std::cerr << "me: " << v << std::endl;
            value.add( v );
        }
    }
}
void MultiExpr::entype( EntypeContext& tc, bool drop_value, type_t t )
{
    if( v.size() == 1 ) {
        v[0]->entype( tc, drop_value, t );
        update_type( tc, h, v[0]->h.t );
    } else {
        if( t ) {
            int n = Type::getTupleSize( t );
            if( v.size() != 1 && n != int(v.size()) ) {
                Addr addr;
                if( !v.empty() ) { addr = v[0]->h.beg; }
                throw wrong_multiple_value( addr, v.size(), n );
            }
        }

        typevec_t tv;
        for( size_t i = 0 ; i < v.size() ; i++ ) {
            v[i]->entype( tc, drop_value, Type::getElementType( t, int(i) ) );
            tv.push_back( v[i]->h.t );
        }

        update_type( tc, h, Type::getTupleType( tv ) );
    }
}

////////////////////////////////////////////////////////////////
// Expr
void Expr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Expr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void LogicalOr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOrElems::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    // 結果レジスタ
    char label[256];
    sprintf( label, "or_s%d_result", h.id );
    llvm::Instruction* orresult =
        new llvm::AllocaInst( 
            llvm::Type::Int1Ty,
            label,
            cc.bb );

    // 式の数だけBasicBlockを作成(最後のブロックは失敗ブロック)
    std::vector< llvm::BasicBlock* > bb;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        sprintf( label, "or_s%d_%d", h.id, int(i) );
        bb.push_back( llvm::BasicBlock::Create( label, cc.f ) );
    }

    // 失敗ブロック
    new llvm::StoreInst( llvm::ConstantInt::getFalse(), orresult, bb.back() );

    // 成功ブロック
    sprintf( label, "or_s%d_ok", h.id );
    llvm::BasicBlock* success = llvm::BasicBlock::Create( label, cc.f );
    new llvm::StoreInst( llvm::ConstantInt::getTrue(), orresult, success );

    // 最終ブロック
    sprintf( label, "or_s%d_final", h.id );
    llvm::BasicBlock* final = llvm::BasicBlock::Create( label, cc.f );
    llvm::BranchInst::Create( final, bb.back() );
    llvm::BranchInst::Create( final, success );

    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->encode( cc, false, value );
        llvm::Value* vv = check_value_1( value );
        value.clear();
        
        llvm::BranchInst::Create( success, bb[i], vv, cc.bb );
        cc.bb = bb[i];
    }

    cc.bb = final;
    sprintf( label, "or_s%d_value", h.id );

    value.assign(
        new llvm::LoadInst( orresult, label, final ),
        Type::getBoolType() );
}
void LogicalOrElems::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    for( size_t i = 0 ; i < v.size(); i++ ) {
        v[i]->entype( tc, false, Type::getBoolType() );
    }
}

////////////////////////////////////////////////////////////////
// LogicalAnd
void LogicalAnd::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void LogicalAnd::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalAndElems
void LogicalAndElems::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    // 結果レジスタ
    char label[256];
    sprintf( label, "and_s%d_result", h.id );
    llvm::Instruction* andresult =
        new llvm::AllocaInst( 
            llvm::Type::Int1Ty,
            label,
            cc.bb );

    // 式の数だけBasicBlockを作成(最後のブロックは成功ブロック)
    std::vector< llvm::BasicBlock* > bb;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        sprintf( label, "and_s%d_%d", h.id, int(i) );
        bb.push_back( llvm::BasicBlock::Create( label, cc.f ) );
    }

    // 成功ブロック
    new llvm::StoreInst( llvm::ConstantInt::getTrue(), andresult, bb.back() );

    // 失敗ブロック
    sprintf( label, "and_s%d_ng", h.id );
    llvm::BasicBlock* failure = llvm::BasicBlock::Create( label, cc.f );
    new llvm::StoreInst( llvm::ConstantInt::getFalse(), andresult, failure );

    // 最終ブロック
    sprintf( label, "and_s%d_final", h.id );
    llvm::BasicBlock* final = llvm::BasicBlock::Create( label, cc.f );
    llvm::BranchInst::Create( final, bb.back() );
    llvm::BranchInst::Create( final, failure );

    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->encode( cc, false, value );
        llvm::Value* vv = check_value_1( value );
        value.clear();

        llvm::BranchInst::Create( bb[i], failure, vv, cc.bb );
        cc.bb = bb[i];
    }

    cc.bb = final;
    sprintf( label, "and_s%d_value", h.id );
    value.assign(
        new llvm::LoadInst( andresult, label, final ),
        Type::getBoolType() );
}
void LogicalAndElems::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    for( size_t i = 0 ; i < v.size(); i++ ) {
        v[i]->entype( tc, false, Type::getBoolType() );
    }
}

////////////////////////////////////////////////////////////////
// Equality
void Equality::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Equality::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// EqualityEq
void EqualityEq::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "eq", llvm::ICmpInst::ICMP_EQ, cc, value );
}
void EqualityEq::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// EqualityNe
void EqualityNe::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "ne", llvm::ICmpInst::ICMP_NE, cc, value );
}
void EqualityNe::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// Relational
void Relational::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Relational::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// RelationalLt
void RelationalLt::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "lt", llvm::ICmpInst::ICMP_SLT, cc, value );
}
void RelationalLt::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// RelationalGt
void RelationalGt::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "gt", llvm::ICmpInst::ICMP_SGT, cc, value );
}
void RelationalGt::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// RelationalLe
void RelationalLe::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "le", llvm::ICmpInst::ICMP_SLE, cc, value );
}
void RelationalLe::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// RelationalGe
void RelationalGe::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "ge", llvm::ICmpInst::ICMP_SGE, cc, value );
}
void RelationalGe::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// Additive
void Additive::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Additive::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// AddExpr
void AddExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    binary_operator( h.id, cc, lhs, rhs, llvm::Instruction::Add, value );
}
void AddExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// SubExpr
void SubExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    binary_operator( h.id, cc, lhs, rhs, llvm::Instruction::Sub, value );
}
void SubExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// Multiplicative
void Multiplicative::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Multiplicative::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MulExpr
void MulExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    binary_operator( h.id, cc, lhs, rhs, llvm::Instruction::Mul, value );
}
void MulExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// DivExpr
void DivExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    binary_operator( h.id, cc, lhs, rhs, llvm::Instruction::SDiv, value );
}
void DivExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// PrimExpr
void PrimExpr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void PrimExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralBoolean
void LiteralBoolean::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    value.assign( 
        this->value ?
        llvm::ConstantInt::getTrue() :
        llvm::ConstantInt::getFalse(),
        Type::getBoolType() );
}
void LiteralBoolean::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && t != Type::getBoolType() ) {
        throw context_mismatch( h.beg, "<bool>", Type::getDisplay( t ) );
    }
    update_type( tc, h, Type::getBoolType() );
}

////////////////////////////////////////////////////////////////
// LiteralInteger
void LiteralInteger::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    value.assign(
        llvm::ConstantInt::get( llvm::Type::Int32Ty, this->value ),
        Type::getIntType() );
}
void LiteralInteger::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && t != Type::getIntType() ) {
        throw context_mismatch( h.beg, "<int>", Type::getDisplay( t ) );
    }
    update_type( tc, h, Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// LiteralChar
void LiteralChar::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    value.assign(
        llvm::ConstantInt::get( llvm::Type::Int32Ty, this->value ),
        Type::getIntType() );
}
void LiteralChar::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && t != Type::getIntType() ) {
        throw context_mismatch( h.beg, "<int>", Type::getDisplay( t ) );
    }
    update_type( tc, h, Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// VarRef
void VarRef::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    Reference r = cc.env.find( name->s );;
    if( !r.v ) {
        //cc.print( std::cerr );
        throw no_such_variable( h.beg, name->s->s );
    }
    if( !r.t ) {
        throw ambiguous_type( h.beg, name->s->s );
    }
    value.assign( r.v, r.t );
}
void VarRef::entype( EntypeContext& tc, bool, type_t t )
{
    type_t vt = tc.env.find( name->s );

    if( vt ) {
        // すでに変数の型が決まっている
        if( t ) {
            // コンテキスト型がanyでない
            if( vt != t ) {
                throw context_mismatch(
                    h.beg, Type::getDisplay( vt ), Type::getDisplay( t ) );
            }
        }
        tc.env.update( name->s, vt );
        update_type( tc, h, vt );
    } else {
        // 変数の型がまだ決まってない
        if( t ) {
            // コンテキスト型が決まっている
            tc.env.update( name->s, t );
            update_type( tc, h, t );
        }
    }        

    tc.env.refer( name->s, h.t );
}

////////////////////////////////////////////////////////////////
// Parenthized
void Parenthized::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    exprs->encode( cc, false, value );
}
void Parenthized::entype( EntypeContext& tc, bool, type_t t )
{
    exprs->entype( tc, false, t );
    update_type( tc, h, exprs->h.t );
}

////////////////////////////////////////////////////////////////
// CastExpr
void CastExpr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void CastExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Cast
void Cast::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    char reg[256];
    sprintf( reg, "cast%d", h.id );

    expr->encode( cc, false, value );
    llvm::Value* expr_value = check_value_1( value );
    value.clear();

	type_t tt = typeexpr_to_type( this->t );
    value.assign( 
        llvm::CastInst::createIntegerCast(
            expr_value,
            getLLVMType( tt ),
            true,
            reg,
            cc.bb ),
        tt );
}
void Cast::entype( EntypeContext& tc, bool, type_t t )
{
	type_t tt = typeexpr_to_type( this->t );
    if( t != tt ) {
        throw context_mismatch(
            h.beg,
            Type::getDisplay( tt ),
            Type::getDisplay( t ) );
    }
    expr->entype( tc, false, NULL );
    update_type( tc, h, tt );
}

////////////////////////////////////////////////////////////////
// FunCall
void FunCall::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    Reference r = cc.env.find( func->s );
    //std::cerr << "funcall: " << func->s->s << std::endl;
    if( !r.t ) {
        throw no_such_function( h.beg, func->s->s );
    }

    //std::cerr << func->s->s << ": " << Type::getDisplay( v.t ) << std::endl;
    if( Type::isFunction( r.t ) ) {
        llvm::Function* f = cc.m->getFunction( func->s->s );

        //std::cerr << "funcall type(" << func->s->s << "): "
        //<< *f->getType() << std::endl;

        std::vector< llvm::Value* > args;
        for( symmap_t::const_iterator i = r.c.begin() ;
             i != r.c.end() ;
             ++i ) {
            Reference r = cc.env.find( (*i).first );
            assert( r.v );
            //std::cerr << "env: " << vv.v << std::endl;
            args.push_back( r.v );
        }
        for( size_t i = 0 ; i < aargs->v.size() ; i++ ) {
            aargs->v[i]->encode( cc, false, value ); // TODO: 暫定
            llvm::Value* vv = check_value_1( value );
            value.clear();
            
            assert( vv );
            //std::cerr << "arg: " << vv << std::endl;
            args.push_back( vv );
        }

        char reg[256];
        sprintf( reg, "ret%d", h.id );

        //std::cerr << "args: " << args.size() << std::endl;

        int n = Type::getTupleSize( r.t->getReturnType() );
        if( n == 1 ) {
            value.assign(
                llvm::CallInst::Create(
                    f, args.begin(), args.end(), reg, cc.bb ),
                r.t->getReturnType() );
        } else {
            llvm::Value* ret = llvm::CallInst::Create(
                f, args.begin(), args.end(), reg, cc.bb );
            for( int i = 0 ; i < n ; i++ ) {
                sprintf( reg, "ret%d_%d", h.id, i );
                llvm::Instruction* lv = llvm::ExtractValueInst::Create(
                    ret, i, reg, cc.bb );

                Value av;
                av.assign( lv, Type::getElementType( r.t, i ) );
                std::cerr << "av: " << av << std::endl;
                value.add( av );
            }
        }
    } else if( Type::isClosure( r.t ) ) {
        char reg[256];

        //std::cerr << "closure call: " << *v.v->getType() << std::endl;
        
        llvm::Value* indices[2];
        indices[0] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );
        indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );

        // stub func type
        sprintf( reg, "stub_func_addr%d", h.id );
        llvm::Value* faddr = llvm::GetElementPtrInst::Create(
            r.v, indices, indices+2, reg, cc.bb );

        sprintf( reg, "stub_func_ptr%d", h.id );
        llvm::Value* fptr = new llvm::LoadInst( faddr, reg, cc.bb );

/*
        const llvm::PointerType* fptr_type =
            llvm::cast<llvm::PointerType>(fptr->getType());

        const llvm::FunctionType* func_type =
            llvm::cast<llvm::FunctionType>(fptr_type->getElementType());
*/
        //std::cerr << "func ptr: " << *fptr_type << std::endl;

        // stub env type
        indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 1 );

        sprintf( reg, "stub_env_addr%d", h.id );
        llvm::Value* eaddr = llvm::GetElementPtrInst::Create(
            r.v, indices, indices+2, reg, cc.bb );

        //std::cerr << "stub env addr: " << *eaddr->getType()
        //<< std::endl;

        std::vector< llvm::Value* > args;
        args.push_back( eaddr );
        for( size_t i = 0 ; i < aargs->v.size() ; i++ ) {
            aargs->v[i]->encode( cc, false, value );
            llvm::Value* vv = check_value_1( value ); // TODO: 暫定
            value.clear();

            assert( vv );
            //std::cerr << "arg: " << *vv->getType() << std::endl;
            args.push_back( vv );
        }

/*
        std::cerr << "final fptr: "
                  << *fptr->getType() << ", "
                  << *fptr_type << ", "
                  << *func_type
                  << std::endl;
        for( size_t i = 0 ; i < args.size() ; i++ ) {
            std::cerr << "arg" << i << "f: " << *func_type->getParamType(i)
                      << std::endl;
            std::cerr << "arg" << i << "a: " << *args[i]->getType()
                      << std::endl;
        }
*/        
        sprintf( reg, "ret%d", h.id );

        int n = Type::getTupleSize( r.t->getReturnType() );
        if( n == 1 ) {
            value.assign(
                llvm::CallInst::Create(
                    fptr, args.begin(), args.end(), reg, cc.bb ),
                r.t->getReturnType() );
        } else {
            llvm::Value* ret = llvm::CallInst::Create(
                fptr, args.begin(), args.end(), reg, cc.bb );
            for( int i = 0 ; i < n ; i++ ) {
                sprintf( reg, "ret%d_%d", h.id, i );
                llvm::Instruction* lv = llvm::ExtractValueInst::Create(
                    ret, i, reg, cc.bb );

                Value av;
                av.assign( lv, Type::getElementType( r.t, i ) );
                value.add( av );
            }
        }
    } else {
        //std::cerr << Type::getDisplay( v.t ) << std::endl;
        assert(0);
    }

}
void FunCall::entype( EntypeContext& tc, bool, type_t t )
{
    type_t ft = tc.env.find( func->s );
    //std::cerr << "Funcall::entype " << func->s->s << " => "
    //<< Type::getDisplay( ft->getReturnType() ) << std::endl;
    if( !Type::isCallable( ft ) ) {
        throw uncallable(
            h.beg, func->s->s + "(" + Type::getDisplay( ft ) + ")" );
    }

    // 戻り値とコンテキスト型がミスマッチ
    if( t && !Type::match( ft->getReturnType(), t ) ) {
        throw context_mismatch(
            h.beg,
            func->s->s + "(" + Type::getDisplay( ft->getReturnType() ) + ")",
            Type::getDisplay( t ) );
    }

    // 引数の個数が合わない
    int farity = Type::getTupleSize( ft->getArgumentType() );
    if( int( aargs->v.size() ) != farity ) {
        throw wrong_arity(
            h.beg,
            aargs->v.size(),
            farity,
            func->s->s );
    }

    // 実引数の型付け
    for( size_t i = 0 ; i < aargs->v.size() ; i++ ) {
        aargs->v[i]->entype(
            tc, false, ft->getArgumentType()->getElement(i) );
    }

    update_type( tc, h, ft->getReturnType() );

    tc.env.refer( func->s, h.t );
}

////////////////////////////////////////////////////////////////
// Lambda
void Lambda::encode( EncodeContext& cc, bool drop_value, Value& value )
{
    check_empty( value );

    // lambda-lifted functionの作成
    llvm::Function* raw_function = encode_function(
        cc,
        drop_value,
        h,
        name,
        fargs,
        result_type,
        body,
        freevars );

    // closure-env型の作成
    std::vector< const llvm::Type* > v;
    for( symmap_t::const_iterator i = freevars.begin() ;
         i != freevars.end() ;
         ++i ) {
        v.push_back( getLLVMType( (*i).second ) );
    }
    llvm::Type* closure_env_type = llvm::StructType::get( v );
    //std::cerr << "closure_env_type: " << *closure_env_type << std::endl;

    // stub function型の作成
    
    // ...arguments
    std::vector< const llvm::Type* > atypes;
    atypes.push_back( llvm::PointerType::getUnqual( closure_env_type ) );
    for( size_t i = 0 ; i < fargs->v.size() ; i++ ) {
        atypes.push_back( getLLVMType( typeexpr_to_type( fargs->v[i]->t ) ) );
    }

    // ...result
    const llvm::Type* rtype = raw_function->getReturnType();

    // ...function type
    llvm::FunctionType* stub_ft =
        llvm::FunctionType::get(
            rtype, atypes, /* not vararg */ false );
    //std::cerr << "stub_ft: " << *stub_ft << std::endl;

    // closure型の作成
    v.clear();
    v.push_back( llvm::PointerType::getUnqual( stub_ft ) );
    v.push_back( closure_env_type );
    llvm::Type* closure_type = llvm::StructType::get( v );
    //std::cerr << "closure_type: " << *closure_type << std::endl;

    // ...function
    llvm::Function* stub_f =
        llvm::Function::Create(
            stub_ft,
            llvm::Function::ExternalLinkage,
            name->s + "_stub",
            cc.m );
    
    // ...basic block
    llvm::BasicBlock* bb = llvm::BasicBlock::Create("ENTRY", stub_f);

    // ... stub => raw arguments
    llvm::Function::arg_iterator ai = stub_f->arg_begin();
    std::vector< llvm::Value* > args;

    ai->setName( "env" );
    llvm::Value* env = ai++;

    char reg[256];
    sprintf( reg, "env%d", h.id );

    llvm::Value* indices[3];
    indices[0] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );
    indices[1] = NULL;

    for( size_t i = 0 ; i < freevars.size() ; i++ ) {
        sprintf( reg, "freevar_a%d_%d", h.id, int(i) );
        indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, i );
        llvm::Value* fv = llvm::GetElementPtrInst::Create(
            env, indices, indices+2, reg, bb );

        sprintf( reg, "freevar_v%d_%d", h.id, int(i) );
        llvm::Value* v = new llvm::LoadInst( fv, reg, bb );
        args.push_back( v );
    }

    int fargs_index = 0;
    for( llvm::Function::arg_iterator i = ai; i != stub_f->arg_end() ; ++i ) {
        i->setName( fargs->v[fargs_index++]->name->s->s );
        args.push_back( i );
    }

    sprintf( reg, "call_%d", h.id );
    llvm::Value* stub_v = llvm::CallInst::Create(
        raw_function, args.begin(), args.end(), reg, bb );

    bb->getInstList().push_back( llvm::ReturnInst::Create(stub_v) );


    sprintf( reg, "closure_%d", h.id );
    llvm::Value* closure =
        new llvm::MallocInst( 
            closure_type,
            reg,
            cc.bb );

    indices[0] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );
    indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );
    indices[2] = NULL;

    sprintf( reg, "stub_func_addr_%d", h.id );
    llvm::Value* stub_func_addr = llvm::GetElementPtrInst::Create(
        closure, indices, indices+2, reg, cc.bb );

    new llvm::StoreInst( 
        stub_f,
        stub_func_addr,
        cc.bb );

    int no = 0;
    indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 1 );
    for( symmap_t::const_iterator i = freevars.begin() ;
         i != freevars.end() ;
         ++i ) {
        indices[2] = llvm::ConstantInt::get( llvm::Type::Int32Ty, no++ );

        sprintf( reg, "freevar_addr_%d", h.id );
        llvm::Value* freevar_addr = llvm::GetElementPtrInst::Create(
            closure, indices, indices+3, reg, cc.bb );

        new llvm::StoreInst(
            cc.env.find( (*i).first ).v,
            freevar_addr,
            cc.bb );
    }

    sprintf( reg, "casted_closure_%d", h.id );
    llvm::Value* casted_closure =
        new llvm::BitCastInst(
            closure, getLLVMType( h.t ), reg, cc.bb );

    value.assign( casted_closure, h.t );
}
void Lambda::entype( EntypeContext& tc, bool drop_value, type_t t )
{
    if( !name ) {
        name = tc.ce.gensym();
    }

    entype_function(
        tc,
        drop_value,
        t,
        h,
        name,
        fargs,
        result_type,
        body,
        freevars );

    assert( Type::isFunction( h.t ) );

    update_type( tc, h, Type::getClosureType( h.t ) );
}

////////////////////////////////////////////////////////////////
// ActualArgs
void ActualArgs::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void ActualArgs::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// ActualArg
void ActualArg::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    expr->encode( cc, false, value );
}
void ActualArg::entype( EntypeContext& tc, bool, type_t t )
{
    expr->entype( tc, false, t );
    update_type( tc, h, expr->h.t );
}

////////////////////////////////////////////////////////////////
// Identifier
void Identifier::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}
void Identifier::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

} // namespace leaf
