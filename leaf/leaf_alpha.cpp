// 2008/10/23 Naoyuki Hirayama

#include "leaf_ast.hpp"
#include <vector>
#include <map>

namespace leaf {

////////////////////////////////////////////////////////////////
// AlphaContext
struct AlphaContext {
	struct StackFrame {
		std::map< Symbol*, Symbol* > dic;
	};

    CompileEnv&                 ce;
	std::vector< StackFrame >	stack;

	AlphaContext( CompileEnv& e ) : ce( e )
	{
	}

	void push()
	{
		stack.push_back( StackFrame() );
	}
	void pop()
	{
		stack.pop_back();
	}

	Symbol* find( Symbol* x )
	{
		for( int i = int(stack.size()-1) ; 0 <= i ; i-- ) {
			const StackFrame& f = stack[i];
			std::map< Symbol*, Symbol* >::const_iterator i = f.dic.find( x );
			if( i != f.dic.end() ) {
				return (*i).second;
			}
		}
		return NULL;
	}
	
	Symbol* unshadow( Symbol* source )
	{
		if( find( source ) ) {
			Symbol* s = ce.gensym();
			stack.back().dic[source] = s;
			return s;
		} else {
			stack.back().dic[source] = source;
			return source;
		}
	}

	Symbol* freshname( Symbol* s )
	{
		Symbol* r = find( s );
		if( r ) {
			return r;
		} else {
			return s;
		}
	}

	
	void dump_stack()
	{
		for( int i = int(stack.size()-1) ; 0 <= i ; i-- ) {
			const StackFrame& f = stack[i];
			
			for( std::map< Symbol*, Symbol* >::const_iterator i =
					 f.dic.begin();
				 i != f.dic.end() ;
				 ++i ) {
				std::cerr << (*i).first->s << " <= " << (*i).second->s
						  << std::endl;
			}
		}
	}
};

////////////////////////////////////////////////////////////////
// Node
void Node::alpha( CompileEnv& ce )
{
	AlphaContext ac( ce );
	alpha( ac );
}

////////////////////////////////////////////////////////////////
// Module
void Module::alpha( AlphaContext& ac )
{
    topelems->alpha( ac );
}

////////////////////////////////////////////////////////////////
// TopElems
void TopElems::alpha( AlphaContext& ac )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// TopElem
void TopElem::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Require
void Require::alpha( AlphaContext& ac )
{
    module->alpha( ac );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDecl
void TopLevelFunDecl::alpha( AlphaContext& ac )
{
    fundecl->alpha( ac );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDef
void TopLevelFunDef::alpha( AlphaContext& ac )
{
    fundef->alpha( ac );
}

////////////////////////////////////////////////////////////////
// TopLevelStructDef
void TopLevelStructDef::alpha( AlphaContext& ac )
{
    structdef->alpha( ac );
}

////////////////////////////////////////////////////////////////
// Block
void Block::alpha( AlphaContext& ac )
{
	ac.push();
    statements->alpha( ac );
	ac.pop();
}

////////////////////////////////////////////////////////////////
// Statements
void Statements::alpha( AlphaContext& ac )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
		v[i]->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// Statement
void Statement::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FunDecl
void FunDecl::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// FunDef
void FunDef::alpha( AlphaContext& ac )
{
	ac.push();
	sig->name->fresh = ac.unshadow( sig->name->source );
	for( size_t i = 0 ; i < sig->fargs->v.size() ; i++ ) {
		FormalArg* fa = sig->fargs->v[i];
		fa->name->fresh = ac.unshadow( fa->name->source );
	}
	body->alpha( ac );
	ac.pop();
}

////////////////////////////////////////////////////////////////
// FunSig
void FunSig::alpha( AlphaContext& ac )
{
	assert(0);
}

////////////////////////////////////////////////////////////////
// StructDef
void StructDef::alpha( AlphaContext& ac )
{
	// TODO: 要検討
}

////////////////////////////////////////////////////////////////
// Slots
void Members::alpha( AlphaContext& ac )
{
	// TODO: 要検討
	assert(0);
}

////////////////////////////////////////////////////////////////
// Member
void Member::alpha( AlphaContext& ac )
{
	// TODO: 要検討
	assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArgs
void FormalArgs::alpha( AlphaContext& ac )
{
	assert(0);
}

////////////////////////////////////////////////////////////////
// FormalArg
void FormalArg::alpha( AlphaContext& ac )
{
	assert(0);
}

////////////////////////////////////////////////////////////////
// VarDecl
void VarDecl::alpha( AlphaContext& ac )
{
    data->alpha( ac );
    varelems->alpha( ac );
}

////////////////////////////////////////////////////////////////
// VarDeclElem
void VarDeclElem::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDeclElems
void VarDeclElems::alpha( AlphaContext& ac )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// VarDeclIdentifier
void VarDeclIdentifier::alpha( AlphaContext& ac )
{
	name->fresh = ac.unshadow( name->source );
}

////////////////////////////////////////////////////////////////
// IfThenElse
void IfThenElse::alpha( AlphaContext& ac )
{
    cond->alpha( ac );
    iftrue->alpha( ac );
    iffalse->alpha( ac );
}

////////////////////////////////////////////////////////////////
// MultiExpr
void MultiExpr::alpha( AlphaContext& ac )
{
	for( size_t i = 0 ; i < v.size() ; i++ ) {
		v[i]->alpha( ac );
	}
}

////////////////////////////////////////////////////////////////
// Expr
void Expr::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOr::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOrElems::alpha( AlphaContext& ac )
{
    for( size_t i = 0 ; i < v.size(); i++ ) {
        v[i]->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// LogicalAnd
void LogicalAnd::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalAndElems
void LogicalAndElems::alpha( AlphaContext& ac )
{
    for( size_t i = 0 ; i < v.size(); i++ ) {
        v[i]->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// Equality
void Equality::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// EqualityEq
void EqualityEq::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// EqualityNe
void EqualityNe::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// Relational
void Relational::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// RelationalLt
void RelationalLt::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// RelationalGt
void RelationalGt::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// RelationalLe
void RelationalLe::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// RelationalGe
void RelationalGe::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// Additive
void Additive::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// AddExpr
void AddExpr::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// SubExpr
void SubExpr::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// Multiplicative
void Multiplicative::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MulExpr
void MulExpr::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// DivExpr
void DivExpr::alpha( AlphaContext& ac )
{
	lhs->alpha( ac );
	rhs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// CastExpr
void CastExpr::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Cast
void Cast::alpha( AlphaContext& ac )
{
    expr->alpha( ac );
}

////////////////////////////////////////////////////////////////
// MemberExpr
void MemberExpr::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MemberRef
void MemberRef::alpha( AlphaContext& ac )
{
	expr->alpha( ac );
}

////////////////////////////////////////////////////////////////
// PrimExpr
void PrimExpr::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralBoolean
void LiteralBoolean::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// LiteralInteger
void LiteralInteger::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// LiteralChar
void LiteralChar::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// VarRef
void VarRef::alpha( AlphaContext& ac )
{
	name->fresh = ac.freshname( name->source );
}

////////////////////////////////////////////////////////////////
// Parenthized
void Parenthized::alpha( AlphaContext& ac )
{
    exprs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// FunCall
void FunCall::alpha( AlphaContext& ac )
{
	func->fresh = ac.freshname( func->source );
	aargs->alpha( ac );
}

////////////////////////////////////////////////////////////////
// LiteralStruct
void LiteralStruct::alpha( AlphaContext& ac )
{
	// TODO: 要検討
    for( size_t i = 0 ; i < members->v.size() ; i++ ) {
		members->v[i]->data->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// LiteralMembers
void LiteralMembers::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralMember
void LiteralMember::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Lambda
void Lambda::alpha( AlphaContext& ac )
{
	ac.push();
	for( size_t i = 0 ; i < fargs->v.size() ; i++ ) {
		FormalArg* fa = fargs->v[i];
		fa->name->fresh = ac.unshadow( fa->name->source );
	}
	body->alpha( ac );
	ac.pop();
}

////////////////////////////////////////////////////////////////
// ActualArgs
void ActualArgs::alpha( AlphaContext& ac )
{
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        v[i]->alpha( ac );
    }
}

////////////////////////////////////////////////////////////////
// ActualArg
void ActualArg::alpha( AlphaContext& ac )
{
    expr->alpha( ac );
}

////////////////////////////////////////////////////////////////
// SectionLabel
void SectionLabel::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// CatchLabel
void CatchLabel::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// FinallyLabel
void FinallyLabel::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// ThrowStatement
void ThrowStatement::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// Invoke
void Invoke::alpha( AlphaContext& ac )
{
	func->alpha( ac );
}

////////////////////////////////////////////////////////////////
// Identifier
void Identifier::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// TypeExpr
void TypeExpr::alpha( AlphaContext& ac )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// NamedType
void NamedType::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// Types
void Types::alpha( AlphaContext& ac )
{
}

////////////////////////////////////////////////////////////////
// TypeRef
void TypeRef::alpha( AlphaContext& ac )
{
}

} // namespace leaf

