import std.cstream;
import std.ctype;
import std.string;
import list1_parser;

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

    list1_parser.Token get(out int v)
    {
        int c;
        do {
            c = fgetc(file_);
            if(c == '\n') return list1_parser.Token.token_eof;
        } while (std.ascii.isWhite(c));

        // 記号類
        switch( c ) {
        case '(': return list1_parser.Token.token_LParen;
        case ')': return list1_parser.Token.token_RParen;
        case ',': return list1_parser.Token.token_Comma;
        case EOF: return list1_parser.Token.token_eof;
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
                return list1_parser.Token.token_Number;
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
            int total = 0;
            foreach(int x; a) {
                std.stdio.writefln("%d, ", x);
                total += x;
            }
            return total;
        } else {
            return 42;
        }
    }

}

int main(char [][] args)
{
    auto s = new Scanner(stdin);

    alias list1_parser.Parser!(int, SemanticAction) Parser;
    auto parser = new Parser(new SemanticAction);

    printf(">");

    list1_parser.Token token;
    int v;
    do{
        token = s.get(v);
    } while(!parser.post(token, v));

    if (parser.error()) {
       throw new Error("syntax error");
    }

    if (parser.accept(v)){
        printf("accpeted %d\n", v);
    }

    return 0;
}