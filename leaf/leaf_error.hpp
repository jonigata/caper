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
	int			addr;

	std::string filename;
	int			lineno;
	int			column;

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
};

class syntax_error : public error {
public:
	syntax_error( int a, const std::string& t ) : error(a), at_token(t)
	{
		std::stringstream ss;
		ss << "syntax error around '" << at_token << "'";
		message = ss.str();
	}
	~syntax_error() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		at_token;
};

class noreturn : public error {
public:
	noreturn( int a ) : error(a){}
	~noreturn() throw () {}

	const char* what() const throw()
	{
		return "function must have at least 1 expression" ;
	}
};

class duplicated_symbol : public error {
public:
	duplicated_symbol( int a, const std::string& an ) : error(a), name(an)
	{
		std::stringstream ss;
		ss << "duplicated symbol " << name;
		message = ss.str();
	}
	~duplicated_symbol() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		name;
};

class undefined_symbol : public error {
public:
	undefined_symbol( int a, const std::string& an ) : error(a), name(an)
	{
		std::stringstream ss;
		ss << "undefined symbol " << name;
		message = ss.str();
	}
	~undefined_symbol() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		name;
};

class unexpected_char : public error {
public:
	unexpected_char( int a, int c ) : error(a), ch(c)
	{
		std::stringstream ss;
		ss << "unexpected char: '" << char(ch) << ",";
		message = ss.str();
	}
	~unexpected_char() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	int				ch;
};

class mismatch_paren : public error {
public:
	mismatch_paren( int a, int c ) : error(a), ch(c)
	{
		std::stringstream ss;
		ss << "mismatch paren: '" << char(ch) << "'";
		message = ss.str();
	}
	~mismatch_paren() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	int				ch;
};

class bad_directive : public error {
public:
	bad_directive( int a, const std::string& s ) : error(a), directive(s)
	{
		std::stringstream ss;
		ss << "bad directive: \"" << s << "\"";
		message = ss.str();
	}
	~bad_directive() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		directive;
};

class duplicated_semantic_action_argument : public error {
public:
	duplicated_semantic_action_argument( int a, const std::string& m, int i )
		: error(a), method(m), index(i) 
	{
		std::stringstream ss;
		ss << "duplicated semantic action argument: method '" << m << "', index " << i;
		message = ss.str();
	}
	~duplicated_semantic_action_argument() throw (){}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		method;
	int				index;
};

class skipped_semantic_action_argument : public error {
public:
	skipped_semantic_action_argument( int a, const std::string& m, int i )
		: error(a), method(m), index(i)
	{
		std::stringstream ss;
		ss << "skipped semantic action argument: method '" << m << "', index " << i;
		message = ss.str();
	}
	~skipped_semantic_action_argument() throw (){}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		method;
	int				index;
};

class untyped_terminal : public error {
public:
	untyped_terminal( int a, const std::string& m )
		: error(a), terminal_name(m)
	{
		std::stringstream ss;
		ss << "untyped terminal occurs at semantic action argument: terminal '" << m << "'";
		message = ss.str();
	}
	~untyped_terminal() throw (){}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		terminal_name;
};

class no_such_variable : public error {
public:
	no_such_variable( int a, const std::string& v )
		: error(a), variable_name(v)
	{
		std::stringstream ss;
		ss << "no such variable: \"" << v << "\"";
		message = ss.str();
	}
	~no_such_variable() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		variable_name;
};

class no_such_function : public error {
public:
	no_such_function( int a, const std::string& v )
		: error(a), function_name(v)
	{
		std::stringstream ss;
		ss << "no such function: \"" << v << "\"";
		message = ss.str();
	}
	~no_such_function() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		function_name;
};

class require_fail : public error {
public:
	require_fail( int a, const std::string& v )
		: error(a), module_name(v)
	{
		std::stringstream ss;
		ss << "require failed ( no such file ): \"" << v << "\"";
		message = ss.str();
	}
	~require_fail() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		module_name;
};

class wrong_arity : public error {
public:
	wrong_arity( int a, int fa, int aa )
		: error(a), farity(fa), aarity(aa)
	{
		std::stringstream ss;
		ss << "wrong arity: # of " << aarity << " arguments appared "
		   << "where " << farity <<" arguments required";
		message = ss.str();
	}
	~wrong_arity() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	int				farity;
	int				aarity;
};

class inexplicit_return_type : public error {
public:
	inexplicit_return_type( int a ) : error(a){}
	~inexplicit_return_type() throw () {}

	const char* what() const throw()
	{
		return "function must have return type" ;
	}
};

class imcomplete_return_type : public error {
public:
	imcomplete_return_type( int a ) : error(a){}
	~imcomplete_return_type() throw () {}

	const char* what() const throw()
	{
		return "return type must not be incomplete" ;
	}
};

class inexplicit_argument_type : public error {
public:
	inexplicit_argument_type( int a ) : error(a){}
	~inexplicit_argument_type() throw () {}

	const char* what() const throw()
	{
		return "formal argument type must not be omitted" ;
	}
};

class type_mismatch : public error {
public:
	type_mismatch( int a, const std::string& at, const std::string& ft )
		: error(a), atype(at), ftype(ft)
	{
		std::stringstream ss;
		ss << "type mismatch: " << atype << " with " << ftype;
		message = ss.str();
	}
	~type_mismatch() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		atype;
	std::string		ftype;
};

class context_mismatch : public error {
public:
	context_mismatch( int a, const std::string& at, const std::string& ft )
		: error(a), atype(at), ftype(ft)
	{
		std::stringstream ss;
		ss << "context mismatch: "
		   << atype << " appeared where " << ftype <<" required";
		message = ss.str();
	}
	~context_mismatch() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		atype;
	std::string		ftype;
};

class uncallable : public error {
public:
	uncallable( int a, const std::string& v )
		: error(a), func(v)
	{
		std::stringstream ss;
		ss << "can't call: \"" << func << "\"";
		message = ss.str();
	}
	~uncallable() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		func;
};

class formalarg_must_be_typed : public error {
public:
	formalarg_must_be_typed( int a, const std::string& v )
		: error(a), formalarg(v)
	{
		std::stringstream ss;
		ss << "argument \"" << formalarg << "\" must be typed";
		message = ss.str();
	}
	~formalarg_must_be_typed() throw () {}
	const char* what() const throw () { return message.c_str(); }

	std::string		message;
	std::string		formalarg;
};

} // namespace leaf

#endif // ERROR_HPP_
