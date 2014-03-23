// Copyright (C) 2006 Naoyuki Hirayama.
// Copyright (C) 2014 Katayama Hirofumi MZ.
// All Rights Reserved.

// $Id$

#include "recovery2.ipp"
#include <iostream>
#include <cmath>

class unexpected_char : public std::exception {};

template <class It>
class scanner {
public:
    typedef int char_type;
    int eof() { return std::char_traits<char_type>::eof(); }

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_(eof()) {}

    calc::Token get(int& v) {
        int c;
        do {
            c = getch();
            if (c == '\n') return calc::token_NewLine;
        } while (isspace(c));

        // 記号類
        if (c == eof()) {
            return calc::token_eof;
        } else {
            v = c;
            switch (c) {
                case '-': return calc::token_Minus;
                case '+': return calc::token_Plus;
                case '*': return calc::token_Star;
                case '/': return calc::token_Slash;
                case '(': return calc::token_LParen;
                case ')': return calc::token_RParen;
            }
        }

        // 整数
        if (isdigit(c)) {
            int n = 0;
            while (c != eof() && isdigit(c)) {
                n *= 10;
                n += c - '0';
                c = getch();
            }
            ungetch(c);
            v = n;
            return calc::token_Number;
        }

        std::cerr << char(c) << std::endl;
        throw unexpected_char();
    }

private:
    char_type getch() {
        int c;
        if (unget_ != eof()) {
            c = unget_;
            unget_ = eof();
        } else if (c_ == e_) {
            c = eof(); 
        } else {
            c = *c_++;
        }
        return c;
    }

    void ungetch(char_type c) {
        if (c != eof()) {
            unget_ = c;
        }
    }

private:
    It              b_;
    It              e_;
    It              c_;
    char_type       unget_;

};

struct SemanticAction {
    void syntax_error() {}
    void stack_overflow() {}
    void downcast(int& x, int y) { x = y; }
    void upcast(int& x, int y) { x = y; }

    int DoLine1() { return 0; }
    int DoLine2(int exp) {
        std::cout << "Exp: " << exp << std::endl;
        return exp;
    }
    int DoLine3() {
        std::cout << "catched" << std::endl;
        return -1;
    }

    int DoAddExp1(int exp1) { return exp1; }
    int DoAddExp2(int exp1, int exp2) { return exp1 + exp2; }
    int DoAddExp3(int exp1, int exp2) { return exp1 - exp2; }

    int DoMulExp1(int exp1) { return exp1; }
    int DoMulExp2(int exp1, int exp2) { return exp1 * exp2; }
    int DoMulExp3(int exp1, int exp2) { return exp1 / exp2; }

    int DoUnaryExp1(int exp1) { return exp1; }
    int DoUnaryExp2(int exp1) { return -exp1; }

    int DoPrimExp1(int num1) { return num1; }
    int DoPrimExp2(int exp1) { return exp1; }
};

int main( int, char** )
{
    // スキャナ
    typedef std::istreambuf_iterator<char> is_iterator;
    is_iterator b( std::cin );   // 即値にするとVC++が頓珍漢なことを言う
    is_iterator e;
    scanner<is_iterator> s(b, e);

    SemanticAction sa;
    calc::Parser<int, SemanticAction> parser(sa);

    calc::Token token;
    for(;;) {
        int v;
        token = s.get( v );
        std::cout << calc::token_label(token) << std::endl;
        if (parser.post(token, v)) { break; }
    }

    if (parser.error()) {
        std::cerr << "error occured: " << calc::token_label(token) << std::endl;
        exit(1);
    }

    int v;
    if (parser.accept(v)) {
        std::cerr << "accepted\n";
        std::cerr << v << std::endl;
    }

    return 0;
}
