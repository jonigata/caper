import std.cstream;
import std.ctype;
import std.string;
import calc0_parser;

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

    calc0_parser.Token get(out int v)
    {
        int c;
        do {
            c = fgetc(file_);
            if(c == '\n') return calc0_parser.Token.token_eof;
        } while (std.ascii.isWhite(c));

        // 記号類
        switch( c ) {
        case '+': return calc0_parser.Token.token_Add;
        case '-': return calc0_parser.Token.token_Sub;
        case '*': return calc0_parser.Token.token_Mul;
        case '/': return calc0_parser.Token.token_Div;
        case EOF: return calc0_parser.Token.token_eof;
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
                return calc0_parser.Token.token_Number;
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
    int Identity(int n) { return n; }

    int MakeAdd(int x, int y)
    {
        fprintf(stderr, "%d + %d\n", x, y);
        return x + y ; 
    }

    int MakeSub(int x, int y)
    {
        fprintf(stderr, "%d - %d\n", x, y);
        return x - y ; 
    }

    int MakeMul(int x, int y)
    { 
        fprintf(stderr, "%d * %d\n", x, y);
        return x * y ; 
    }

    int MakeDiv(int x, int y)
    { 
        fprintf(stderr, "%d / %d\n", x, y);
        return x / y ; 
    }
}

int main(char [][] args)
{
    auto s = new Scanner(stdin);

    alias calc0_parser.Parser!(int, SemanticAction) Parser;
    auto parser = new Parser(new SemanticAction);

    printf(">");

    calc0_parser.Token token;
    int v;
    do{
        token = s.get(v);
    }while(!parser.post(token, v));

    if(parser.accept(v)){
        printf("accpeted %d\n", v);
    }

    return 0;
}