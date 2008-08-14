// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include <string>
#include <iostream>
#include "hello2.hpp"

struct SemanticAction {
        void syntax_error(){}
        void stack_overflow(){}
        void upcast( std::string& x, const std::string& y ) { x = y; }
        void downcast( std::string& x, const std::string& y ) { x = y; }

        std::string Greet( const std::string& x, const std::string& y )
        {
                std::cout << x << y << std::endl; return "";
        }
};

int main( int, char** )
{
        SemanticAction sa;
        hello_world::Parser< std::string, SemanticAction > parser( sa );

        parser.post( hello_world::token_Hello, "Guten Tag, " );
        parser.post( hello_world::token_World, "Welt" );
        parser.post( hello_world::token_eof, "" );

        return 0;
}
