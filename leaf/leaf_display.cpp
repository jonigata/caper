// 2008/10/02 Naoyuki Hirayama

#include "leaf_ast.hpp"
#include <iostream>

namespace leaf {

struct idt {
public:
	idt( int n ) : n_(n) {}
	int n_;
};
std::ostream& operator<<( std::ostream& os, const idt& x )
{
	for( int i = 0 ; i < x.n_*2 ; i++ ) {
		os.put(' ');
	}
	return os;
}

struct dsp {
public:
	dsp( int indent, Node* n ) : indent_(indent), n_(n) {}
	int indent_;
	Node* n_;
};
std::ostream& operator<<( std::ostream& os, const dsp& x )
{
	x.n_->display( x.indent_, os );
	return os;
}

struct hdr {
public:
	hdr( Header& h ) : h_(h) {}
	Header h_;
};
std::ostream& operator<<( std::ostream& os, const hdr& x )
{
	os << " ## " << Type::getDisplay( x.h_.t );
	return os;
}

template < class T >
void display_list( int indent, std::ostream& os, const T& v )
{
	for( typename T::const_iterator i = v.begin() ; i != v.end() ; ++i ) {
		os << std::endl;
		os << idt(indent) << dsp( indent, (*i) );
	}
}

template < class T >
void display_list_h( std::ostream& os, const T& v, const char* sep = ", " )
{
	bool first = true;
	for( typename T::const_iterator i = v.begin() ; i != v.end() ; ++i ) {
		if( first ) { first = false; } else { os << sep; }
		(*i)->display( 0, os );
	}
}


////////////////////////////////////////////////////////////////
// Module
void Module::display( int indent, std::ostream& os )
{
	os << idt(indent) << "<Module>" << hdr( h );
	topelems->display( indent+1, os );
	os << std::endl;
}

////////////////////////////////////////////////////////////////
// TopElems
void TopElems::display( int indent, std::ostream& os )
{
	display_list( indent, os, v );
}

////////////////////////////////////////////////////////////////
// TopElem
void TopElem::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Require
void Require::display( int indent, std::ostream& os )
{
	os << "<Require " << name->source->s << ">" << hdr( h );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDecl
void TopLevelFunDecl::display( int indent, std::ostream& os )
{
	fundecl->display( indent, os );
}

////////////////////////////////////////////////////////////////
// TopLevelFunDef
void TopLevelFunDef::display( int indent, std::ostream& os )
{
	fundef->display( indent, os );
}

////////////////////////////////////////////////////////////////
// TopLevelStructDef
void TopLevelStructDef::display( int indent, std::ostream& os )
{
	structdef->display( indent, os );
}

////////////////////////////////////////////////////////////////
// Block
void Block::display( int indent, std::ostream& os )
{
	os << idt(indent) << "<Block>" << hdr( h );
	statements->display( indent+1, os );
}

////////////////////////////////////////////////////////////////
// Statements
void Statements::display( int indent, std::ostream& os )
{
	display_list( indent, os, v );
}

////////////////////////////////////////////////////////////////
// Statement
void Statement::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FunDecl
void FunDecl::display( int indent, std::ostream& os )
{
	os << idt(indent) << "<FunDecl>" << hdr( h );
	sig->display( indent+1, os );
}

////////////////////////////////////////////////////////////////
// FunDef
void FunDef::display( int indent, std::ostream& os )
{
	os << "<FunDef>" << hdr( h ) << std::endl;
	os << idt(indent+1) << dsp( indent+1, sig ) << std::endl;
	body->display( indent+1, os );
}

////////////////////////////////////////////////////////////////
// FunSig
void FunSig::display( int indent, std::ostream& os )
{
	os << "<FunSig " << name->source->s;
	if( name->fresh ) { os << "~" << name->fresh->s; }
	os << " = ( " << dsp( 0, fargs ) << " ): "
	   << dsp( 0, result_type ) << ">" << hdr( h );
}

////////////////////////////////////////////////////////////////
// StructDef
void StructDef::display( int indent, std::ostream& os )
{
	os << idt(indent) << "<StructDef "
	   << name->source->s << ">";
	members->display( indent+1, os );
}

////////////////////////////////////////////////////////////////
// Slots
void Members::display( int indent, std::ostream& os )
{
	display_list( indent, os, v );
}

////////////////////////////////////////////////////////////////
// Member
void Member::display( int indent, std::ostream& os )
{
	os << idt(indent) << dsp( 0, farg ) << hdr( h );
}

////////////////////////////////////////////////////////////////
// FormalArgs
void FormalArgs::display( int indent, std::ostream& os )
{
	display_list_h( os, v );
}

////////////////////////////////////////////////////////////////
// FormalArg
void FormalArg::display( int, std::ostream& os )
{
	os << name->source->s << ": " << dsp( 0, t );
}

////////////////////////////////////////////////////////////////
// VarDecl
void VarDecl::display( int indent, std::ostream& os )
{
	os << "<Var " << dsp( 0, varelems )
	   << " = " << dsp( 0, data ) << ">" << hdr( h );
}

////////////////////////////////////////////////////////////////
// VarDeclElem
void VarDeclElem::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// VarDeclElems
void VarDeclElems::display( int indent, std::ostream& os )
{
	os << "( ";
	display_list_h( os, v );
	os << " )";
}

////////////////////////////////////////////////////////////////
// VarDeclIdentifier
void VarDeclIdentifier::display( int indent, std::ostream& os )
{
	os << name->source->s;
	if( name->fresh ) { os << "~" << name->fresh->s; }
	if( t ) { os << ": " << dsp( 0, t ); }
}

////////////////////////////////////////////////////////////////
// IfThenElse
void IfThenElse::display( int indent, std::ostream& os )
{
	os << "<IfThenElse " << dsp( 0, cond ) << ">" << hdr( h ) << std::endl;
	iftrue->display( indent+1, os ); os << std::endl;
	iffalse->display( indent+1, os );
}

////////////////////////////////////////////////////////////////
// TypeExpr
void TypeExpr::display( int, std::ostream& os )
{
	assert(0);
}

////////////////////////////////////////////////////////////////
// NamedType
void NamedType::display( int, std::ostream& os )
{
	os << "^" << name->source;
}

////////////////////////////////////////////////////////////////
// Types
void Types::display( int, std::ostream& os )
{
	display_list_h( os, v );
}

////////////////////////////////////////////////////////////////
// TypeRef
void TypeRef::display( int, std::ostream& os )
{
	os << Type::getDisplay( t );
}

////////////////////////////////////////////////////////////////
// MultiExpr
void MultiExpr::display( int indent, std::ostream& os )
{
	display_list_h( os, v ); 
	os << hdr( h );
}

////////////////////////////////////////////////////////////////
// Expr
void Expr::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOr::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalOr
void LogicalOrElems::display( int indent, std::ostream& os )
{
	display_list_h( os, v, " || " );
}

////////////////////////////////////////////////////////////////
// LogicalAnd
void LogicalAnd::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LogicalAndElems
void LogicalAndElems::display( int indent, std::ostream& os )
{
	display_list_h( os, v, " && " );
}

////////////////////////////////////////////////////////////////
// Equality
void Equality::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// EqualityEq
void EqualityEq::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " == ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// EqualityNe
void EqualityNe::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " != ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// Relational
void Relational::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// RelationalLt
void RelationalLt::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " < ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// RelationalGt
void RelationalGt::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " > ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// RelationalLe
void RelationalLe::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " <= ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// RelationalGe
void RelationalGe::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " >= ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// Additive
void Additive::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// AddExpr
void AddExpr::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " + ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// SubExpr
void SubExpr::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " - ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// Multiplicative
void Multiplicative::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MulExpr
void MulExpr::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " * ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// DivExpr
void DivExpr::display( int indent, std::ostream& os )
{
	lhs->display( 0, os );
	os << " / ";
	rhs->display( 0, os );
}

////////////////////////////////////////////////////////////////
// PrimExpr
void PrimExpr::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralBoolean
void LiteralBoolean::display( int indent, std::ostream& os )
{
	os << ( data ? "true" : "false" );
}

////////////////////////////////////////////////////////////////
// LiteralInteger
void LiteralInteger::display( int indent, std::ostream& os )
{
	os << data;
}

////////////////////////////////////////////////////////////////
// LiteralChar
void LiteralChar::display( int indent, std::ostream& os )
{
	os << "'" << char(data) << "'";
}

////////////////////////////////////////////////////////////////
// VarRef
void VarRef::display( int indent, std::ostream& os )
{
	os << name->source->s;
	if( name->fresh ) { os << "~" << name->fresh->s; }
}

////////////////////////////////////////////////////////////////
// Parenthized
void Parenthized::display( int indent, std::ostream& os )
{
	os << "<Parenthized " << dsp( 0, exprs ) << " >" << hdr( h );
}

////////////////////////////////////////////////////////////////
// CastExpr
void CastExpr::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// Cast
void Cast::display( int indent, std::ostream& os )
{
}

////////////////////////////////////////////////////////////////
// MemberExpr
void MemberExpr::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// MemberRef
void MemberRef::display( int indent, std::ostream& os )
{
}

////////////////////////////////////////////////////////////////
// FunCall
void FunCall::display( int indent, std::ostream& os )
{
	os << func->source->s << "( " << dsp( 0, aargs ) << " )";
}

////////////////////////////////////////////////////////////////
// Invoke
void Invoke::display( int indent, std::ostream& os )
{
	assert(0);
}

////////////////////////////////////////////////////////////////
// Lambda
void Lambda::display( int indent, std::ostream& os )
{
}

////////////////////////////////////////////////////////////////
// LiteralStruct
void LiteralStruct::display( int indent, std::ostream& os )
{
}

////////////////////////////////////////////////////////////////
// LiteralMembers
void LiteralMembers::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// LiteralMember
void LiteralMember::display( int indent, std::ostream& os )
{
}

////////////////////////////////////////////////////////////////
// ActualArgs
void ActualArgs::display( int indent, std::ostream& os )
{
	display_list_h( os, v );
}

////////////////////////////////////////////////////////////////
// ActualArg
void ActualArg::display( int indent, std::ostream& os )
{
	expr->display( indent, os );
}

////////////////////////////////////////////////////////////////
// SectionLabel
void SectionLabel::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// CatchLabel
void CatchLabel::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// FinallyLabel
void FinallyLabel::display( int indent, std::ostream& os )
{
    assert(0);
}

////////////////////////////////////////////////////////////////
// ThrowStatement
void ThrowStatement::display( int indent, std::ostream& os )
{
}

////////////////////////////////////////////////////////////////
// Identifier
void Identifier::display( int indent, std::ostream& os )
{
    assert(0);
}

} // namespace leaf
