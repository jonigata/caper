import Calc0Parser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}
    public function identity(a: Int) {
        trace('identity');
        return a;
    }
    public function makeAdd(a: Int, b: Int) {
        trace('makeAdd');
        return a + b;
    }
    public function makeSub(a: Int, b: Int) {
        trace('makeSub');
        return a - b;
    }
    public function makeMul(a: Int, b: Int) {
        trace('makeMul');
        return a * b;
    }
    public function makeDiv(a: Int, b: Int): Int {
        trace('makeDiv');
        return Math.floor(a / b);
    }
}

class Calc0 {
    static function scanner(s: String) {
        var r = ~/^\s*/;
        r.match(s);
        s = r.matchedRight();
        trace(s);
    
        if (s.length == 0) {
            return { token: Token.Eof, value: null, rest: '' };
        }
    
        switch(s.charAt(0)) {
            case '+': return { token: Add, value: null, rest: s.substr(1) };
            case '-': return { token: Sub, value: null, rest: s.substr(1) };
            case '*': return { token: Mul, value: null, rest: s.substr(1) };
            case '/': return { token: Div, value: null, rest: s.substr(1) };
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
        var parser = new Calc0Parser.Parser(new SemanticAction());

        while(true) {
            var a = scanner(s);
            trace(a);
            if (parser.post(a.token, a.value)) {
                break;
            }
            s = a.rest;
        }
        trace(parser.accept());
    }

    public static function main(): Void {
        parse('3+7*8+2');
    }
}

