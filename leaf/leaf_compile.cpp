// 2008/08/13 Naoyuki Hirayama

#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Assembly/Writer.h>
#include <boost/scoped_ptr.hpp>
#include <iostream>

#include "leaf_compile.hpp"
#include "leaf_parser.hpp"

////////////////////////////////////////////////////////////////
// token_representation
std::string
token_representation( leaf::Token token, leaf::Node* v )
{
    switch( token ) {
    case leaf::token_eof:           return "<EOF>";
    case leaf::token_Add:           return "+";
    case leaf::token_Assign:        return "=";
    case leaf::token_Comma:         return ",";
    case leaf::token_Div:           return "/";
    case leaf::token_Else:          return "else";
    case leaf::token_Eq:            return "==";
    case leaf::token_Fun:           return "fun";
    case leaf::token_Ge:            return ">";
    case leaf::token_Gt:            return ">=";
    case leaf::token_Identifier:
        return static_cast<leaf::Identifier*>(v)->s->s + "<identifier>";
    case leaf::token_If:            return "if";
    case leaf::token_LBra:          return "{";
    case leaf::token_Le:            return "<";
    case leaf::token_LiteralBoolean:
        {
            if( static_cast<leaf::LiteralBoolean*>(v)->value ) {
                return "true";
            } else {
                return "false";
            }
        }
    case leaf::token_LiteralInteger:
        {
            char buffer[256];
            sprintf( buffer, "%d<integer>",
                     static_cast<leaf::LiteralInteger*>(v)->value );
            return buffer;
        }
    case leaf::token_LiteralChar:
        {
            char buffer[256];
            sprintf( buffer, "'%c'",
                     static_cast<leaf::LiteralChar*>(v)->value );
            return buffer;
        }
    case leaf::token_LPar:          return "(";
    case leaf::token_Lt:            return "<=";
    case leaf::token_Mul:           return "*";
    case leaf::token_Ne:            return "!=";
    case leaf::token_RBra:          return "}";
    case leaf::token_Req:           return "require";
    case leaf::token_RPar:          return ")";
    case leaf::token_Semicolon:     return ";";
    case leaf::token_Sub:           return "-";
    case leaf::token_Var:           return "var";
    default:
        return "<<<UNKNOWN TOKEN>>>";
    }
    return "";
}

////////////////////////////////////////////////////////////////
// read_from_file
leaf::Node* read_from_file(
    const std::string&  infile,
    SemanticAction&     sa, 
    scanner_type&       s )
{
    // パーサ
    leaf::Parser< leaf::Node*, SemanticAction > parser( sa );

    // リードループ
    try {
        leaf::Token token = leaf::token_eof;
        leaf::Node* v = NULL;
        for(;;) {
            token = s.get( v );
            if( parser.post( token, v ) ) { break; }
        }

        if( parser.error() ) {
            throw leaf::syntax_error(
                s.addr(), token_representation( token, v ) );
        }
        if( parser.accept( v ) ) {
            v->entype();
            return v;
        }
    }
    catch( leaf::error& e ) {
        if( e.addr < 0 ) {
            e.addr = s.addr();
        }
        if( !e.caught() ) {
            e.set_info( infile, s.lineno( e.addr ), s.column( e.addr ) );
        }
        throw;
    }

    assert(0);
    return NULL;
}

////////////////////////////////////////////////////////////////
// compile
void compile(
    const std::string&  infile,
    scanner_type&       s,
    leaf::Node*         n,
    std::ostream&       os )
{
    // module
    boost::scoped_ptr< llvm::Module > module( new llvm::Module( "test" ) );

    // ASTから変換
    try {
        n->encode( module.get() );
    }
    catch( leaf::error& e ) {
        e.set_info( infile, s.lineno( e.addr ), s.column( e.addr ) );
        throw;
    }

    os << *module;
}

