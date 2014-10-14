import OptionalParser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}

    public function document(x: Null<Int>) {
        if (x != null) {
            return x;
        } else {
            return 42;
        }
    }
}

class Optional {
    static function scanner(s: String) {
        var r = ~/^\s*/;
        r.match(s);
        s = r.matchedRight();
        //trace(s);
    
        if (s.length == 0) {
            return { token: Token.Eof, value: null, rest: '' };
        }
    
        switch(s.charAt(0)) {
            case '(': return { token: LParen, value: null, rest: s.substr(1) };
            case ')': return { token: RParen, value: null, rest: s.substr(1) };
            case ',': return { token: Comma, value: null, rest: s.substr(1) };
            case '*': return { token: Star, value: null, rest: s.substr(1) };
        }
        r = ~/^\d+/;
        if (r.match(s)) {
            s = r.matchedRight();
            return {
                token: Token.Number,
                value: Std.parseInt(r.matched(0)),
                rest: s
            };
        }
        throw "unknown char";
    }

    static function parse(s) {
        var parser = new OptionalParser.Parser(new SemanticAction());

        while(true) {
            var a = scanner(s);
            //trace(a);
            if (parser.post(a.token, a.value)) {
                break;
            }
            s = a.rest;
        }
        trace(parser.accept());
    }

    public static function main(): Void {
        parse('()');
    }
}

