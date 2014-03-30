import std.stdio;
import hello0_parser;

class SemanticAction {
    void stack_overflow() {}
    void syntax_error() {}
}

int main(char [][] args)
{
    alias hello0_parser.Parser!(int, SemanticAction) Parser;
    auto parser = new Parser(new SemanticAction);

    parser.post(hello0_parser.Token.token_Hello, 1);
    parser.post(hello0_parser.Token.token_World, 2);
    parser.post(hello0_parser.Token.token_eof, 3);

    return 0;
}
