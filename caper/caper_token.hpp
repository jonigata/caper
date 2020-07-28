#ifndef CAPER_TOKEN_HPP_
#define CAPER_TOKEN_HPP_

#include <boost/variant.hpp>
#include <ostream>

////////////////////////////////////////////////////////////////
// Token
enum Token {
    token_empty,
    token_error,
    token_identifier,
    token_integer,
    token_typetag,
    token_colon,
    token_semicolon,
    token_pipe,
    token_lparen,
    token_rparen,
    token_lbracket,
    token_rbracket,
    token_equal,
    token_star,
    token_plus,
    token_question,
    token_slash,
    token_directive_token,
    token_directive_token_prefix,
    token_directive_external_token,
    token_directive_allow_ebnf,
    token_directive_namespace,
    token_directive_recover,
    token_directive_access_modifier,
    token_directive_dont_use_stl,
    token_directive_smart_pointer,
    token_eof,
};

struct TokenTraits {
    static Token eof() { return token_eof; }
};

inline
const char* token_labels(Token op) {
    const char* labels[] = {
        "<>",
        "error",
        "IDENT",
        "number",
        "<type>",
        ":",
        ";",
        "|",
        "(",
        ")",
        "[",
        "]",
        "=",
        "*",
        "+",
        "?",
        "/",
        ".",
        "%token",
        "%token_prefix",
        "%external_token",
        "%allow_ebnf",
        "%namespace",
        "%recover",
        "%access_modifier",
        "%dont_use_stl",
        "%smart_pointer",
        "$"
    };

    return labels[op];
}

inline
std::ostream& operator<<(std::ostream& os, Token op) {
    os <<  token_labels(op);
    return os;
}

#endif // CAPER_TOKEN_HPP_
