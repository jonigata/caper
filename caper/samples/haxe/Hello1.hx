import Hello1Parser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError     () {}
    public function greet(): Int { trace('Guten Tag'); return 0; }
}

class Hello1 {
    public static function main(): Void {
        var parser = new Hello1Parser.Parser(new SemanticAction());

        parser.post(Token.Hello, 1);
        parser.post(Token.World, 2);
        parser.post(Token.Eof, 3);

        return;
    }
}

