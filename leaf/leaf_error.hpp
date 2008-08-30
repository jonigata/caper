#ifndef LEAF_ERROR_HPP_
#define LEAF_ERROR_HPP_

#include <stdexcept>
#include <sstream>

namespace leaf {

////////////////////////////////////////////////////////////////
// errors
class error : public std::exception {
public:
    error( int a ) : addr( a ), lineno( -1 ), column( -1 ) {}
    virtual ~error() throw() {}
    int         addr;

    std::string message;
    std::string filename;
    int         lineno;
    int         column;

    bool caught()
    {
        return lineno != -1 && column != -1;
    }

    void set_info( const std::string& a_filename, int a_lineno, int a_column )
    {
        filename = a_filename;
        lineno = a_lineno;
        column = a_column;
    }

    void set_message( const std::string& m )
    {
        message = m;
    }

    const char* what() const throw () { return message.c_str(); }
};

class syntax_error : public error {
public:
    syntax_error( int a, const std::string& t ) : error(a), at_token(t)
    {
        std::stringstream ss;
        ss << "syntax error around '" << at_token << "'";
        set_message( ss.str() );
    }
    ~syntax_error() throw () {}
    std::string     at_token;
};

class noreturn : public error {
public:
    noreturn( int a ) : error(a)
    {
        set_message( "function must have at least 1 expression" );
    }
    ~noreturn() throw () {}
};

class unexpected_char : public error {
public:
    unexpected_char( int a, int c ) : error(a), ch(c)
    {
        std::stringstream ss;
        ss << "unexpected char: '" << char(ch) << ",";
        set_message( ss.str() );
    }
    ~unexpected_char() throw () {}
    int             ch;
};

class mismatch_paren : public error {
public:
    mismatch_paren( int a, int c ) : error(a), ch(c)
    {
        std::stringstream ss;
        ss << "expected ')' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~mismatch_paren() throw () {}
    int             ch;
};

class primexpr_expected : public error {
public:
    primexpr_expected( int a, int c ) : error(a), ch(c)
    {
        std::stringstream ss;
        ss << "expected ')' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~primexpr_expected() throw () {}
    int             ch;
};

class no_such_variable : public error {
public:
    no_such_variable( int a, const std::string& v )
        : error(a), variable_name(v)
    {
        std::stringstream ss;
        ss << "no such variable: \"" << v << "\"";
        set_message( ss.str() );
    }
    ~no_such_variable() throw () {}
    std::string     variable_name;
};

class no_such_function : public error {
public:
    no_such_function( int a, const std::string& v )
        : error(a), function_name(v)
    {
        std::stringstream ss;
        ss << "no such function: \"" << v << "\"";
        set_message( ss.str() );
    }
    ~no_such_function() throw () {}
    std::string     function_name;
};

class require_fail : public error {
public:
    require_fail( int a, const std::string& v )
        : error(a), module_name(v)
    {
        std::stringstream ss;
        ss << "require failed ( no such file ): \"" << v << "\"";
        set_message( ss.str() );
    }
    ~require_fail() throw () {}
    std::string     module_name;
};

class wrong_arity : public error {
public:
    wrong_arity( int a, int fa, int aa )
        : error(a), farity(fa), aarity(aa)
    {
        std::stringstream ss;
        ss << "wrong arity: # of " << aarity << " arguments appared "
           << "where " << farity <<" arguments required";
        set_message( ss.str() );
    }
    ~wrong_arity() throw () {}
    int             farity;
    int             aarity;
};

class inexplicit_return_type : public error {
public:
    inexplicit_return_type( int a ) : error(a)
    {
        set_message( "function must have return type" );
    }
    ~inexplicit_return_type() throw () {}
};

class imcomplete_return_type : public error {
public:
    imcomplete_return_type( int a ) : error(a)
    {
        set_message( "return type must not be incomplete" );
    }
    ~imcomplete_return_type() throw () {}
};

class inexplicit_argument_type : public error {
public:
    inexplicit_argument_type( int a ) : error(a)
    {
        set_message( "formal argument type must not be omitted" );
    }
    ~inexplicit_argument_type() throw () {}
};

class type_mismatch : public error {
public:
    type_mismatch( int a, const std::string& at, const std::string& ft )
        : error(a), atype(at), ftype(ft)
    {
        std::stringstream ss;
        ss << "type mismatch: " << atype << " with " << ftype;
        set_message( ss.str() );
    }
    ~type_mismatch() throw () {}
    std::string     atype;
    std::string     ftype;
};

class context_mismatch : public error {
public:
    context_mismatch( int a, const std::string& at, const std::string& ft )
        : error(a), atype(at), ftype(ft)
    {
        std::stringstream ss;
        ss << "context mismatch: "
           << atype << " appeared where " << ftype <<" required";
        set_message( ss.str() );
    }
    ~context_mismatch() throw () {}
    std::string     atype;
    std::string     ftype;
};

class uncallable : public error {
public:
    uncallable( int a, const std::string& v )
        : error(a), func(v)
    {
        std::stringstream ss;
        ss << "can't call: \"" << func << "\"";
        set_message( ss.str() );
    }
    ~uncallable() throw () {}
    std::string     func;
};

class formalarg_must_be_typed : public error {
public:
    formalarg_must_be_typed( int a, const std::string& v )
        : error(a), formalarg(v)
    {
        std::stringstream ss;
        ss << "argument \"" << formalarg << "\" must be typed";
        set_message( ss.str() );
    }
    ~formalarg_must_be_typed() throw () {}
    std::string     formalarg;
};

} // namespace leaf

#endif // ERROR_HPP_
