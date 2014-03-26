// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include <iostream>
#include "empty_hello.hpp"

struct SemanticAction {
        void syntax_error(){}
        void stack_overflow(){}
        void upcast( int& x, int y ) { x = y; }

        int DoEmpty0(){ return 0; }
        int DoEmpty1(){ return 0; }
        int DoS(){ return 0; }
};

int main( int, char** )
{
        SemanticAction sa;
        empty_hello::Parser< int, SemanticAction > parser( sa );

        parser.post( empty_hello::token_Hello, 0 );

        return 0;
}
