// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "calc1.hpp"
#include <iostream>

class unexpected_char : public std::exception {};

template < class It >
class scanner {
public:
    typedef int char_type;

public:
    scanner( It b, It e ) : b_(b), e_(e), c_(b), unget_(EOF) { }

    calc::Token get( Node*& v )
    {
        v = NULL;
        int c;
        do {
            c = getc();
        } while( isspace( c ) );

        // 記号類
        switch( c ) {
        case '+': return calc::token_Add;
        case '-': return calc::token_Sub;
        case '*': return calc::token_Mul;
        case '/': return calc::token_Div;
        case EOF: return calc::token_eof;
        }

        // 整数
        if( isdigit( c ) ) {
            int n = 0;
            while( c != EOF && isdigit( c ) ) {
                n *= 10;
                n += c - '0';
                c = getc();
            }
            ungetc( c );
            v = new Number( n );
            return calc::token_Number;
        }


        std::cerr << char(c) << std::endl;
        throw unexpected_char();
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
        }
        return c;
    }

    void ungetc( char_type c )
    {
        if( c != EOF ) {
            unget_ = c;
        }
    }

private:
    It              b_;
    It              e_;
    It              c_;
    char_type       unget_;

};

struct SemanticAction {
    void syntax_error(){}
    void stack_overflow(){}

    template < class T >
    void downcast( T*& x, Node* y ) { x = static_cast<T*>( y ); }

    template < class T >
    void upcast( Node*& x, T* y ) { x = y; }

    Expr* MakeExpr( Term* x ) { return new TermExpr( x ); }
    Expr* MakeAdd( Expr* x, Term* y )
    {
        std::cerr << "expr " << x << " + " << y << std::endl;
        return new AddExpr( x, new TermExpr( y ) ) ;
    }
    Expr* MakeSub( Expr* x, Term* y )
    {
        std::cerr << "expr " << x << " - " << y << std::endl;
        return new SubExpr( x, new TermExpr( y ) ) ;
    }
    Term* MakeTerm( Number* x ) { return new NumberTerm( x ); }
    Term* MakeMul( Term* x, Number* y )
    {
        std::cerr << "expr " << x << " * " << y << std::endl;
        return new MulTerm( x, new NumberTerm( y ) ) ;
    }
    Term* MakeDiv( Term* x, Number* y )
    {
        std::cerr << "expr " << x << " / " << y << std::endl;
        return new DivTerm( x, new NumberTerm( y ) ) ;
    }
};

int main( int, char** )
{
    // スキャナ
    typedef std::istreambuf_iterator<char> is_iterator;
    is_iterator b( std::cin );   // 即値にするとVC++が頓珍漢なことを言う
    is_iterator e;
    scanner< is_iterator > s( b, e );

    SemanticAction sa;

    calc::Parser< Node*, SemanticAction > parser( sa );

    calc::Token token;
    for(;;) {
        Node* v;
        token = s.get( v );
        if( parser.post( token, v ) ) { break; }
    }

    Node* v;
    if( parser.accept( v ) ) {
        std::cerr << "accpeted\n";
        std::cerr << v->calc() << std::endl;
    }

    return 0;
}
