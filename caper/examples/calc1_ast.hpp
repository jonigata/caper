#ifndef CALC1_AST_HPP
#define CALC1_AST_HPP

struct Node {
        virtual ~Node(){}
        virtual int calc() = 0;
};

struct Expr : public Node {
};

struct Term : public Node {
};

struct Number : public Node {
        int number;
        Number( int n ) : number( n ) {}
        int calc() { return number; }
};

struct AddExpr : public Expr {
        Expr*   lhs;
        Expr*   rhs;
        AddExpr( Expr* x, Expr* y ) : lhs( x ), rhs( y ) {}
        int calc() { return lhs->calc() + rhs->calc(); }
};

struct SubExpr : public Expr {
        Expr*   lhs;
        Expr*   rhs;
        SubExpr( Expr* x, Expr* y ) : lhs( x ), rhs( y ) {}
        int calc() { return lhs->calc() - rhs->calc(); }
};

struct TermExpr : public Expr {
        Term*   term;
        TermExpr( Term* x )  : term( x ) {}
        int calc() { return term->calc(); }
};

struct MulTerm : public Term {
        Term*   lhs;
        Term*   rhs;
        MulTerm( Term* x, Term* y ) : lhs( x ), rhs( y ) {}
        int calc() { return lhs->calc() * rhs->calc(); }
};

struct DivTerm : public Term {
        Term*   lhs;
        Term*   rhs;
        DivTerm( Term* x, Term* y ) : lhs( x ), rhs( y ) {}
        int calc() { return lhs->calc() / rhs->calc(); }
};

struct NumberTerm : public Term {
        Number* number;
        NumberTerm( Number* x ) : number( x ) {}
        int calc() { return number->calc(); }
};

#endif // CALC1_AST_HPP
