#ifndef CAPER_ERROR_HPP
#define CAPER_ERROR_HPP

#include "caper_ast.hpp"

////////////////////////////////////////////////////////////////
// errors
class caper_error : public std::exception {
public:
        caper_error( int a ) : addr( a ) {}
        
        int             addr;
};
class syntax_error : public caper_error {
public:
        syntax_error( int a, Token at ) : caper_error(a), at_token(at)
        {
                std::stringstream ss;
                ss << "syntax error around " << at_token;
                message = ss.str();
        }
        ~syntax_error() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        Token           at_token;
};
class duplicated_symbol : public caper_error {
public:
        duplicated_symbol( int a, const std::string& an ) : caper_error(a), name(an)
        {
                std::stringstream ss;
                ss << "duplicated symbol " << name;
                message = ss.str();
        }
        ~duplicated_symbol() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     name;
};
class undefined_symbol : public caper_error {
public:
        undefined_symbol( int a, const std::string& an ) : caper_error(a), name(an)
        {
                std::stringstream ss;
                ss << "undefined symbol " << name;
                message = ss.str();
        }
        ~undefined_symbol() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     name;
};
class unexpected_char : public caper_error {
public:
        unexpected_char( int a, int c ) : caper_error(a), ch(c)
        {
                std::stringstream ss;
                ss << "unexpected char: '" << char(ch) << ",";
                message = ss.str();
        }
        ~unexpected_char() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        int             ch;
};
class mismatch_paren : public caper_error {
public:
        mismatch_paren( int a, int c ) : caper_error(a), ch(c)
        {
                std::stringstream ss;
                ss << "mismatch paren: '" << char(ch) << "'";
                message = ss.str();
        }
        ~mismatch_paren() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        int             ch;
};
class bad_directive : public caper_error {
public:
        bad_directive( int a, const std::string& s ) : caper_error(a), directive(s)
        {
                std::stringstream ss;
                ss << "bad directive: \"" << s << "\"";
                message = ss.str();
        }
        ~bad_directive() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     directive;
};
class duplicated_semantic_action_argument : public caper_error {
public:
        duplicated_semantic_action_argument( int a, const std::string& m, int i )
                : caper_error(a), method(m), index(i) 
        {
                std::stringstream ss;
                ss << "duplicated semantic action argument: method '" << m << "', index " << i;
                message = ss.str();
        }
        ~duplicated_semantic_action_argument() throw (){}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     method;
        int             index;
};
class skipped_semantic_action_argument : public caper_error {
public:
        skipped_semantic_action_argument( int a, const std::string& m, int i )
                : caper_error(a), method(m), index(i)
        {
                std::stringstream ss;
                ss << "skipped semantic action argument: method '" << m << "', index " << i;
                message = ss.str();
        }
        ~skipped_semantic_action_argument() throw (){}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     method;
        int             index;
};
class untyped_terminal : public caper_error {
public:
        untyped_terminal( int a, const std::string& m )
                : caper_error(a), terminal_name(m)
        {
                std::stringstream ss;
                ss << "untyped terminal occurs at semantic action argument: terminal '" << m << "'";
                message = ss.str();
        }
        ~untyped_terminal() throw (){}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     terminal_name;
};
class duplicated_rule : public caper_error {
public:
        duplicated_rule( int a, const tgt::rule& r ) : caper_error(a), rule(r)
        {
                std::stringstream ss;
                ss << "duplicated rule " << r;
                message = ss.str();
        }
        ~duplicated_rule() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        tgt::rule       rule;       
};

#endif // CAPER_ERROR_HPP
