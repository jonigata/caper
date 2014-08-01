<?php

error_reporting(E_ALL);

require_once('hello1_parser.php');

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
    
    function Greet()
    {
        header('Content-Type: text/html');
        echo("hello world\n");
    }
}

$parser = new hello_world\Parser(new SemanticAction);

$parser->post(hello_world\Token::token_Hello, 0);
$parser->post(hello_world\Token::token_World, 0);
$parser->post(hello_world\Token::token_eof, 0);
