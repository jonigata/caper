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

namespace leaf {

////////////////////////////////////////////////////////////////
// token_representation
std::string
Compiler::token_representation( Token token, Node* v )
{
    switch( token ) {
    case token_eof:         return "<EOF>";
    case token_Add:         return "+";
    case token_Assign:      return "=";
    case token_Comma:           return ",";
    case token_Div:         return "/";
    case token_Else:            return "else";
    case token_Eq:          return "==";
    case token_Fun:         return "fun";
    case token_Ge:          return ">";
    case token_Gt:          return ">=";
    case token_Identifier:
        return static_cast<Identifier*>(v)->source->s + "<identifier>";
    case token_If:          return "if";
    case token_LBra:            return "{";
    case token_Le:          return "<";
    case token_LiteralBoolean:
        {
            if( static_cast<LiteralBoolean*>(v)->data ) {
                return "true";
            } else {
                return "false";
            }
        }
    case token_LiteralInteger:
        {
            char buffer[256];
            sprintf( buffer, "%d<integer>",
                     static_cast<LiteralInteger*>(v)->data );
            return buffer;
        }
    case token_LiteralChar:
        {
            char buffer[256];
            sprintf( buffer, "'%c'",
                     static_cast<LiteralChar*>(v)->data );
            return buffer;
        }
    case token_LPar:            return "(";
    case token_Lt:          return "<=";
    case token_Mul:         return "*";
    case token_Ne:          return "!=";
    case token_RBra:            return "}";
    case token_Req:         return "require";
    case token_RPar:            return ")";
    case token_Semicolon:       return ";";
    case token_Colon:       return ":";
    case token_Sub:         return "-";
    case token_TypeChar:        return "<char>";
    case token_TypeInt:     return "<int>";
    case token_TypeLong:        return "<long>";
    case token_TypeShort:       return "<short>";
    case token_TypeVoid:        return "<void>";
    case token_Var:         return "var";
	case token_Throw:		return "throw";
	case token_Catch:		return "catch";
    default:
		std::cerr << token << std::endl;
        return "<<<UNKNOWN TOKEN>>>";
    }
    return "";
}

void Compiler::compile_internal( leaf::Node* n, std::ostream& os )
{
    // module
    boost::scoped_ptr< llvm::Module > module( new llvm::Module( "test" ) );

    try {
        // AST‚©‚ç•ÏŠ·
		n->alpha( env_ );
        n->entype( env_ );
		n->display( 0, std::cerr );
        n->encode( env_, module.get() );
    }
    catch( error& e ){
        if( e.addr.file ) {
            e.filename = e.addr.file->s;
            e.lineno = env_.sm.lineno( e.addr );
            e.column = env_.sm.column( e.addr );
        }
        throw;
    }

    os << *module;
}

} // namespace leaf
