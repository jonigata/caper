// 2008/08/09 Naoyuki Hirayama

/*!
    @file     scanner.hpp
    @brief    <概要>

    <説明>
*/

#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include "scoped_allocator.hpp"
#include "leaf_node.hpp"
#include "leaf_ast.hpp"
#include "leaf_grammar.hpp"
#include "leaf_error.hpp"

namespace leaf {

template < class It >
class Scanner : public boost::noncopyable {
public:
    typedef int char_type;

public:
    Scanner( CompileEnv& env, Symbol* file, It b, It e )
        : env_(env), file_( file ), b_(b), e_(e), c_(b), unget_(EOF)
    {
        pos_ = 0;
        reserved_["require"] = token_Req;
        reserved_["var"] = token_Var;
        reserved_["fun"] = token_Fun;
        reserved_["struct"] = token_Struct;
        reserved_["if"] = token_If;
        reserved_["else"] = token_Else;
        reserved_["void"] = token_TypeVoid;
        reserved_["long"] = token_TypeLong;
        reserved_["int"] = token_TypeInt;
        reserved_["short"] = token_TypeShort;
        reserved_["char"] = token_TypeChar;
        reserved_["throw"] = token_Throw;
        reserved_["catch"] = token_Catch;

        lines_ = &env.sm.get_lines( file ).v;
        lines_->clear();
        lines_->push_back( 0 );
    }

    Addr addr( int pos ) { return Addr( file_, pos ); }
    Addr addr() { return addr( pos_ ); }

    Token get( Node*& v )
    {
        int c;
        v = NULL;

        // 空白スキップ
        do {
            c = getc();
        } while( isspace( c ) );

        // 位置
        int b = pos_ - 1;
        int e = pos_;
        
        // 記号類
        switch( c ) {
        case '@': return token_At;
        case '+': return token_Add;
        case '-': return token_Sub;
        case '*': return token_Mul;
        case '/': return token_Div;
        case '=':
            c = getc();
            if( c == '=' ) {
                return token_Eq;
            } else {
                ungetc( c );
                return token_Assign;
            }

        case '<':
            c = getc();
            if( c == '=' ) {
                return token_Le;
            } else {
                ungetc( c );
                return token_Lt;
            }
        case '>':
            c = getc();
            if( c == '=' ) {
                return token_Ge;
            } else {
                ungetc( c );
                return token_Gt;
            }
        case '!':
            c = getc();
            if( c == '=' ) {
                return token_Ne;
            } else {
                ungetc( c );
                return token_Not;
            }
        case '&':
            c = getc();
            if( c == '&' ) {
                return token_And;
            } else {
                ungetc( c );
                return token_BitAnd;
            }
        case '|':
            c = getc();
            if( c == '|' ) {
                return token_Or;
            } else {
                ungetc( c );
                return token_BitOr;
            }

        case '(': return token_LPar;
        case ')': return token_RPar;
        case '{': return token_LBra;
        case '}': return token_RBra;
        case ',': return token_Comma;
        case ';': return token_Semicolon;
        case ':': return token_Colon;
        case '\'':
                c = getc();
                if( c == '\'' ) {
                    // ''
                    throw unexpected_char( addr(b), c );
                } else if( c == '\\' ) {
                    // '\0'
                    c = getc();
                    switch( c ) {
                    case '0': c = '\0'; break;
                    case 'n': c = '\n'; break;
                    case 't': c = '\t'; break;
                    }
                    int k = getc();
                    if( k != '\'' ) {
                        throw unexpected_char( addr(b), k );
                    }
                    v = h( b, e, env_.cage.allocate<LiteralChar>( c ) );
                    return token_LiteralChar;
                } else {
                    // 'a'
                    int k = getc();
                    if( k != '\'' ) {
                        throw unexpected_char( addr(b), k );
                    }
                    v = h( b, e, env_.cage.allocate<LiteralChar>( c ) );
                    return token_LiteralChar;
                }
		case '.':
			return token_Dot;
        case EOF:
            return token_eof;
        }

        // 識別子
        if( isalpha( c ) || c == '_' ) {
            std::stringstream ss;
            while( c != EOF && ( isalpha( c ) || isdigit( c ) || c == '_' ) ) {
                ss << (char)c;
                c = getc();
            }
            ungetc( c );

            std::string s = ss.str();
            if( s == "true" ) {
                v = h( b, e,
                       env_.cage.allocate<LiteralBoolean>( true ) );
                return token_LiteralBoolean;
            } else if( s == "false" ) {
                v = h( b, e,
                       env_.cage.allocate<LiteralBoolean>( false ) );
                return token_LiteralBoolean;
            }

            reserved_type::const_iterator i = reserved_.find( s );
            if( i != reserved_.end() ) {
                v = NULL;
                return (*i).second;
            } else {
                v = h( b, e,
                       env_.cage.allocate<Identifier>( env_.intern( s ) ) );
                return token_Identifier;
            }
        }

        // 整数
        if( isdigit( c ) ) {
            boost::int64_t n = 0;
            while( c != EOF && isdigit( c ) ) {
                n *= 10;
                n += c - '0';
                c = getc();
            }
            ungetc( c );

            v = h( b, e, env_.cage.allocate<LiteralInteger>( n ) );

            return token_LiteralInteger;
        }


        std::cerr << char(c) << std::endl;
        throw unexpected_char( addr(b), c );
    }

private:
    char_type getc()
    {
        int c;
        if( unget_ != EOF ) {
            c = unget_;
            unget_ = EOF;
        } else if( c_ == e_ ) {
            c = EOF; 
        } else {
            c = *c_++;
            if( c == '\n' ) {
                lines_->push_back( pos_+1 );
            }
        }
        pos_++;
        return c;
    }

    void ungetc( char_type c )
    {
        if( c != EOF ) {
            pos_--;
            unget_ = c;
        }
    }

    template < class T >
    T* h( int b, int e, T* p )
    {
        p->h.id = env_.idseed++;
        p->h.beg = Addr( file_, b );
        p->h.end = Addr( file_, e );
        return p;
    }

private:
    CompileEnv&         env_;
    Symbol*             file_;
    std::vector<int>*   lines_;
    It                  b_;
    It                  e_;
    It                  c_;
    char_type           unget_;
    int                 pos_;

    typedef std::map< std::string, Token > reserved_type;
    reserved_type reserved_;

};

} // namespace leaf

#endif // SCANNER_HPP_
