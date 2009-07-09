#ifndef CALC0_HPP_
#define CALC0_HPP_

#include <cstdlib>
#include <cassert>

namespace calc {

enum Token {
    token_eof,
    token_Add,
    token_Div,
    token_Mul,
    token_Number,
    token_Sub,
};

template < class T, int StackSize >
class Stack {
public:
	Stack(){ top_ = 0; gap_ = 0; tmp_ = 0; }
	~Stack(){}
	
	void reset_tmp()
	{
		for( size_t i = 0 ; i < tmp_ ; i++ ) {
			at( StackSize - 1 - i ).~T(); // explicit destructor
		}
		tmp_ = 0;
		gap_ = top_;
	}

	void commit_tmp()
	{
		for( size_t i = 0 ; i < tmp_ ; i++ ) {
			if( gap_ + i < top_ ) {
				at( gap_ + i ) = at( StackSize - 1 - i );
			} else {
				// placement new copy constructor
				new ( &at( gap_ + i ) ) 
				    T( at( StackSize - 1 - i ) );
			}
			at( StackSize - 1 - i).~T(); // explicit destructor
		}
		if( gap_ + tmp_ < top_ ) {
			for( int i = 0 ; i < int( top_ - gap_ - tmp_ ) ; i++ ) {
				at( top_ - 1 - i ).~T(); // explicit destructor
			}
		}

		top_ = gap_ = gap_ + tmp_;
		tmp_ = 0;
	}
	
	bool push( const T& f )
	{
		if( StackSize <= top_ + tmp_ ) { return false; }
		// placement new copy constructor
		new( &at( StackSize - 1 - tmp_++ ) ) T( f );
		return true;
	}

	void pop( size_t n )
	{
		size_t m = n; if( m > tmp_ ) m = tmp_;

		for( size_t i = 0 ; i < m ; i++ ) {
			at( StackSize - tmp_ + i ).~T(); // explicit destructor
		}

		tmp_ -= m;
		gap_ -= n - m;
	}

	const T& top()
	{
		if( 0 < tmp_ ) {
			return at( StackSize - 1 - (tmp_-1) );
		} else {
			return at( gap_ - 1 );
		}
	}

	const T& get_arg( size_t base, size_t index )
	{
		if( base - index <= tmp_ ) {
			return at( StackSize-1-( tmp_ - ( base - index ) ) );
		} else {
			return at( gap_ - ( base - tmp_ ) + index );
		}
	}

	void clear()
	{
		reset_tmp();
		for( size_t i = 0 ; i < top_ ; i++ ) {
			at( i ).~T(); // explicit destructor
		}
		top_ = gap_ = tmp_ = 0;
	}

private:
	T& at( size_t n )
	{
		return *(T*)(stack_ + (n * sizeof(T) ));
	}

private:
	char stack_[ StackSize * sizeof(T) ];
	size_t top_;
	size_t gap_;
	size_t tmp_;

};

template < class Value, class SemanticAction, int StackSize = 1024 >
class Parser {
public:
	typedef Token token_type;
	typedef Value value_type;

public:
	Parser( SemanticAction& sa ) : sa_( sa ) { reset(); }

	void reset()
	{
		error_ = false;
		accepted_ = false;
		clear_stack();
		reset_tmp_stack();
		if( push_stack( &Parser::state_0, &Parser::gotof_0, value_type() ) ) {
			commit_tmp_stack();
		} else {
			sa_.stack_overflow();
			error_ = true;
		}
	}

	bool post( token_type token, const value_type& value )
	{
		assert( !error_ );
		reset_tmp_stack();
		while( (this->*(stack_top()->state) )( token, value ) ); // may throw
		if( !error_ ) {
			commit_tmp_stack();
		}
		return accepted_ || error_;
	}

	bool accept( value_type& v )
	{
		assert( accepted_ );
		if( error_ ) { return false; }
		v = accepted_value_;
		return true;
	}

	bool error() { return error_; }

private:
	typedef Parser< Value, SemanticAction, StackSize > self_type;
	typedef bool ( self_type::*state_type )( token_type, const value_type& );
	typedef bool ( self_type::*gotof_type )( int, const value_type& );

	bool            accepted_;
	bool            error_;
	value_type      accepted_value_;

	SemanticAction& sa_;

	struct stack_frame {
		state_type state;
		gotof_type gotof;
		value_type value;

		stack_frame( state_type s, gotof_type g, const value_type& v )
		    : state( s ), gotof( g ), value( v ) {}
	};

	Stack< stack_frame, StackSize > stack_;
	bool push_stack( state_type s, gotof_type g, const value_type& v )
	{
		bool f = stack_.push( stack_frame( s, g, v ) );
		assert( !error_ );
		if( !f ) {
			error_ = true;
			sa_.stack_overflow();
		}
		return f;
	}

	void pop_stack( size_t n )
	{
		stack_.pop( n );
	}

	const stack_frame* stack_top()
	{
		return &stack_.top();
	}

	const value_type& get_arg( size_t base, size_t index )
	{
		return stack_.get_arg( base, index ).value;
	}

	void clear_stack()
	{
		stack_.clear();
	}

	void reset_tmp_stack()
	{
		stack_.reset_tmp();
	}

	void commit_tmp_stack()
	{
		stack_.commit_tmp();
	}

	bool call_0_MakeAdd( int nonterminal_index, int base, int arg_index0, int arg_index1 )
	{
		int arg0; sa_.downcast( arg0, get_arg( base, arg_index0 ) );
		int arg1; sa_.downcast( arg1, get_arg( base, arg_index1 ) );
		int r = sa_.MakeAdd( arg0, arg1 );
		value_type v; sa_.upcast( v, r );
		pop_stack( 3 );
		return (this->*(stack_top()->gotof))( nonterminal_index, v );
	}

	bool call_0_MakeSub( int nonterminal_index, int base, int arg_index0, int arg_index1 )
	{
		int arg0; sa_.downcast( arg0, get_arg( base, arg_index0 ) );
		int arg1; sa_.downcast( arg1, get_arg( base, arg_index1 ) );
		int r = sa_.MakeSub( arg0, arg1 );
		value_type v; sa_.upcast( v, r );
		pop_stack( 3 );
		return (this->*(stack_top()->gotof))( nonterminal_index, v );
	}

	bool call_0_Identity( int nonterminal_index, int base, int arg_index0 )
	{
		int arg0; sa_.downcast( arg0, get_arg( base, arg_index0 ) );
		int r = sa_.Identity( arg0 );
		value_type v; sa_.upcast( v, r );
		pop_stack( 1 );
		return (this->*(stack_top()->gotof))( nonterminal_index, v );
	}

	bool call_0_MakeDiv( int nonterminal_index, int base, int arg_index0, int arg_index1 )
	{
		int arg0; sa_.downcast( arg0, get_arg( base, arg_index0 ) );
		int arg1; sa_.downcast( arg1, get_arg( base, arg_index1 ) );
		int r = sa_.MakeDiv( arg0, arg1 );
		value_type v; sa_.upcast( v, r );
		pop_stack( 3 );
		return (this->*(stack_top()->gotof))( nonterminal_index, v );
	}

	bool call_0_MakeMul( int nonterminal_index, int base, int arg_index0, int arg_index1 )
	{
		int arg0; sa_.downcast( arg0, get_arg( base, arg_index0 ) );
		int arg1; sa_.downcast( arg1, get_arg( base, arg_index1 ) );
		int r = sa_.MakeMul( arg0, arg1 );
		value_type v; sa_.upcast( v, r );
		pop_stack( 3 );
		return (this->*(stack_top()->gotof))( nonterminal_index, v );
	}

	bool gotof_0( int nonterminal_index, const value_type& v )
	{
		switch( nonterminal_index ) {
		case 0: return push_stack( &Parser::state_1, &Parser::gotof_1, v );
		case 1: return push_stack( &Parser::state_2, &Parser::gotof_2, v );
		default: assert(0); return false;
		}
	}

	bool state_0( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Number:
			// shift
			push_stack( &Parser::state_7, &Parser::gotof_7, value );
			return false;
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_1( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_1( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_eof:
			// accept
			// run_semantic_action();
			accepted_ = true;
			accepted_value_  = get_arg( 1, 0 );
			return false;
		case token_Add:
			// shift
			push_stack( &Parser::state_3, &Parser::gotof_3, value );
			return false;
		case token_Sub:
			// shift
			push_stack( &Parser::state_5, &Parser::gotof_5, value );
			return false;
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_2( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_2( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Div:
			// shift
			push_stack( &Parser::state_10, &Parser::gotof_10, value );
			return false;
		case token_Mul:
			// shift
			push_stack( &Parser::state_8, &Parser::gotof_8, value );
			return false;
		case token_eof:
		case token_Add:
		case token_Sub:
			return call_0_Identity( 0, 1, 0 );
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_3( int nonterminal_index, const value_type& v )
	{
		switch( nonterminal_index ) {
		case 1: return push_stack( &Parser::state_4, &Parser::gotof_4, v );
		default: assert(0); return false;
		}
	}

	bool state_3( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Number:
			// shift
			push_stack( &Parser::state_7, &Parser::gotof_7, value );
			return false;
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_4( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_4( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Div:
			// shift
			push_stack( &Parser::state_10, &Parser::gotof_10, value );
			return false;
		case token_Mul:
			// shift
			push_stack( &Parser::state_8, &Parser::gotof_8, value );
			return false;
		case token_eof:
		case token_Add:
		case token_Sub:
			return call_0_MakeAdd( 0, 3, 0, 2 );
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_5( int nonterminal_index, const value_type& v )
	{
		switch( nonterminal_index ) {
		case 1: return push_stack( &Parser::state_6, &Parser::gotof_6, v );
		default: assert(0); return false;
		}
	}

	bool state_5( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Number:
			// shift
			push_stack( &Parser::state_7, &Parser::gotof_7, value );
			return false;
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_6( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_6( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Div:
			// shift
			push_stack( &Parser::state_10, &Parser::gotof_10, value );
			return false;
		case token_Mul:
			// shift
			push_stack( &Parser::state_8, &Parser::gotof_8, value );
			return false;
		case token_eof:
		case token_Add:
		case token_Sub:
			return call_0_MakeSub( 0, 3, 0, 2 );
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_7( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_7( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_eof:
		case token_Add:
		case token_Div:
		case token_Mul:
		case token_Sub:
			return call_0_Identity( 1, 1, 0 );
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_8( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_8( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Number:
			// shift
			push_stack( &Parser::state_9, &Parser::gotof_9, value );
			return false;
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_9( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_9( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_eof:
		case token_Add:
		case token_Div:
		case token_Mul:
		case token_Sub:
			return call_0_MakeMul( 1, 3, 0, 2 );
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_10( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_10( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_Number:
			// shift
			push_stack( &Parser::state_11, &Parser::gotof_11, value );
			return false;
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

	bool gotof_11( int nonterminal_index, const value_type& v )
	{
		assert(0);
		return true;
	}

	bool state_11( token_type token, const value_type& value )
	{
		switch( token ) {
		case token_eof:
		case token_Add:
		case token_Div:
		case token_Mul:
		case token_Sub:
			return call_0_MakeDiv( 1, 3, 0, 2 );
		default:
			sa_.syntax_error();
			error_ = true;
			return false;
		}
	}

};

} // namespace calc

#endif // #ifndef CALC0_HPP_
