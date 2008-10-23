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
    EntypeContext( CompileEnv& ace ) : ce(ace), updated(false) {}
    
    CompileEnv&                 ce;
    Environment< type_t >       env;
	bool						updated;
};

////////////////////////////////////////////////////////////////
// utility functions
inline
std::string disptype( type_t t )
{
	return Type::getDisplay( t );
}

inline
void update_type( EntypeContext& tc, Header& h, type_t t )
{
	assert( Type::unify( h.t, t ) == t );
	
    if( h.t != t ) {
		h.t = t;
		tc.updated = true;
	}
}

inline
void check_bool_expr_type( const Header& h, type_t t )
{
    if( t && t != Type::getBoolType() ) {
        throw context_mismatch( h.beg, disptype( t ), "<bool>" );
    }
}

inline
void check_int_expr_type( const Header& h, type_t t )
{
    if( t && t != Type::getIntType() ) {
        throw context_mismatch( h.beg, disptype( t ), "<int>" );
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

	// ...戻り値が関数型だったらクロージャに変換
    if( Type::isFunction( rttype ) ) {
        rttype = Type::getClosureType( rttype );
    }

    // 引数の型
	formal_args->entype( tc, false, NULL );
    type_t attype = formal_args->h.t;

    // 再帰関数のために本体より先にbind
	// TODO: ローカル関数が再帰関数だったとき、これで動くか？
    update_type(
		tc, h, Type::unify( h.t, Type::getFunctionType( rttype, attype )  ) );
    tc.env.bind( funcname, h.t );

    tc.env.push();
    for( size_t i = 0 ; i < formal_args->v.size() ; i++ ) {
        tc.env.bind(
            formal_args->v[i]->name->source,
            formal_args->v[i]->h.t );
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

    // 自由変数
    for( symmap_t::iterator i = tc.env.freevars().begin();
         i != tc.env.freevars().end() ;
         ++i ) {
        Symbol* s = (*i).first;
        if( !tc.env.find_in_toplevel( s ) ) {
            freevars[s] = (*i).second;
        }
    }

    // 引数の型をアップデート
    for( size_t i = 0 ; i < formal_args->v.size() ; i++ ) {
        type_t t = tc.env.find( formal_args->v[i]->name->source );
        if( !t ) {
            // TODO: 引数の型を決定できないerror
        }
        formal_args->v[i]->h.t = t;
    }

    tc.env.pop();
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
				Header h = s->h;
				h.t = NULL;

				// ...make internal function
				Symbol* funname = ce.gensym();
				FunSig* funsig = make_header(
					ce, h,
					ce.cage.allocate<FunSig>(
						ce.cage.allocate<Identifier>(
							funname, (Symbol*)NULL ),
						make_header(
							ce, h,
							ce.cage.allocate<FormalArgs>() ),
						ce.cage.allocate<TypeRef>( t ) ) );
				Block* block = make_header(
					ce, h,
					ce.cage.allocate<Block>( 
						ce.cage.allocate<Statements>( sv ),
						false ) );
				FunDef* fundef = make_header(
					ce, h,
					ce.cage.allocate<FunDef>( funsig, block, symmap_t() ) );

				// ...make function call
				Invoke* invoke = make_header(
					ce, h,
					ce.cage.allocate<Invoke>(
						ce.cage.allocate<Identifier>(
							funname, (Symbol*)NULL ) ) );

				sv.clear();
				sv.push_back( fundef );
				sv.push_back( invoke );

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

  retry:
    tc.env.push();
    entype( tc, false, NULL );
    tc.env.pop();

	if( tc.updated ) {
		tc.updated = false;
		goto retry;
	}
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
void Block::entype( EntypeContext& tc, bool drop_value, type_t t )
{
	if( !expanded && h.t ) {
		statements = expand_block_sugar( tc.ce, statements, h.t );
		expanded = true;
	}

    statements->entype( tc, drop_value, t );
	update_type( tc, h, statements->h.t );
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

	if( !v.empty() ) {
		update_type( tc, h, v.back()->h.t );
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
	sig->fargs->entype( tc, false, NULL );
    type_t args_type = sig->fargs->h.t;
    if( !Type::isComplete( args_type ) ) {
        throw inexplicit_argument_type( h.beg );
    }

    update_type( tc, h, Type::getFunctionType( result_type, args_type ) );
    tc.env.bind( sig->name->source, h.t );
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
        sig->name->source,
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
void StructDef::entype( EntypeContext& tc, bool, type_t )
{
	members->entype( tc, false, NULL );
    update_type( tc, h, members->h.t );
    tc.env.bind( name->source, h.t );
}

////////////////////////////////////////////////////////////////
// Slots
void Members::entype( EntypeContext& tc, bool, type_t )
{
    std::vector< Slot > sv;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
		FormalArg* farg = v[i]->farg;
		farg->entype( tc, false, NULL );
        sv.push_back( Slot( farg->name->source, farg->h.t ) );
    }
    update_type( tc, h, Type::getStructType( sv ) );
}

////////////////////////////////////////////////////////////////
// Member
void Member::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArgs
void FormalArgs::entype( EntypeContext& tc, bool, type_t )
{
    typevec_t tv;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
		v[i]->entype( tc, false, NULL );
        tv.push_back( v[i]->h.t );
    }

	update_type( tc, h, Type::getTupleType( tv ) );
}

////////////////////////////////////////////////////////////////
// FormalArg
void FormalArg::entype( EntypeContext& tc, bool, type_t )
{
	this->t->entype( tc, false, NULL );
	update_type( tc, h, this->t->h.t );
}

////////////////////////////////////////////////////////////////
// VarDecl
void VarDecl::entype( EntypeContext& tc, bool drop_value, type_t )
{
    if( !drop_value ) {
        throw unused_variable( h.beg, "@@@@" );
    }

	type_t t = NULL;
  retry:
    varelems->entype( tc, drop_value, t );
    data->entype( tc, false, t );

    type_t ft = varelems->h.t;
    type_t at = data->h.t;
	if( !Type::match( ft, at ) ) {
		throw type_mismatch(
			h.beg,
			disptype( ft ) + " at variable" ,
			disptype( at ) + " at value" );
	}

	type_t ut = Type::unify( ft, at );
	if( ft != ut || at != ut ) {
		t = ut;
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

    if( t ) {
		int n = Type::getTupleSize( t );
		if( int( v.size() ) != n ) {
			throw wrong_multiple_value( h.beg, v.size(), n );
		}
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
    if( !Type::match( h.t, t ) ) {
        throw type_mismatch(
            h.beg,
            disptype( h.t ) + " at variabne" ,
            disptype( t ) + " at value" );
    }
    update_type( tc, h, Type::unify( h.t, t ) );
    tc.env.bind( name->source, h.t );
}

////////////////////////////////////////////////////////////////
// IfThenElse
void IfThenElse::entype( EntypeContext& tc, bool drop_value, type_t t )
{
    cond->entype( tc, false, Type::getBoolType() );

  retry:
    iftrue->entype( tc, drop_value, t );
    iffalse->entype( tc, drop_value, t );

    if( !drop_value ) {
		type_t tt = iftrue->h.t;
		type_t ft = iffalse->h.t;
									
		if( Type::match( tt, ft ) ) {
			type_t ut = Type::unify( tt, ft );
			if( t != ut ) {
				t = ut;
				goto retry;
			}
		} else {
			throw type_mismatch(
				h.beg,
				disptype( iftrue->h.t ) + " at true-clause" ,
				disptype( iffalse->h.t ) + " at false-clause" );
		}
    }

    update_type( tc, h, iftrue->h.t );
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
            if( n != int( v.size() ) ) {
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
void Expr::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOr::entype( EntypeContext&, bool, type_t )
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
void LogicalAnd::entype( EntypeContext&, bool, type_t )
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
void Equality::entype( EntypeContext&, bool, type_t )
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
void Relational::entype( EntypeContext&, bool, type_t )
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
void Additive::entype( EntypeContext&, bool, type_t )
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
void Multiplicative::entype( EntypeContext&, bool, type_t )
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
void PrimExpr::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralBoolean
void LiteralBoolean::entype( EntypeContext& tc, bool, type_t t )
{
    if( !Type::match( Type::getBoolType(), t ) ) {
        throw context_mismatch( h.beg, "<bool>", disptype( t ) );
    }
    update_type( tc, h, Type::getBoolType() );
}

////////////////////////////////////////////////////////////////
// LiteralInteger
void LiteralInteger::entype( EntypeContext& tc, bool, type_t t )
{
    if( !Type::match( Type::getIntType(), t ) ) {
        throw context_mismatch( h.beg, "<int>", disptype( t ) );
    }
    update_type( tc, h, Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// LiteralChar
void LiteralChar::entype( EntypeContext& tc, bool, type_t t )
{
    if( !Type::match( Type::getIntType(), t ) ) {
        throw context_mismatch( h.beg, "<int>", disptype( t ) );
    }
    update_type( tc, h, Type::getIntType() );
}

////////////////////////////////////////////////////////////////
// VarRef
void VarRef::entype( EntypeContext& tc, bool, type_t t )
{
    type_t vt = tc.env.find( name->source );

	if( !Type::match( vt, t ) ) {
		throw context_mismatch(
			h.beg, disptype( vt ), disptype( t ) );
	} else {
		type_t ut = Type::unify( vt, t );
        tc.env.update( name->source, ut );
        update_type( tc, h, ut );
    }        

    tc.env.refer( name->source, h.t );
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
void CastExpr::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Cast
void Cast::entype( EntypeContext& tc, bool, type_t t )
{
	this->t->entype( tc, false, NULL );

    type_t ct = this->t->h.t;
	if( !Type::isComplete( ct ) ) {
		throw cast_to_imcomplete_type( h.beg, disptype( ct ) );
	}

    if( !Type::match( ct, t ) ) {
        throw context_mismatch( h.beg, disptype( ct ), disptype( t ) );
    }

    expr->entype( tc, false, NULL );
    update_type( tc, h, ct );
}

////////////////////////////////////////////////////////////////
// MemberExpr
void MemberExpr::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MemberRef
void MemberRef::entype( EntypeContext& tc, bool, type_t t )
{
	expr->entype( tc, false, NULL );

	type_t st = expr->h.t;
	if( !st ) { return; }

	if( !Type::isStruct( st ) ) {
		throw wrong_memberref( h.beg, disptype( st ), field->source->s );
	}

	int slot_index = st->getSlotIndex( field->source );
	if( slot_index < 0 ) {
		throw wrong_memberref( h.beg, disptype( st ), field->source->s );
	}

	update_type( tc, h, st->getSlot( slot_index ).type );

	if( !Type::match( h.t, t ) ) {
        throw context_mismatch( h.beg, disptype( h.t ), disptype( t ) );
	}
}

////////////////////////////////////////////////////////////////
// FunCall
void FunCall::entype( EntypeContext& tc, bool, type_t t )
{
    type_t ft = tc.env.find( func->source );
    if( !Type::isCallable( ft ) ) {
        throw uncallable(
			h.beg, func->source->s + "(" + disptype( ft ) + ")" );
    }

    // 戻り値とコンテキスト型がミスマッチ
	type_t rt = ft->getReturnType();
    if( !Type::match( rt, t ) ) {
        throw context_mismatch(
            h.beg,
			func->source->s + "(" + disptype( rt ) + ")",
			disptype( t ) );
    }

    // 引数の個数が合わない
	type_t at = ft->getArgumentType();
    int farity = Type::getTupleSize( at );
    if( int( aargs->v.size() ) != farity ) {
        throw wrong_arity(
            h.beg, aargs->v.size(), farity, func->source->s );
    }

    // 実引数の型付け
	aargs->entype( tc, false, at );

    tc.env.refer( func->source, ft );

	update_type( tc, h, rt );
}

////////////////////////////////////////////////////////////////
// Invoke
void Invoke::entype( EntypeContext& tc, bool, type_t t )
{
    type_t ft = tc.env.find( func->source );
    if( !Type::isCallable( ft ) ) {
        throw uncallable(
			h.beg, func->source->s + "(" + disptype( ft ) + ")" );
    }

    // 戻り値とコンテキスト型がミスマッチ
	type_t rt = ft->getReturnType();
    if( !Type::match( rt, t ) ) {
        throw context_mismatch(
            h.beg,
			func->source->s + "(" + disptype( rt ) + ")", 
			disptype( t ) );
    }

    tc.env.refer( func->source, ft );
	update_type( tc, h, rt );
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

    if( Type::isFunction( h.t ) ) {
		update_type( tc, h, Type::getClosureType( h.t ) );
	}
}

////////////////////////////////////////////////////////////////
// LiteralStruct
void LiteralStruct::entype( EntypeContext& tc, bool, type_t t )
{
    type_t st = tc.env.find( name->source );
    if( !Type::isStruct( st ) ) {
        throw not_struct( h.beg, name->source->s );
    }

    std::vector<bool> used( st->getSlotCount(), false );
    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        LiteralMember* m = members->v[i];
        int index = st->getSlotIndex( m->name->source );
        if( index < 0 ) {
            throw no_such_member(
				m->h.beg, name->source->s, m->name->source->s );
        }
		
		m->data->entype( tc, false, st->getSlot( index ).type );

        assert( index <= int( used.size() ) ); 
        used[index] = true;
    }

    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
        if( !used[i] ) {
            throw not_initialized_member(
                h.beg, name->source->s, members->v[i]->name->source->s );
        }
    }

	update_type( tc, h, st );
}

////////////////////////////////////////////////////////////////
// LiteralMembers
void LiteralMembers::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralMember
void LiteralMember::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// ActualArgs
void ActualArgs::entype( EntypeContext& tc, bool, type_t t )
{
	typevec_t tv;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->entype( tc, false, Type::getElementType( t, i ) );
		tv.push_back( v[i]->h.t );
    }
	update_type( tc, h, Type::getTupleType( tv ) );
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
void SectionLabel::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// CatchLabel
void CatchLabel::entype( EntypeContext&, bool, type_t )
{
}

////////////////////////////////////////////////////////////////
// FinallyLabel
void FinallyLabel::entype( EntypeContext&, bool, type_t )
{
}

////////////////////////////////////////////////////////////////
// ThrowStatement
void ThrowStatement::entype( EntypeContext&, bool, type_t )
{
}

////////////////////////////////////////////////////////////////
// Identifier
void Identifier::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// TypeExpr
void TypeExpr::entype( EntypeContext&, bool, type_t )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// NamedType
void NamedType::entype( EntypeContext& tc, bool, type_t )
{
	update_type( tc, h, tc.env.find( name->source ) );
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

} // namespace leaf

