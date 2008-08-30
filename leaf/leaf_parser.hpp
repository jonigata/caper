// 2008/08/11 Naoyuki Hirayama

/*!
	@file	  leaf_reader.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef LEAF_READER_HPP_
#define LEAF_READER_HPP_

#include "leaf_node.hpp"
#include "leaf_ast.hpp"
#include "leaf_grammar.hpp"
#include "leaf_scanner.hpp"
#include "leaf_error.hpp"
#include "leaf_compile.hpp"
#include <limits>
#include <fstream>

struct SemanticAction {
	SemanticAction( leaf::SymDic& sdic, heap_cage& c, int& idseed )
		: symdic( sdic ), cage( c ), id( idseed ) {}

	leaf::SymDic&	symdic;
	heap_cage&		cage;
	int				id;

	leaf::Type* anytype() { return (leaf::Type*)NULL; }

	template < class T >
	T* h( T* p )
	{
		p->h.beg = (std::numeric_limits<int>::max)();
		p->h.end = (std::numeric_limits<int>::min)();
		p->h.id = id++;
		return p;
	}

	template < class T >
	T* h( const leaf::Header& header, T* p )
	{
		p->h = header;
		p->h.id = id++;
		return p;
	}

	template < class T, class U >
	T* makeSeq1( U* x )
	{
		std::vector< U* > v;
		v.push_back( x );
		return h( x->h, cage.allocate<T>( v ) );
	}


	template < class T, class U >
	T* append( T* x, U* y )
	{
		x->h += y->h;
		x->v.push_back( y );
		return x;
	}

	void syntax_error() {}
	void stack_overflow(){}

	template < class T0, class T1 >
	void downcast( T0& x, T1 y ) { x = dynamic_cast<T0>(y); }

	template < class T0, class T1 >
	void upcast( T0& x, T1 y ) { x = y; }

	template < class T >
	T* identity( T* x ) { return x; }

	leaf::Module* makeModule( leaf::TopElems* s )
	{
		return h( s->h, cage.allocate<leaf::Module>( s ) );
	}

	leaf::TopElems* makeTopElems0( leaf::TopElem* e )
	{
		return makeSeq1<leaf::TopElems>( e );
	}

	leaf::TopElems* makeTopElems1( leaf::TopElems* s, leaf::TopElem* e )
	{
		return append( s, e );
	}

	leaf::Statements* makeStatements0()
	{
		return h( cage.allocate<leaf::Statements>() );
	}

	leaf::Statements* makeStatements1( leaf::Statements* ss,
									   leaf::Statement* s )
	{
		return append( ss, s );
	}

	leaf::Require* makeRequire( leaf::Identifier* i )
	{
		std::string filename = "rtl/" + i->s->s + ".lh";

		std::ifstream ifs( filename.c_str() );
		if( !ifs ) {
			throw leaf::require_fail( i->h.beg, filename );
		}
	
		// スキャナ
		typedef std::istreambuf_iterator<char> is_iterator;
		is_iterator b( ifs );	 // 即値にするとVC++が頓珍漢なことを言う
		is_iterator e;
		scanner_type s( b, e, symdic, cage, id );

		leaf::Node* v = NULL;
		try {
			v = read_from_file( filename, s );
		}
		catch( leaf::error& e ) {
			if( !e.caught() ) {
				e.set_info( filename, s.lineno( e.addr ), s.column( e.addr ) );
			}
			throw;
		}

		leaf::Module* m = dynamic_cast<leaf::Module*>(v);
		assert( m );

		return h( i->h, cage.allocate<leaf::Require>( i, m ) );
	}

	leaf::FunDecl* makeFunDecl( leaf::FunSig* s )
	{
		return h( s->h, cage.allocate<leaf::FunDecl>( s ) );
	}

	leaf::FunDef* makeFunDef( leaf::FunSig* s, leaf::Block* b )
	{
		return h( s->h + b->h, cage.allocate<leaf::FunDef>( s, b ) );
	}

	leaf::FunSig* makeFunSig0( leaf::Identifier* i,
							   leaf::FormalArgs* fa )
	{
		return h( i->h + fa->h, cage.allocate<leaf::FunSig>(
					  i, fa, (leaf::Types*)NULL ) );
	}

	leaf::FunSig* makeFunSig1( leaf::Identifier* i,
							   leaf::FormalArgs* fa,
							   leaf::Types* t )
	{
		return h( i->h + fa->h + t->h,
				  cage.allocate<leaf::FunSig>( i, fa, t ) );
	}

	leaf::FormalArgs* makeFormalArgs0()
	{
		return h( cage.allocate<leaf::FormalArgs>() );
	}

	leaf::FormalArgs* makeFormalArgs1( leaf::FormalArg* fa )
	{
		return makeSeq1<leaf::FormalArgs>( fa );
	}

	leaf::FormalArgs* makeFormalArgs2( leaf::FormalArgs* fargs,
									   leaf::FormalArg* fa )
	{
		return append( fargs, fa );
	}

	leaf::FormalArg* makeFormalArg0( leaf::Identifier* i )
	{
#if 0
		return h( i->h, cage.allocate<leaf::FormalArg>(
					  i, (leaf::TypeRef*)NULL ) );
#else
		throw leaf::formalarg_must_be_typed( i->h.beg, i->s->s );
#endif
	}

	leaf::FormalArg* makeFormalArg1( leaf::Identifier* i, leaf::TypeRef* t )
	{
		return h( i->h + t->h,
				  cage.allocate<leaf::FormalArg>( i, t ) );
	}

	leaf::Block* makeBlock( leaf::Statements* s )
	{
		return h( s->h, cage.allocate<leaf::Block>( s ) );
	}

	leaf::VarDecl* makeVarDecl0( leaf::Identifier* i, leaf::Expr* e )
	{
		return h( i->h + e->h, cage.allocate<leaf::VarDecl>(
					  i, (leaf::TypeRef*)NULL, e ) );
	}

	leaf::VarDecl* makeVarDecl1( leaf::Identifier* i,
								 leaf::TypeRef* t,
								 leaf::Expr* e )
	{
		return h( i->h + t->h + e->h,
				  cage.allocate<leaf::VarDecl>( i, t, e ) );
	}

	leaf::IfThenElse* makeIfThenElse0( leaf::Expr* c,
									   leaf::Block* t,
									   leaf::Block* f )
	{
		return h( c->h + t->h + f->h,
				  cage.allocate<leaf::IfThenElse>( c, t, f ) );
	}

	leaf::IfThenElse* makeIfThenElse1( leaf::Expr* c,
									   leaf::Block* t,
									   leaf::Statement* ite )
	{
		leaf::Block* else_clause =
			h( ite->h, cage.allocate<leaf::Block>(
				   makeSeq1<leaf::Statements>( ite ) ) );
		
		return h( c->h + t->h + ite->h,
				  cage.allocate<leaf::IfThenElse>( c, t, else_clause ) );
	}

	leaf::TypeRef* makeTypeVoid()
	{
		return h( cage.allocate<leaf::TypeRef>( leaf::Type::getVoidType() ) );
	}

	leaf::TypeRef* makeTypeLong()
	{
		return h( cage.allocate<leaf::TypeRef>( leaf::Type::getLongType() ) );
	}

	leaf::TypeRef* makeTypeInt()
	{
		return h( cage.allocate<leaf::TypeRef>( leaf::Type::getIntType() ) );
	}

	leaf::TypeRef* makeTypeShort()
	{
		return h( cage.allocate<leaf::TypeRef>( leaf::Type::getShortType() ) );
	}

	leaf::TypeRef* makeTypeChar()
	{
		return h( cage.allocate<leaf::TypeRef>( leaf::Type::getCharType() ) );
	}

	leaf::TypeRef* makeFunctionType(
		leaf::Types* atypes, leaf::Types* rtype )
	{
		return NULL;
	}

	leaf::Types* makeTypes0()
	{
		return h( cage.allocate<leaf::Types>() );
	}
	
	leaf::Types* makeTypes1( leaf::TypeRef* t )
	{
		return makeSeq1<leaf::Types>( t );
	}
	
	leaf::Types* makeTypes2( leaf::Types* x, leaf::TypeRef* y )
	{
		return append( x, y );
	}
	
	leaf::LogicalOr* makeLogicalOr0( leaf::LogicalAnd* y )
	{
		return makeSeq1<leaf::LogicalOr>( y );
	}

	leaf::LogicalOr* makeLogicalOr1( leaf::LogicalOr* x, leaf::LogicalAnd* y )
	{
		return append( x, y );
	}

	leaf::LogicalAnd* makeLogicalAnd0( leaf::Equality* y )
	{
		return makeSeq1<leaf::LogicalAnd>( y );
	}

	leaf::LogicalAnd* makeLogicalAnd1( leaf::LogicalAnd* x, leaf::Equality* y )
	{
		return append( x, y );
	}

	leaf::Equality* makeEq( leaf::Equality* x, leaf::Relational* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::EqualityEq>( x, y ) );
	}

	leaf::Equality* makeNe( leaf::Equality* x, leaf::Relational* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::EqualityNe>( x, y ) );
	}

	leaf::Relational* makeLt( leaf::Relational* x, leaf::Additive* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::RelationalLt>( x, y ) );
	}

	leaf::Relational* makeGt( leaf::Relational* x, leaf::Additive* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::RelationalGt>( x, y ) );
	}

	leaf::Relational* makeLe( leaf::Relational* x, leaf::Additive* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::RelationalLe>( x, y ) );
	}

	leaf::Relational* makeGe( leaf::Relational* x, leaf::Additive* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::RelationalGe>( x, y ) );
	}

	leaf::Additive* makeAdd( leaf::Additive* x, leaf::Multiplicative* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::AddExpr>( x, y ) );
	}
	leaf::Additive* makeSub( leaf::Additive* x, leaf::Multiplicative* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::SubExpr>( x, y ) );
	}

	leaf::Multiplicative* makeMul( leaf::Multiplicative* x, leaf::PrimExpr* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::MulExpr>( x, y ) );
	}
	leaf::Multiplicative* makeDiv( leaf::Multiplicative* x, leaf::PrimExpr* y )
	{
		return h( x->h + y->h, cage.allocate<leaf::DivExpr>( x, y ) );
	}

	leaf::PrimExpr* makeVarRef( leaf::Identifier* i )
	{
		return h( i->h, cage.allocate<leaf::VarRef>( i ) );
	}
	leaf::PrimExpr* makeParenthized( leaf::Expr* e )
	{
		return h( e->h, cage.allocate<leaf::Parenthized>( e ) );
	}

	leaf::PrimExpr* makeFunCall0( leaf::Identifier* func )
	{
		return h( func->h,
				  cage.allocate<leaf::FunCall>(
					  func, cage.allocate<leaf::ActualArgs>() ) );
	}

	leaf::PrimExpr* makeFunCall1( leaf::Identifier* func,
								 leaf::ActualArgs* aargs )
	{
		return h( func->h + aargs->h,
				  cage.allocate<leaf::FunCall>( func, aargs ) );
	}

	leaf::ActualArgs* makeActualArgs0( leaf::ActualArg* fa )
	{
		return makeSeq1<leaf::ActualArgs>( fa );
	}
	leaf::ActualArgs* makeActualArgs1( leaf::ActualArgs* aargs,
									   leaf::ActualArg* aa )
	{
		return append( aargs, aa );
	}
	leaf::ActualArg* makeActualArg( leaf::Expr* e )
	{
		return h( e->h, cage.allocate<leaf::ActualArg>( e ) );
	}
};

#endif // LEAF_READER_HPP_
