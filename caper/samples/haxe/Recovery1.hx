import Recovery1Parser;

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}

    public function doLine1() { return 0; }
    public function doLine2(exp) {
        trace("Exp");
        return exp;
    }
    public function doLine3() {
        trace("catched");
        return -1;
    }

    public function doAddExp1(exp1) { return exp1; }
    public function doAddExp2(exp1, exp2) { return exp1 + exp2; }
    public function doAddExp3(exp1, exp2) { return exp1 - exp2; }

    public function doMulExp1(exp1) { return exp1; }
    public function doMulExp2(exp1, exp2) { return exp1 * exp2; }
    public function doMulExp3(exp1, exp2) { return Math.floor(exp1 / exp2); }

    public function doUnaryExp1(exp1) { return exp1; }
    public function doUnaryExp2(exp1) { return -exp1; }

    public function doPrimExp1(num1) { return num1; }
    public function doPrimExp2(exp1) { return exp1; }
}

class Recovery1 {
    static function scanner(s: String) {
        var r = ~/^\s*/;
        r.match(s);
        s = r.matchedRight();
        //trace(s);
    
        if (s.length == 0) {
            return { token: Token.Eof, value: null, rest: '' };
        }
    
        switch(s.charAt(0)) {
            case '-': return { token: Minus, value: null, rest: s.substr(1) };
            case '+': return { token: Plus, value: null, rest: s.substr(1) };
            case '*': return { token: Star, value: null, rest: s.substr(1) };
            case '/': return { token: Slash, value: null, rest: s.substr(1) };
            case '(': return { token: LParen, value: null, rest: s.substr(1) };
            case ')': return { token: RParen, value: null, rest: s.substr(1) };
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
        var parser = new Recovery1Parser.Parser(new SemanticAction());

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
        parse('(1+2*3+/-8)');
    }
}

