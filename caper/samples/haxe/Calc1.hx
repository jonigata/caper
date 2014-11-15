import Calc1Parser;

enum Term {
    Mul(a: Term, b: Int);
    Div(a: Term, b: Int);
    Term_Int(a: Int);
}

enum Expr {
    Add(a: Expr, b: Term);
    Sub(a: Expr, b: Term);
    Expr_Term(a: Term);
}

class SemanticAction {
    public function new() {}
    public function stackOverflow() {}
    public function syntaxError() {}

    public function makeAdd(a: Expr, b: Term): Expr {
        return Add(a, b);
    }
    public function makeSub(a: Expr, b: Term): Expr {
        return Sub(a, b);
    }
    public function makeExpr(a: Term): Expr {
        return Expr_Term(a);
    }
    public function makeMul(a: Term, b: Int): Term {
        return Mul(a, b);
    }
    public function makeDiv(a: Term, b: Int): Term {
        return Div(a, b);
    }
    public function makeTerm(a: Int): Term {
        return Term_Int(a);
    }
}

class Calc1 {
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
        var parser = new Calc1Parser.Parser(new SemanticAction());

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

