import Hello0Parser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}
}

class Hello0 {
    public static function main(): Void {
        var parser = new Hello0Parser.Parser(new SemanticAction());

        parser.post(Token.Hello, 1);
        parser.post(Token.World, 2);
        parser.post(Token.Eof, 3);

        return;
    }
}

