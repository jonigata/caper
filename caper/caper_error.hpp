#ifndef CAPER_ERROR_HPP
#define CAPER_ERROR_HPP

#include "caper_ast.hpp"
#include "caper_format.hpp"

////////////////////////////////////////////////////////////////
// errors
class caper_error : public std::exception {
public:
    caper_error(int a, const std::string& m)
        : addr(a), message(m) {}

    int         addr;
    std::string message;

    const char* what() const throw () { return message.c_str(); }

    // typesafe sprintf
    template <typename ...T>
    std::string fmt(const std::string& fs, T... args) const {
        return format(fs, args...);
    }
};
class syntax_error : public caper_error {
public:
    syntax_error(int a, Token at)
        : caper_error(a, fmt("syntax error around %s", token_labels(at))) {
    }
};
class duplicated_symbol : public caper_error {
public:
    duplicated_symbol(int a, const std::string& an)
        : caper_error(a, "duplicated symbol " + an) {
    }
};
class undefined_symbol : public caper_error {
public:
    undefined_symbol(int a, const std::string& an)
        : caper_error(a, "undefined symbol " + an) {
    }
};
class empty_type_tag : public caper_error {
public:
    empty_type_tag(int a) : caper_error(a, "empty type tag") {}
};
class unexpected_char : public caper_error {
public:
    unexpected_char(int a, int c)
        : caper_error(a, fmt("unexpected char: '%c'", (char)c)) {
    }
};
class mismatch_paren : public caper_error {
public:
    mismatch_paren(int a, int c)
        : caper_error(a, fmt("mismatch paren: '%c'", c)) {
    }
};
class bad_directive : public caper_error {
public:
    bad_directive(int a, const std::string& s)
        : caper_error(a, fmt("bad directive: \"%s\"", s)) {
    }
};
class duplicated_semantic_action_argument : public caper_error {
public:
    duplicated_semantic_action_argument(int a, const std::string& m, int i)
        : caper_error(
            a,
            fmt("duplicated semantic action argument: method '%s', index %d",
                m, i )) {
    }
};
class skipped_semantic_action_argument : public caper_error {
public:
    skipped_semantic_action_argument(int a, const std::string& m, int i)
        : caper_error(
            a,
            fmt("skipped semantic action argument: method '%s', index %d",
                m, i)){
    }
};
class untyped_terminal : public caper_error {
public:
    untyped_terminal(int a, const std::string& m)
        : caper_error(a, fmt("untyped terminal occurs at semantic action argument: terminal '%s'", m)){
    }
};
class duplicated_rule : public caper_error {
public:
    duplicated_rule(int a, const tgt::rule& r)
        : caper_error(a, make_message(r)) {
    }

    std::string make_message(const tgt::rule& r) {
        std::stringstream ss;
        ss << "duplicated rule " << r;
        return ss.str();
    }
};
class unknown_special_identifier : public caper_error {
public:
    unknown_special_identifier(int a, const std::string& m)
        : caper_error(a, fmt("unknown special identifier '%s'", m)){
    }
};
class unallowed_ebnf : public caper_error {
public:
    unallowed_ebnf(int a)
        : caper_error(a, "EBNF is not allowed, use %allow_ebnf"){
    }
};

class unsupported_feature : public caper_error {
public:
    unsupported_feature(const char* g, const char* f) 
        : caper_error(
            0, fmt("%s generator does not support the feature '%s'", g, f)){
    }
};

#endif // CAPER_ERROR_HPP
