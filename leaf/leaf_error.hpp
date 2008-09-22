#ifndef LEAF_ERROR_HPP_
#define LEAF_ERROR_HPP_

#include <stdexcept>
#include <sstream>
#include "leaf_node.hpp"

namespace leaf {

////////////////////////////////////////////////////////////////
// errors
class error : public std::exception {
public:
    error( const Addr& a ) : addr( a ), lineno( -1 ), column( -1 ) {}
    virtual ~error() throw() {}
    Addr		addr;

    std::string message;
	std::string	filename;
    int         lineno;
    int         column;

    void set_message( const std::string& m )
    {
        message = m;
    }

    const char* what() const throw () { return message.c_str(); }
};

class syntax_error : public error {
public:
    syntax_error( const Addr& a, const std::string& at_token ) : error(a)
    {
        std::stringstream ss;
        ss << "syntax error around '" << at_token << "'";
        set_message( ss.str() );
    }
    ~syntax_error() throw () {}
};

class noreturn : public error {
public:
    noreturn( const Addr& a, const std::string& type ) : error(a)
    {
        set_message( "function returns no value: " + type + " required" );
    }
    ~noreturn() throw () {}
};

class unexpected_char : public error {
public:
    unexpected_char( const Addr& a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "unexpected char: '" << char(ch) << ",";
        set_message( ss.str() );
    }
    ~unexpected_char() throw () {}
};

class mismatch_paren : public error {
public:
    mismatch_paren( const Addr& a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "expected ')' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~mismatch_paren() throw () {}
};

class primexpr_expected : public error {
public:
    primexpr_expected( const Addr& a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "expected ')' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~primexpr_expected() throw () {}
};

class semicolon_expected : public error {
public:
    semicolon_expected( const Addr& a, int ch ) : error(a)
    {
        std::stringstream ss;
        ss << "expected ';' before '" << char(ch) << "' token";
        set_message( ss.str() );
    }
    ~semicolon_expected() throw () {}
};

class bad_formalarg : public error {
public:
    bad_formalarg( const Addr& a ) : error(a)
    {
        std::stringstream ss;
        ss << "function argument must be '<type>: <varname>'";
        set_message( ss.str() );
    }
    ~bad_formalarg() throw () {}
};

class no_such_variable : public error {
public:
    no_such_variable( const Addr& a, const std::string& var ) : error(a)
    {
        std::stringstream ss;
        ss << "no such variable: \"" << var << "\"";
        set_message( ss.str() );
    }
    ~no_such_variable() throw () {}
};

class no_such_function : public error {
public:
    no_such_function( const Addr& a, const std::string& func ) : error(a)
    {
        std::stringstream ss;
        ss << "no such function: \"" << func << "\"";
        set_message( ss.str() );
    }
    ~no_such_function() throw () {}
};

class require_fail : public error {
public:
    require_fail( const Addr& a, const std::string& module ) : error(a)
    {
        std::stringstream ss;
        ss << "require failed ( cannot open ): \"" << module << "\"";
        set_message( ss.str() );
    }
    ~require_fail() throw () {}
};

class wrong_arity : public error {
public:
    wrong_arity( const Addr& a, int fa, int aa, const std::string& func )
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
    inexplicit_return_type( const Addr& a ) : error(a)
    {
        set_message( "function must have return type" );
    }
    ~inexplicit_return_type() throw () {}
};

class imcomplete_return_type : public error {
public:
    imcomplete_return_type( const Addr& a ) : error(a)
    {
        set_message( "return type must be complete" );
    }
    ~imcomplete_return_type() throw () {}
};

class inexplicit_argument_type : public error {
public:
    inexplicit_argument_type( const Addr& a ) : error(a)
    {
        set_message( "formal argument type must not be omitted" );
    }
    ~inexplicit_argument_type() throw () {}
};

class type_mismatch : public error {
public:
    type_mismatch( const Addr& a, const std::string& atype, const std::string& ftype )
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
        const Addr& a, const std::string& atype, const std::string& ftype )
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
    uncallable( const Addr& a, const std::string& func ) : error(a)
    {
        std::stringstream ss;
        ss << "'" << func << "' cannot be used as a function";
        set_message( ss.str() );
    }
    ~uncallable() throw () {}
};

class formalarg_must_be_typed : public error {
public:
    formalarg_must_be_typed( const Addr& a, const std::string& formalarg ) : error(a)
    {
        std::stringstream ss;
        ss << "argument \"" << formalarg << "\" must be typed";
        set_message( ss.str() );
    }
    ~formalarg_must_be_typed() throw () {}
};

class unused_variable : public error {
public:
    unused_variable( const Addr& a, const std::string& var ) : error(a)
    {
        std::stringstream ss;
        ss << "unused variable '" << var << "'";
        set_message( ss.str() );
    }
    ~unused_variable() throw () {}
};

class wrong_multiple_value : public error {
public:
    wrong_multiple_value( const Addr& a, int vn, int cn ) : error(a)
    {
        std::stringstream ss;
        ss << "multiple value mismatch: "
           << vn << " exprs appeared where "
           << cn << " exprs required";
        set_message( ss.str() );
    }
    ~wrong_multiple_value() throw () {}
};

class ambiguous_type : public error {
public:
    ambiguous_type( const Addr& a, const std::string& n ) : error(a)
    {
        std::stringstream ss;
        ss << "ambigous type: variable " << n;
        set_message( ss.str() );
    }
    ~ambiguous_type() throw () {}
};

class not_struct : public error {
public:
    not_struct( const Addr& a, const std::string& n ) : error(a)
    {
        std::stringstream ss;
        ss << n << " is not struct" << n;
        set_message( ss.str() );
    }
    ~not_struct() throw () {}
};

class no_such_member : public error {
public:
    no_such_member(
		const Addr& a, const std::string& s, const std::string& n ) : error(a)
    {
        std::stringstream ss;
        ss << "struct " << s << " has not such member: " << n;
        set_message( ss.str() );
    }
    ~no_such_member() throw () {}
};

class not_initialized_member : public error {
public:
    not_initialized_member(
		const Addr& a, const std::string& s, const std::string& n ) : error(a)
    {
        std::stringstream ss;
        ss << "struct " << s << " member " << n << " is not initialized";
        set_message( ss.str() );
    }
    ~not_initialized_member() throw () {}
};

class wrong_memberref : public error {
public:
	wrong_memberref(
		const Addr& a, const std::string& s, const std::string& n )
		: error( a )
	{
        std::stringstream ss;
        ss << "'" << s << "' has no member named '" << n << "'";
        set_message( ss.str() );
	}
	~wrong_memberref() throw() {}

};

class wrong_member_type : public error {
public:
	wrong_member_type(
		const Addr& a, const std::string& s, const std::string& n )
		: error( a )
	{
        std::stringstream ss;
        ss << "type mismatch: '" << s << "' is not a type '" << n << "'";
        set_message( ss.str() );
	}
	~wrong_member_type() throw() {}

};

class finally_must_be_the_last_section : public error {
public:
	finally_must_be_the_last_section( const Addr& a ) : error( a )
	{
        set_message( "'finally' must be the last section" );
	}
	~finally_must_be_the_last_section() throw() {}

};

} // namespace leaf

#endif // ERROR_HPP_
