import std.stdio;
import hello2_parser;

class SemanticAction {
    void stack_overflow() {}
    void syntax_error() {}
    void upcast(out string x, string y) { x = y; }
    void downcast(out string x, string y) { x = y; }

    string Greet(string x, string y) {
        writefln("%s %s", x, y);
        return "greet";
    }
}

int main(char [][] args)
{
    alias hello2_parser.Parser!(string, SemanticAction) Parser;
    auto parser = new Parser(new SemanticAction);

    parser.post(hello2_parser.Token.token_Hello, "Guten Tag");
    parser.post(hello2_parser.Token.token_World, "Welt");
    parser.post(hello2_parser.Token.token_eof, "");

    return 0;
}
