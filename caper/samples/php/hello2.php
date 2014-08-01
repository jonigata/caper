<?php

error_reporting(E_ALL);

require_once('hello2_parser.php');

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
    
    function Greet($x, $y)
    {
        header('Content-Type: text/html');
        printf("%s%s\n", $x, $y);
        return "";
    }
}

$parser = new hello_world\Parser(new SemanticAction);

$parser->post(hello_world\Token::token_Hello, "Guten Tag, ");
$parser->post(hello_world\Token::token_World, "Welt");
$parser->post(hello_world\Token::token_eof, "");
