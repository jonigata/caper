// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include <iostream>
#include "hello1.ipp"

struct SemanticAction {
    void syntax_error(){}
    void stack_overflow(){}
    void upcast( int& x, int y ) { x = y; }

    int Greet() { std::cout << "hello world" << std::endl; return 0; }
};

int main( int, char** )
{
    SemanticAction sa;
    hello_world::Parser< int, SemanticAction > parser( sa );

    parser.post( hello_world::token_Hello, 0 );
    parser.post( hello_world::token_World, 0 );
    parser.post( hello_world::token_eof, 0 );

    return 0;
}
