// 2008/08/13 Naoyuki Hirayama

#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include "leaf_node.hpp"
#include "leaf_ast.hpp"
#include "leaf_error.hpp"
#include "leaf_environment.hpp"

namespace leaf {

////////////////////////////////////////////////////////////////
// EntypeContext
struct EntypeContext : public boost::noncopyable {
    EntypeContext( CompileEnv& ace ) : ce(ace) {}
    
    CompileEnv&                 ce;
    Environment< type_t >       env;
};

////////////////////////////////////////////////////////////////
// utility functions
inline
void update_type( EntypeContext& tc, Header& h, type_t t )
{
	// TODO: 重要
    if( t && h.t != t ) { h.t = t; }
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

static type_t
entype_formalargs( EntypeContext& tc, FormalArgs* formalargs )
{
    typevec_t v;
    for( size_t i = 0 ; i < formalargs->v.size() ; i++ ) {
        if( !formalargs->v[i]->t ) {
            return NULL;
        }
		formalargs->v[i]->t->entype( tc, false, NULL );
		type_t t = formalargs->v[i]->t->h.t;
		update_type( tc, formalargs->v[i]->h, t );
        v.push_back( t );
    }
    return Type::getTupleType( v );
}

void freevars_to_typevec( const symmap_t& freevars, typevec_t& v )
{
    for( symmap_t::const_iterator i = freevars.begin() ;
         i != freevars.end() ;
         ++i ) {
        v.push_back( (*i).second );
    }
}

void
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
	result_type->entype( tc, false, NULL );
	type_t rttype = result_type->h.t;
    if( !Type::isComplete( rttype ) ) {
        // あり得ないはず
        assert(0);
        throw imcomplete_return_type( h.beg );
    }

    if( Type::isFunction( rttype ) ) {
        rttype = Type::getClosureType( rttype );
    }

    // 引数の型
    type_t attype = entype_formalargs( tc, formal_args );
    if( !Type::isComplete( attype ) ) {
        throw inexplicit_argument_type( h.beg );
    }

    // 再帰関数のために本体より先にbind
    update_type( tc, h, Type::getFunctionType( rttype, attype ) );
    tc.env.bind( funcname, h.t );

    tc.env.push();
    for( size_t i = 0 ; i < formal_args->v.size() ; i++ ) {
		formal_args->v[i]->t->entype( tc, false, NULL );
        tc.env.bind(
            formal_args->v[i]->name->s,
            formal_args->v[i]->t->h.t );
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

type_t make_tuple_tree_from_vardeclelems( EntypeContext& tc, VarDeclElems* f )
{
    typevec_t v;
    for( size_t i = 0 ; i < f->v.size() ; i++ ) {
        VarDeclElem* e = f->v[i];

        VarDeclElems* ev = dynamic_cast<VarDeclElems*>(e);
        if( ev ) {
            v.push_back( make_tuple_tree_from_vardeclelems( tc, ev ) );
            continue;
        }

        VarDeclIdentifier* ei = dynamic_cast<VarDeclIdentifier*>(e);
        if( ei ) {
            if( ei->t ) {
				ei->t->entype( tc, false, NULL );
                v.push_back( ei->t->h.t );
            } else {
                v.push_back( NULL );
            }
            continue;
        }

        assert(0);
    }

    return Type::getTupleType( v );
}

template < class T >
T* make_header( CompileEnv& ce, T* p )
{
	p->h.id = ce.idseed++;
	return p;
}

template < class T >
T* make_header( CompileEnv& ce, const Header& header, T* p )
{
	p->h = header;
	p->h.id = ce.idseed++;
	return p;
}

Statements* expand_block_sugar( CompileEnv& ce, Statements* s, type_t t )
{
	assert( t );

	// for LLVM
	bool have_try = false;
	bool finally = false;
	for( size_t i = 0 ; i < s->v.size() ; i++ ) {
		if( dynamic_cast<SectionLabel*>(s->v[i]) ) {
			if( i != s->v.size() - 1 &&
				!dynamic_cast<SectionLabel*>(s->v[i+1]) ) {
				have_try = true;
				break;
			}
			if( dynamic_cast<FinallyLabel*>(s->v[i]) ) {
				finally = true;
			} else {
				if( finally ) {
					throw finally_must_be_the_last_section( s->h.beg );
				}
			}
		}
	}

	if( !have_try ) {
		// そのまま
		return s;
	} else {
		// fun foo(): void
		// {
		//     bar();
		// finally:
		//     baz();
		// }
		// を
		// fun foo(): void
		// {
		//     fun gensym1(): void
		//     {
		//         bar();
		//     }
		//     gensym1();
		//  finally:
		//     baz();
		//  }
		// に変換する

		// 加工する
		std::vector< Statement* > sv;
		bool try_section_done = false;
		for( size_t i = 0 ; i < s->v.size() ; i++ ) {
			if( !try_section_done &&
				dynamic_cast<SectionLabel*>(s->v[i]) ) {

				// try section done

				// ...make internal function
				Symbol* funname = ce.gensym();
				FunSig* funsig = make_header(
					ce, s->h,
					ce.cage.allocate<FunSig>(
						ce.cage.allocate<Identifier>( funname ),
						make_header(
							ce, s->h,
							ce.cage.allocate<FormalArgs>() ),
						ce.cage.allocate<TypeRef>( t ) ) );
				Block* block = make_header(
					ce, s->h,
					ce.cage.allocate<Block>( 
						ce.cage.allocate<Statements>( sv ),
						false ) );
				FunDef* fundef = make_header(
					ce, s->h,
					ce.cage.allocate<FunDef>( funsig, block, symmap_t() ) );

				// ...make function call
				FunCall* funcall = make_header(
					ce, s->h,
					ce.cage.allocate<FunCall>(
						ce.cage.allocate<Identifier>( funname ),
						ce.cage.allocate<ActualArgs>() ) );

				sv.push_back( fundef );
				sv.push_back(
					make_header(
						ce, s->h,
						ce.cage.allocate<MultiExpr>( funcall ) ) );

				try_section_done = true;
			}
			sv.push_back( s->v[i] );
		}
		
		return ce.cage.allocate<Statements>( sv );
	}
}

////////////////////////////////////////////////////////////////
// Node
void Node::entype( CompileEnv& ce )
{
    EntypeContext tc( ce );
    tc.env.push();
    entype( tc, false, NULL );
    tc.env.pop();
}

////////////////////////////////////////////////////////////////
// Module
void Module::entype( EntypeContext& tc, bool, type_t t )
{
    topelems->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopElems
void TopElems::entype( EntypeContext& tc, bool, type_t t )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->entype( tc, false, t );
    }
}

////////////////////////////////////////////////////////////////
// TopElem
void TopElem::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Require
void Require::entype( EntypeContext& tc, bool, type_t t )
{
    module->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDecl
void TopLevelFunDecl::entype( EntypeContext& tc, bool, type_t t )
{
    fundecl->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDef
void TopLevelFunDef::entype( EntypeContext& tc, bool, type_t t )
{
    fundef->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// TopLevelStructDef
void TopLevelStructDef::entype( EntypeContext& tc, bool, type_t t )
{
    structdef->entype( tc, false, t );
}

////////////////////////////////////////////////////////////////
// Block
void Block::entype( EntypeContext& tc, bool, type_t t )
{
	//expand_block_sugar( cc.ce, statements, h.t )

	if( !expanded && h.t ) {
		statements = expand_block_sugar( tc.ce, statements, h.t );
		expanded = true;
	}

    statements->entype( tc, false, t );

    if( !statements->v.empty() ) {
        update_type( tc, h, statements->v.back()->h.t );
    }
}

////////////////////////////////////////////////////////////////
// Statements
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
void Statement::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FunDecl
void FunDecl::entype( EntypeContext& tc, bool, type_t t )
{
    // 戻り値の型
	sig->result_type->entype( tc, false, NULL );
    type_t result_type = sig->result_type->h.t;
    if( !Type::isComplete( result_type ) ) {
        throw imcomplete_return_type( h.beg );
    }

    // 引数の型
    type_t args_type = entype_formalargs( tc, sig->fargs );
    if( !Type::isComplete( args_type ) ) {
        throw inexplicit_argument_type( h.beg );
    }

    update_type( tc, h, Type::getFunctionType( result_type, args_type ) );
    tc.env.bind( sig->name->s, h.t );
}

////////////////////////////////////////////////////////////////
// FunDef
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
void FunSig::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// StructDef
void StructDef::entype( EntypeContext& tc, bool, type_t t )
{
    std::vector< Slot > sv;
    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        Member* m = members->v[i];
		m->farg->t->entype( tc, false, NULL );
        sv.push_back( Slot( m->farg->name->s, m->farg->t->h.t ) );
    }
    update_type( tc, h, Type::getStructType( sv ) );
    tc.env.bind( name->s, h.t );
}

////////////////////////////////////////////////////////////////
// Slots
void Members::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Member
void Member::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArgs
void FormalArgs::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArg
void FormalArg::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDecl
void VarDecl::entype( EntypeContext& tc, bool drop_value, type_t )
{
    if( !drop_value ) {
        throw unused_variable( h.beg, "@@@@" );
    }

    varelems->entype( tc, drop_value, NULL );
    type_t ft = make_tuple_tree_from_vardeclelems( tc, varelems );

  retry:
    data->entype( tc, false, ft );
    type_t at = data->h.t;
    
    if( ft != at ) {
        type_t ut = Type::unify( ft, at );
        if( !ut ) {
            throw type_mismatch(
                h.beg,
                Type::getDisplay( ft ) + " at variable" ,
                Type::getDisplay( at ) + " at value" );
        }
        varelems->entype( tc, false, ut );
        data->entype( tc, false, ut );
        ft = ut;
        goto retry;
    }
}

////////////////////////////////////////////////////////////////
// VarDeclElem
void VarDeclElem::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDeclElems
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
void TypeExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// NamedType
void NamedType::entype( EntypeContext& tc, bool, type_t t )
{
	update_type( tc, h, tc.env.find( name->s ) );
}

////////////////////////////////////////////////////////////////
// Types
void Types::entype( EntypeContext& tc, bool, type_t )
{
	typevec_t tv;
	for( size_t i = 0 ; i < v.size() ; i++ ) {
		v[i]->entype( tc, false, NULL );
		tv.push_back( v[i]->h.t );
	}
	update_type( tc, h, Type::getTupleType( tv ) );
}

////////////////////////////////////////////////////////////////
// TypeRef
void TypeRef::entype( EntypeContext& tc, bool, type_t )
{
	update_type( tc, h, this->t );
}

////////////////////////////////////////////////////////////////
// MultiExpr
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
void Expr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOrElems::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    for( size_t i = 0 ; i < v.size(); i++ ) {
        v[i]->entype( tc, false, Type::getBoolType() );
    }
}

////////////////////////////////////////////////////////////////
// LogicalAnd
void LogicalAnd::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalAndElems
void LogicalAndElems::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    for( size_t i = 0 ; i < v.size(); i++ ) {
        v[i]->entype( tc, false, Type::getBoolType() );
    }
}

////////////////////////////////////////////////////////////////
// Equality
void Equality::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// EqualityEq
void EqualityEq::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// EqualityNe
void EqualityNe::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// Relational
void Relational::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// RelationalLt
void RelationalLt::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// RelationalGt
void RelationalGt::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// RelationalLe
void RelationalLe::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// RelationalGe
void RelationalGe::entype( EntypeContext& tc, bool, type_t t )
{
    check_bool_expr_type( h, t );
    entype_int_compare( tc, *this );
}

////////////////////////////////////////////////////////////////
// Additive
void Additive::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// AddExpr
void AddExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// SubExpr
void SubExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// Multiplicative
void Multiplicative::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MulExpr
void MulExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// DivExpr
void DivExpr::entype( EntypeContext& tc, bool, type_t t )
{
    check_int_expr_type( h, t );
    entype_int_binary_arithmetic( tc, *this );
}

////////////////////////////////////////////////////////////////
// PrimExpr
void PrimExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralBoolean
void LiteralBoolean::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && t != Type::getBoolType() ) {
        throw context_mismatch( h.beg, "<bool>", Type::getDisplay( t ) );
    }
    update_type( tc, h, Type::getBoolType() );
}

////////////////////////////////////////////////////////////////
// LiteralInteger
void LiteralInteger::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && t != Type::getIntType() ) {
        throw context_mismatch( h.beg, "<int>", Type::getDisplay( t ) );
    }
    update_type( tc, h, Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// LiteralChar
void LiteralChar::entype( EntypeContext& tc, bool, type_t t )
{
    if( t && t != Type::getIntType() ) {
        throw context_mismatch( h.beg, "<int>", Type::getDisplay( t ) );
    }
    update_type( tc, h, Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// VarRef
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
void Parenthized::entype( EntypeContext& tc, bool, type_t t )
{
    exprs->entype( tc, false, t );
    update_type( tc, h, exprs->h.t );
}

////////////////////////////////////////////////////////////////
// CastExpr
void CastExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Cast
void Cast::entype( EntypeContext& tc, bool, type_t t )
{
	this->t->entype( tc, false, NULL );
    type_t tt = this->t->h.t;
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
// MemberExpr
void MemberExpr::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MemberRef
void MemberRef::entype( EntypeContext& tc, bool, type_t t )
{
	expr->entype( tc, false, NULL );
	if( !expr->h.t ) { return; }

	if( !Type::isStruct( expr->h.t ) ) {
		throw wrong_memberref(
			h.beg, Type::getDisplay( expr->h.t ), field->s->s );
	}

	int slot_index = expr->h.t->getSlotIndex( field->s );
	if( slot_index < 0 ) {
		throw wrong_memberref(
			h.beg, Type::getDisplay( expr->h.t ), field->s->s );
	}

	update_type( tc, h, expr->h.t->getSlot( slot_index ).type );

	if( !Type::match( h.t, t ) ) {
        throw context_mismatch(
			h.beg, Type::getDisplay( h.t ), Type::getDisplay( t ) );
	}
}

////////////////////////////////////////////////////////////////
// FunCall
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
// LiteralStruct
void LiteralStruct::entype( EntypeContext& tc, bool, type_t t )
{
    type_t st = tc.env.find( name->s );
    if( !Type::isStruct( st ) ) {
        throw not_struct( h.beg, name->s->s );
    }

    std::vector<bool> used( st->Type::getSlotCount(), false );
    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        LiteralMember* m = members->v[i];
        int index = st->getSlotIndex( m->name->s );
        if( index < 0 ) {
            throw no_such_member( m->h.beg, name->s->s, m->name->s->s );
        }
		
		m->data->entype(
			tc,
			false,
			Type::unify( st->getSlot( index ).type, m->data->h.t ) );

        assert( index <= int( used.size() ) ); 
        used[index] = true;
    }

    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        if( !used[i] ) {
            throw not_initialized_member(
                h.beg, name->s->s, members->v[i]->name->s->s );
        }
    }

    update_type( tc, h, st );
}

////////////////////////////////////////////////////////////////
// LiteralMembers
void LiteralMembers::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralMember
void LiteralMember::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Lambda
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
void ActualArgs::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// ActualArg
void ActualArg::entype( EntypeContext& tc, bool, type_t t )
{
    expr->entype( tc, false, t );
    update_type( tc, h, expr->h.t );
}

////////////////////////////////////////////////////////////////
// SectionLabel
void SectionLabel::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// CatchLabel
void CatchLabel::entype( EntypeContext& tc, bool, type_t t )
{
}

////////////////////////////////////////////////////////////////
// FinallyLabel
void FinallyLabel::entype( EntypeContext& tc, bool, type_t t )
{
}

////////////////////////////////////////////////////////////////
// ThrowStatement
void ThrowStatement::entype( EntypeContext& tc, bool, type_t t )
{
}

////////////////////////////////////////////////////////////////
// Identifier
void Identifier::entype( EntypeContext& tc, bool, type_t t )
{
    assert(0);
}

} // namespace leaf

