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
#include <cstdarg>
#include "leaf_node.hpp"
#include "leaf_ast.hpp"
#include "leaf_error.hpp"
#include "leaf_environment.hpp"

namespace leaf {

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
private:
    enum Category {
        C_NULL,
        C_SCALOR,
        C_MULTIPLE,
        C_STRUCT,
    };

public:
    Value() { c_ = C_NULL; x_ = NULL; t_ = NULL; }
    ~Value() {}
    Value& operator=( const Value& v )
    {
        c_ = v.c_;
        x_ = v.x_;
        t_ = v.t_;
        v_ = v.v_;
        return *this;
    }

    bool empty() const
    {
        return c_ == C_NULL && !x_ && !t_ && v_.empty();
    }

    void clear()
    {
        c_ = C_NULL;
        x_ = NULL;
        t_ = NULL;
        v_.clear();
    }

    void add( const Value& v )
    {
        assert( !x_ );
        c_ = C_MULTIPLE;
        v_.push_back( v );
    }

    bool isMultiple() const { return c_ == C_MULTIPLE ; }
    llvm::Value* getx() const { return x_; }
    type_t gett() const { return t_; }
    const std::vector< Value >& getv() const { return v_; }
    void sett( type_t t ) { t_ = t; }

    void assign_as_scalor( llvm::Value* x, type_t t )
    {
        c_ = C_SCALOR;
        x_ = x;
        t_ = t;
    }
    void assign_as_multiple( const std::vector< Value >& v )
    {
        c_ = C_MULTIPLE;
        v_ = v;
    }
    void assign_as_struct( const std::vector< Value >& v )
    {
        c_ = C_STRUCT;
        v_ = v;
    }

    int size() const
    {
        if( c_ != C_MULTIPLE ) {
            return 1;
        } else {
            return v_.size();
        }
    }
    const Value& operator[] ( int i ) const
    {
        assert( c_ == C_MULTIPLE );
        return v_[i];
    }


private:
    Category                c_;
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
    EncodeContext( CompileEnv& ace ) : ce(ace) {}

	CompileEnv&					ce;
    llvm::Module*               m;
    llvm::Function*             f;
    llvm::BasicBlock*           bb;
    Environment< Reference >    env;
};

////////////////////////////////////////////////////////////////
// reg
class Reg {
public:
	Reg( const char* s, ... )
	{
		va_list ap;
		va_start( ap, s );
		vsprintf( buffer_, s, ap );
		va_end( ap );
	}

	void make( const char* s, ... )
	{
		va_list ap;
		va_start( ap, s );
		vsprintf( buffer_, s, ap );
		va_end( ap );
	}

	operator std::string() const
	{
		return buffer_;
	}

private:
	char buffer_[256];
};

////////////////////////////////////////////////////////////////
// utility functions
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

    value.assign_as_scalor( 
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
encode_binary_operator(
    int                             id,
    EncodeContext&                  cc,
    LHS*                            lhs,
    RHS*                            rhs,
    llvm::Instruction::BinaryOps    op,
    Value&                          value )
{
    char reg[256]; 
	sprintf( reg, "reg_%d", id );

    lhs->encode( cc, false, value );
    llvm::Value* v0 = check_value_1( value );
    value.clear();

    rhs->encode( cc, false, value );
    llvm::Value* v1 = check_value_1( value );
    value.clear();

    llvm::Instruction* inst = llvm::BinaryOperator::create(
        op, v0, v1, reg );
    cc.bb->getInstList().push_back(inst);

    value.assign_as_scalor( inst, Type::getIntType() );
}

inline
void check_empty( const Value& value )
{
    assert( value.empty() );
}

const llvm::Type* getLLVMType( type_t t, bool add_env = false )
{
    assert( t );

    switch( t->tag() ) {
    case Type::TAG_BOOL: return llvm::Type::Int1Ty;
    case Type::TAG_CHAR: return llvm::Type::Int8Ty;
    case Type::TAG_SHORT: return llvm::Type::Int16Ty;
    case Type::TAG_INT: return llvm::Type::Int32Ty;
    case Type::TAG_LONG: return llvm::Type::Int64Ty;
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
    case Type::TAG_STRUCT:
        {
            int n = t->Type::getSlotCount();

            std::vector< const llvm::Type* > v;
            for( int i = 0 ; i < n ; i++ ) {
                v.push_back( getLLVMType( t->getSlot(i).type ) );
            }
            return llvm::StructType::get( v );
        }
    default:
        return llvm::Type::Int32Ty;
    }
}

llvm::Value*
encode_function_return_value(
    EncodeContext&  cc,
    type_t          t,
    const char*     reg_prefix,
    const Value&    value )
{
    assert( value.size() == Type::getTupleSize( t ) );

    if( value.size() == 1 ) {
        return value.getx();
    } else {
        llvm::Value* undef = llvm::UndefValue::get( getLLVMType( t ) ); 
        llvm::Value* prev = undef;
        for( int i = 0 ; i < value.size() ; i++ ) {
            char reg[256];
            sprintf( reg, "%s_%d", reg_prefix, i );
            llvm::Value* arg = encode_function_return_value(
                cc,
                Type::getElementType( t, i ),
                reg,
                value[i] );
            prev = llvm::InsertValueInst::Create(
                prev, arg, i, reg, cc.bb ) ;
        }
        return prev;
    }
}

llvm::Function*
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
            getLLVMType( formal_args->v[i]->h.t ) );
    }

    // ...result
    type_t leaf_rtype = result_type->h.t;
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
        char reg[256];
        sprintf( reg, "insval%d", h.id );

        llvm::ReturnInst::Create( 
            encode_function_return_value(
                cc,
                h.t->getReturnType(),
                reg,
                value ),
            cc.bb );
    }

    std::swap( cc.bb, bb );

    return f;
}

void encode_vardecl( EncodeContext& cc, VarDeclElem* f, const Value& a )
{
    if( VarDeclElems* fv = dynamic_cast<VarDeclElems*>(f) ) {
        if( a.size() == 1 ) {
            encode_vardecl( cc, fv->v[0], a );
        } else {
            for( size_t i = 0 ; i < fv->v.size() ; i++ ) {
                encode_vardecl( cc, fv->v[i], a[i] );
            }
        }
        return;
    }

    if( VarDeclIdentifier* fi = dynamic_cast<VarDeclIdentifier*>(f) ) {
        cc.env.bind(
            fi->name->s, Reference( a.getx(), a.gett(), symmap_t() ) );
        return;
    }

    assert(0);
}

////////////////////////////////////////////////////////////////
// Node
void Node::encode( CompileEnv& ce, llvm::Module* m )
{
    EncodeContext cc( ce );
    cc.m = m;
    cc.f = NULL;
    cc.bb = NULL;
    cc.env.push();
    Value value;
    encode( cc, false, value );
    cc.env.pop();
}

////////////////////////////////////////////////////////////////
// Module
void Module::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    topelems->encode( cc, false, value );
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

////////////////////////////////////////////////////////////////
// TopElem
void TopElem::encode( EncodeContext& cc, bool, Value& value )
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

////////////////////////////////////////////////////////////////
// TopLevelFunDecl
void TopLevelFunDecl::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    fundecl->encode( cc, false, value );
    value.clear();
}

////////////////////////////////////////////////////////////////
// TopLevelFunDef
void TopLevelFunDef::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    fundef->encode( cc, false, value );
    value.clear();
}

////////////////////////////////////////////////////////////////
// TopLevelStructDef
void TopLevelStructDef::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    structdef->encode( cc, false, value );
    value.clear();
}

////////////////////////////////////////////////////////////////
// Block
void Block::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

	statements->encode( cc, false, value );
}

////////////////////////////////////////////////////////////////
// Statements
void Statements::encode( EncodeContext& cc, bool drop_value, Value& value )
{
    check_empty( value );

	// セクションの作成
    std::vector< Statement* > sections;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        if( SectionLabel* s = dynamic_cast<SectionLabel*>(v[i]) ) {
			if( i != v.size() - 1 && !dynamic_cast<SectionLabel*>(v[i+1]) ) {
                sections.push_back( s );
            }
        }
    }

	// エンコード
	if( sections.empty() ) {
		for( size_t i = 0 ; i < v.size() ; i++ ) {
			bool this_drop_value = drop_value || i != v.size() - 1;

			v[i]->encode( cc, this_drop_value, value );
			if( this_drop_value ) {
				value.clear();
			}
		}
	} else {
		llvm::Function* f = cc.f;

		llvm::BasicBlock* normal_bb = llvm::BasicBlock::Create( "NORMAL", f );
		llvm::BasicBlock* catch_bb = llvm::BasicBlock::Create( "CATCH", f );
		llvm::BasicBlock* finally = NULL;
	
		llvm::Value* invoke = NULL;
		for( size_t i = 0 ; i < v.size() ; i++ ) {
			bool this_drop_value = drop_value || i != v.size() - 1;
			 
			if( dynamic_cast<FinallyLabel*>(v[i]) ) {
				finally = llvm::BasicBlock::Create( "FINALLY", f );
			} else if( SectionLabel* s = dynamic_cast<SectionLabel*>(v[i]) ) {
				if( i != v.size() - 1 &&
					!dynamic_cast<SectionLabel*>(v[i+1]) ) {
					cc.bb = llvm::BasicBlock::Create(
						Reg( "sec_%d", s->h.id ), f );
				}
			} else {
				if( Invoke* inv = dynamic_cast<Invoke*>(v[i]) ) {	
					invoke = llvm::InvokeInst::Create(
						cc.m->getFunction( inv->func->s->s ),
						normal_bb,
						catch_bb,
						(llvm::Value**)NULL,(llvm::Value**)NULL,
						(std::string)Reg( "invoke_%d", inv->h.id ),
						cc.bb );
					continue;
				}
				v[i]->encode( cc, this_drop_value, value );
				if( this_drop_value ) {
					value.clear();
				}
			}
		}

        llvm::ReturnInst::Create( invoke, normal_bb );
        llvm::ReturnInst::Create( invoke, catch_bb ); // TODO: ダミー
	}
}

////////////////////////////////////////////////////////////////
// Statement
void Statement::encode( EncodeContext& cc, bool, Value& value )
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

////////////////////////////////////////////////////////////////
// FunSig
void FunSig::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// StructDef
void StructDef::encode( EncodeContext& cc, bool, Value& value )
{
}

////////////////////////////////////////////////////////////////
// Slots
void Members::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Member
void Member::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArgs
void FormalArgs::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArg
void FormalArg::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDecl
void VarDecl::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );
    data->encode( cc, false, value );
    encode_vardecl( cc, this->varelems, value );
    value.clear();
}

////////////////////////////////////////////////////////////////
// VarDeclElem
void VarDeclElem::encode( EncodeContext&, bool, Value& )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDeclElems
void VarDeclElems::encode( EncodeContext&, bool, Value& )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDeclIdentifier
void VarDeclIdentifier::encode( EncodeContext&, bool, Value& )
{
    assert(0);
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
    
            value.assign_as_scalor( phi, iftrue->h.t );
        } else {
            for( int i = 0 ; i < n ; i++ ) {
                char reg[256];
                sprintf( reg, "phi_%d_%d", h.id, i );
                
                phi = llvm::PHINode::Create( iftype, reg, cc.bb );
                phi->addIncoming( tvalue.getx(), tbb );
                phi->addIncoming( fvalue.getx(), fbb );

                Value av; av.assign_as_scalor(
                    phi, Type::getElementType( iftrue->h.t, i ) );
                value.add( av );
            }
        }
    } else {
        value.assign_as_scalor( NULL, iftrue->h.t );
    }
}

////////////////////////////////////////////////////////////////
// TypeExpr
void TypeExpr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// NamedType
void NamedType::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Types
void Types::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// TypeRef
void TypeRef::encode( EncodeContext& cc, bool, Value& value )
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
        value.sett( h.t );
        for( size_t i = 0 ; i < v.size() ; i++ ) {
            Value v;
            this->v[i]->encode( cc, drop_value, v );
            value.add( v );
        }
    }
}

////////////////////////////////////////////////////////////////
// Expr
void Expr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOr::encode( EncodeContext& cc, bool, Value& value )
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

    value.assign_as_scalor(
        new llvm::LoadInst( orresult, label, final ),
        Type::getBoolType() );
}

////////////////////////////////////////////////////////////////
// LogicalAnd
void LogicalAnd::encode( EncodeContext& cc, bool, Value& value )
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
    value.assign_as_scalor(
        new llvm::LoadInst( andresult, label, final ),
        Type::getBoolType() );
}

////////////////////////////////////////////////////////////////
// Equality
void Equality::encode( EncodeContext& cc, bool, Value& value )
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

////////////////////////////////////////////////////////////////
// EqualityNe
void EqualityNe::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "ne", llvm::ICmpInst::ICMP_NE, cc, value );
}

////////////////////////////////////////////////////////////////
// Relational
void Relational::encode( EncodeContext& cc, bool, Value& value )
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

////////////////////////////////////////////////////////////////
// RelationalGt
void RelationalGt::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "gt", llvm::ICmpInst::ICMP_SGT, cc, value );
}

////////////////////////////////////////////////////////////////
// RelationalLe
void RelationalLe::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "le", llvm::ICmpInst::ICMP_SLE, cc, value );
}

////////////////////////////////////////////////////////////////
// RelationalGe
void RelationalGe::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_int_compare( *this, "ge", llvm::ICmpInst::ICMP_SGE, cc, value );
}

////////////////////////////////////////////////////////////////
// Additive
void Additive::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// AddExpr
void AddExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_binary_operator(
		h.id, cc, lhs, rhs, llvm::Instruction::Add, value );
}

////////////////////////////////////////////////////////////////
// SubExpr
void SubExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_binary_operator(
		h.id, cc, lhs, rhs, llvm::Instruction::Sub, value );
}

////////////////////////////////////////////////////////////////
// Multiplicative
void Multiplicative::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MulExpr
void MulExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_binary_operator(
		h.id, cc, lhs, rhs, llvm::Instruction::Mul, value );
}

////////////////////////////////////////////////////////////////
// DivExpr
void DivExpr::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    encode_binary_operator(
		h.id, cc, lhs, rhs, llvm::Instruction::SDiv, value );
}

////////////////////////////////////////////////////////////////
// PrimExpr
void PrimExpr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralBoolean
void LiteralBoolean::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    value.assign_as_scalor( 
        data ?
        llvm::ConstantInt::getTrue() :
        llvm::ConstantInt::getFalse(),
        Type::getBoolType() );
}

////////////////////////////////////////////////////////////////
// LiteralInteger
void LiteralInteger::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    value.assign_as_scalor(
        llvm::ConstantInt::get( llvm::Type::Int32Ty, data ),
        Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// LiteralChar
void LiteralChar::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    value.assign_as_scalor(
        llvm::ConstantInt::get( llvm::Type::Int32Ty, data ),
        Type::getIntType() );
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
    value.assign_as_scalor( r.v, r.t );
}

////////////////////////////////////////////////////////////////
// Parenthized
void Parenthized::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    exprs->encode( cc, false, value );
}

////////////////////////////////////////////////////////////////
// CastExpr
void CastExpr::encode( EncodeContext& cc, bool, Value& value )
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

    value.assign_as_scalor( 
        llvm::CastInst::createIntegerCast(
            expr_value,
            getLLVMType( this->h.t ),
            true,
            reg,
            cc.bb ),
        this->h.t );
}

////////////////////////////////////////////////////////////////
// MemberExpr
void MemberExpr::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MemberRef
void MemberRef::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

	expr->encode( cc, false, value );
    llvm::Value* expr_value = check_value_1( value );
    value.clear();

	int slot_index = expr->h.t->getSlotIndex( field->s );
	assert( 0 <= slot_index );

	char reg[256];
	sprintf( reg, "memref%d_%d", h.id, slot_index );
	llvm::Instruction* lv = llvm::ExtractValueInst::Create(
		expr_value, slot_index, reg, cc.bb );

	value.assign_as_scalor( lv, h.t );
}

////////////////////////////////////////////////////////////////
// FunCall
void
encode_funcall_return_value(
    const Header&   h,
    EncodeContext&  cc,
    llvm::Value*    ret,
    type_t          t,
    Value&          value )
{
    int n = Type::getTupleSize( t );
    if( n == 1 ) {
        value.assign_as_scalor( ret, t );
    } else {
        for( int i = 0 ; i < n ; i++ ) {
            char reg[256];
            sprintf( reg, "ret%d_%d", h.id, i );
            llvm::Instruction* lv = llvm::ExtractValueInst::Create(
                ret, i, reg, cc.bb );

            Value av;
            encode_funcall_return_value(
                h, cc, lv, Type::getElementType( t, i ), av );
            value.add( av );
        }
    }
}

void FunCall::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    Reference r = cc.env.find( func->s );
    if( !r.t ) {
        throw no_such_function( h.beg, func->s->s );
    }

	if( Type::isFunction( r.t ) ) {
        llvm::Function* f = cc.m->getFunction( func->s->s );

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

        llvm::Value* ret = llvm::CallInst::Create(
            f, args.begin(), args.end(), reg, cc.bb );

        encode_funcall_return_value(
            h,
            cc,
            ret,
            r.t->getReturnType(),
            value );

    } else if( Type::isClosure( r.t ) ) {
        char reg[256];

        llvm::Value* indices[2];
        indices[0] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );
        indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 0 );

        // stub func type
        sprintf( reg, "stub_func_addr%d", h.id );
        llvm::Value* faddr = llvm::GetElementPtrInst::Create(
            r.v, indices, indices+2, reg, cc.bb );

        sprintf( reg, "stub_func_ptr%d", h.id );
        llvm::Value* fptr = new llvm::LoadInst( faddr, reg, cc.bb );

        // stub env type
        indices[1] = llvm::ConstantInt::get( llvm::Type::Int32Ty, 1 );

        sprintf( reg, "stub_env_addr%d", h.id );
        llvm::Value* eaddr = llvm::GetElementPtrInst::Create(
            r.v, indices, indices+2, reg, cc.bb );

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

        sprintf( reg, "ret%d", h.id );

        int n = Type::getTupleSize( r.t->getReturnType() );
        if( n == 1 ) {
            value.assign_as_scalor(
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
                av.assign_as_scalor( lv, Type::getElementType( r.t, i ) );
                value.add( av );
            }
        }
    } else {
        assert(0);
    }

}

////////////////////////////////////////////////////////////////
// Invoke
void Invoke::encode( EncodeContext& cc, bool drop_value, Value& value )
{
	assert(0);
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
        atypes.push_back( getLLVMType( fargs->v[i]->h.t ) );
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

    value.assign_as_scalor( casted_closure, h.t );
}

////////////////////////////////////////////////////////////////
// LiteralStruct
void LiteralStruct::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    assert( Type::isStruct( h.t ) );
    std::vector< Value > values( h.t->getSlotCount() );

    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        LiteralMember* m = members->v[i];

        int index = h.t->getSlotIndex( m->name->s );
        assert( 0 <= index );

        m->encode( cc, false, values[index] );
    }

    const llvm::Type* st = getLLVMType( h.t );
    llvm::Value* prev = llvm::UndefValue::get( st ); 
    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        char reg[256];
        sprintf( reg, "mem%d_%d", h.id, int(i) );

        prev = llvm::InsertValueInst::Create(
            prev, values[i].getx(), i, reg, cc.bb );
    }   

    value.assign_as_scalor( prev, h.t );
}

////////////////////////////////////////////////////////////////
// LiteralMembers
void LiteralMembers::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralMember
void LiteralMember::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );

    data->encode( cc, false, value );
}

////////////////////////////////////////////////////////////////
// ActualArgs
void ActualArgs::encode( EncodeContext& cc, bool, Value& value )
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

////////////////////////////////////////////////////////////////
// SectionLabel
void SectionLabel::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// CatchLabel
void CatchLabel::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FinallyLabel
void FinallyLabel::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// ThrowStatement
void ThrowStatement::encode( EncodeContext& cc, bool, Value& value )
{
    check_empty( value );
	new llvm::UnwindInst( cc.bb );
}

////////////////////////////////////////////////////////////////
// Identifier
void Identifier::encode( EncodeContext& cc, bool, Value& value )
{
    assert(0);
}

} // namespace leaf
