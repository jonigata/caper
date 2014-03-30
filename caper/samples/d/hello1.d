import std.stdio;
import hello1_parser;

class SemanticAction {
    void stack_overflow() {}
    void syntax_error() {}
    void upcast(out int x, int y) { x = y; }
    void downcast(out int x, int y) { x = y; }

    int Greet() {
        writeln("hello world");
        return 42;
    }
}

int main(char [][] args)
{
    alias hello1_parser.Parser!(int, SemanticAction) Parser;
    auto parser = new Parser(new SemanticAction);

    parser.post(hello1_parser.Token.token_Hello, 1);
    parser.post(hello1_parser.Token.token_World, 2);
    parser.post(hello1_parser.Token.token_eof, 3);

    return 0;
}
