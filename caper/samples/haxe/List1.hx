import List1Parser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}

    public function document(x: Array<Int>) {
        var n = 0;
        for (y in x) {
            trace(y);
            n += y;
        }
        return n;
    }
}

class List1 {
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
        var parser = new List1Parser.Parser(new SemanticAction());

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
        parse('(1 2 3 4 5 6 7 8 9 10)');
    }
}

