import Hello2Parser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}
    public function greet(arg0: String, arg1: String): String {
        trace('${arg0} ${arg1}');
        return '${arg0} ${arg1}';
    }
}

class Hello2 {
    public static function main(): Void {
        var parser = new Hello2Parser.Parser(new SemanticAction());

        parser.post(Token.Hello, 'Guten Tag');
        parser.post(Token.World, 'Welt');
        parser.post(Token.Eof, 3);

        return;
    }
}

