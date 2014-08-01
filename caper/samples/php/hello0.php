<?php

error_reporting(E_ALL);

require_once('hello0_parser.php');

class SemanticAction
{
    function syntax_error()
    {
    }

    function stack_overflow()
    {
    }
    
    function downcast($y)
    {
        return $y;
    }

    function upcast($y)
    {
        return $y;
    }
}

$parser = new hello_world\Parser(new SemanticAction());

$parser->post(hello_world\Token::token_Hello, 0);
$parser->post(hello_world\Token::token_World, 0);
