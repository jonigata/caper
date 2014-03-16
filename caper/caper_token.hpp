#ifndef CAPER_TOKEN_HPP_
#define CAPER_TOKEN_HPP_

#include <boost/variant.hpp>

////////////////////////////////////////////////////////////////
// Token
enum Token {
    token_empty,
    token_identifier,
    token_recovery,
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
    token_directive_token,
    token_directive_token_prefix,
    token_directive_external_token,
    token_directive_access_modifier,
    token_directive_namespace,
    token_directive_dont_use_stl,
    token_eof,
};

struct TokenTraits {
    static Token eof() { return token_eof; }
};

inline
const char* display_token(Token op) {
    const char* display[] = {
        "e",
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
        " = ",
        ".",
        "%token",
        "%token_prefix",
        "%external_token",
        "%namespace",
        "%dont_use_stl",
        "$"
    };

    return display[op];
}

inline
std::ostream& operator<<(std::ostream& os, Token op) {
    os <<  display_token(op);
    return os;
}

#endif // CAPER_TOKEN_HPP_
