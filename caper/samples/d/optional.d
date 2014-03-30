import std.cstream;
import std.ctype;
import std.string;
import optional_parser;

class unexpected_char : Exception {
    this(string message, string file = __FILE__, int line = __LINE__){
        super(message, file, line);
    }
}

class Scanner {
    alias int char_type;

    this(FILE *file)
    {
        file_ = file;
    }

    optional_parser.Token get(out int v)
    {
        int c;
        do {
            c = fgetc(file_);
            if(c == '\n') return optional_parser.Token.token_eof;
        } while (std.ascii.isWhite(c));

        // 記号類
        switch( c ) {
        case '(': return optional_parser.Token.token_LParen;
        case ')': return optional_parser.Token.token_RParen;
        case ',': return optional_parser.Token.token_Comma;
        case EOF: return optional_parser.Token.token_eof;
        default:
            // 整数
            if (std.ascii.isDigit(c)) {
                int n = 0;
                while (c != EOF && std.ascii.isDigit(c)) {
                    n *= 10;
                    n += c - '0';
                    c = fgetc(file_);
                }
                ungetc(c, file_);
                v = n;
                return optional_parser.Token.token_Number;
            }
        }

        char cc = cast(char)c;
        throw new unexpected_char(std.string.format("bad input char '%s'(%d)\n", (&cc)[0..1], c));
    }

private:
    FILE *file_;
}

class SemanticAction {
    void syntax_error(){}
    void stack_overflow(){}
    void downcast(out int x, int y ) { x = y; }
    void upcast(out int x, int y ) { x = y; }

    int Document(int[] a) {
        if (0 < a.length) {
            return a[0];
        } else {
            return 42;
        }
    }

}

int main(char [][] args)
{
    auto s = new Scanner(stdin);

    alias optional_parser.Parser!(int, SemanticAction) Parser;
    auto parser = new Parser(new SemanticAction);

    printf(">");

    optional_parser.Token token;
    int v;
    do{
        token = s.get(v);
    }while(!parser.post(token, v));

    if(parser.accept(v)){
        printf("accpeted %d\n", v);
    }

    return 0;
}