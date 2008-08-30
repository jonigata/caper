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
    syntax_error( int a, const std::string& at_token ) : error(a)
    {
        std::stringstream ss;
        ss << "syntax error around '" << at_token << "'";
        set_message( ss.str() );
    }
    ~syntax_error() throw () {}
};

class noreturn : public error {
public:
    noreturn( int a, const std::string& type ) : error(a)
    {
        set_message( "function returns no value: " + type + " required" );
    }
    ~noreturn() throw () {}
};

class unexpected_char : public error {
public:
    unexpected_char( int a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "unexpected char: '" << char(ch) << ",";
        set_message( ss.str() );
    }
    ~unexpected_char() throw () {}
};

class mismatch_paren : public error {
public:
    mismatch_paren( int a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "expected ')' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~mismatch_paren() throw () {}
};

class primexpr_expected : public error {
public:
    primexpr_expected( int a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "expected ')' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~primexpr_expected() throw () {}
};

class no_such_variable : public error {
public:
    no_such_variable( int a, const std::string& var ) : error(a)
    {
        std::stringstream ss;
        ss << "no such variable: \"" << var << "\"";
        set_message( ss.str() );
    }
    ~no_such_variable() throw () {}
};

class no_such_function : public error {
public:
    no_such_function( int a, const std::string& func ) : error(a)
    {
        std::stringstream ss;
        ss << "no such function: \"" << func << "\"";
        set_message( ss.str() );
    }
    ~no_such_function() throw () {}
};

class require_fail : public error {
public:
    require_fail( int a, const std::string& module ) : error(a)
    {
        std::stringstream ss;
        ss << "require failed ( cannot open ): \"" << module << "\"";
        set_message( ss.str() );
    }
    ~require_fail() throw () {}
};

class wrong_arity : public error {
public:
    wrong_arity( int a, int fa, int aa, const std::string& func )
        : error(a)
    {
        std::stringstream ss;
        if( aa < fa ) {
            ss << "too many argument to function " << func;
        } else if( fa < aa ) {
            ss << "too few argument to function " << func;
        }
        set_message( ss.str() );
    }
    ~wrong_arity() throw () {}
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
    type_mismatch( int a, const std::string& atype, const std::string& ftype )
        : error(a)
    {
        std::stringstream ss;
        ss << "type mismatch: " << atype << " with " << ftype;
        set_message( ss.str() );
    }
    ~type_mismatch() throw () {}
};

class context_mismatch : public error {
public:
    context_mismatch(
        int a, const std::string& atype, const std::string& ftype )
        : error(a)
    {
        std::stringstream ss;
        ss << "context mismatch: "
           << atype << " appeared where " << ftype <<" required";
        set_message( ss.str() );
    }
    ~context_mismatch() throw () {}
};

class uncallable : public error {
public:
    uncallable( int a, const std::string& func ) : error(a)
    {
        std::stringstream ss;
        ss << "'" << func << "' cannot be used as a function";
        set_message( ss.str() );
    }
    ~uncallable() throw () {}
};

class formalarg_must_be_typed : public error {
public:
    formalarg_must_be_typed( int a, const std::string& formalarg ) : error(a)
    {
        std::stringstream ss;
        ss << "argument \"" << formalarg << "\" must be typed";
        set_message( ss.str() );
    }
    ~formalarg_must_be_typed() throw () {}
};

} // namespace leaf

#endif // ERROR_HPP_
