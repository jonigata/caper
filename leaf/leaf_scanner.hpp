// 2008/08/09 Naoyuki Hirayama

/*!
	@file	  scanner.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include "scoped_allocator.hpp"
#include "leaf_node.hpp"
#include "leaf_ast.hpp"
#include "leaf_grammar.hpp"
#include "leaf_error.hpp"

namespace leaf {

typedef std::map< std::string, leaf::Symbol* > SymDic;

template < class It >
class Scanner {
public:
	typedef int char_type;

public:
	Scanner( It b, It e, SymDic& symdic, heap_cage& cage, int& id )
		: b_(b), e_(e), c_(b), unget_(EOF),
		  symdic_( symdic ), cage_(cage), id_(id)
	{
		addr_ = 0;
		reserved_["require"] = leaf::token_Req;
		reserved_["var"] = leaf::token_Var;
		reserved_["fun"] = leaf::token_Fun;
		reserved_["if"] = leaf::token_If;
		reserved_["else"] = leaf::token_Else;
		reserved_["void"] = leaf::token_TypeVoid;
		reserved_["long"] = leaf::token_TypeLong;
		reserved_["int"] = leaf::token_TypeInt;
		reserved_["short"] = leaf::token_TypeShort;
		reserved_["char"] = leaf::token_TypeChar;
		
		lines_.push_back( 0 );
	}

	SymDic&		symdic()	{ return symdic_; }
	heap_cage&	cage()		{ return cage_; }
	int&		idseed()	{ return id_; }

	int addr() { return addr_; }

	int lineno( int addr )
	{
		std::vector<int>::const_iterator i =
			std::upper_bound( lines_.begin(), lines_.end(), addr );
		assert( i != lines_.begin() );
		return int( i - lines_.begin() );
	}
	int column( int addr )
	{
		std::vector<int>::const_iterator i =
			std::upper_bound( lines_.begin(), lines_.end(), addr );
		assert( i != lines_.begin() );
		--i;
		return addr - *i;
	}

	leaf::Token get( leaf::Node*& v )
	{
		int c;
		v = NULL;

		// 空白スキップ
		do {
			c = getc();
		} while( isspace( c ) );

		// 位置
		int b = addr_ - 1;
		int e = addr_;
		
		// 記号類
		switch( c ) {
		case '+': return leaf::token_Add;
		case '-': return leaf::token_Sub;
		case '*': return leaf::token_Mul;
		case '/': return leaf::token_Div;
		case '=':
			{
				c = getc();
				if( c == '=' ) {
					return leaf::token_Eq;
				} else {
					ungetc( c );
					return leaf::token_Assign;
				}
			}
		case '<':
			{
				c = getc();
				if( c == '=' ) {
					return leaf::token_Le;
				} else {
					ungetc( c );
					return leaf::token_Lt;
				}
			}
		case '>':
			{
				c = getc();
				if( c == '=' ) {
					return leaf::token_Ge;
				} else {
					ungetc( c );
					return leaf::token_Gt;
				}
			}
		case '!':
			{
				c = getc();
				if( c == '=' ) {
					return leaf::token_Ne;
				} else {
					ungetc( c );
					return leaf::token_Not;
				}
			}
		case '&':
			{
				c = getc();
				if( c == '&' ) {
					return leaf::token_And;
				} else {
					ungetc( c );
					return leaf::token_BitAnd;
				}
			}
		case '|':
			{
				c = getc();
				if( c == '|' ) {
					return leaf::token_Or;
				} else {
					ungetc( c );
					return leaf::token_BitOr;
				}
			}
		case '(': return leaf::token_LPar;
		case ')': return leaf::token_RPar;
		case '{': return leaf::token_LBra;
		case '}': return leaf::token_RBra;
		case ',': return leaf::token_Comma;
		case ';': return leaf::token_Semicolon;
		case ':': return leaf::token_Colon;
		case '\'':
			{
				c = getc();
				if( c == '\'' ) {
					// ''
					throw unexpected_char( b, c );
				} else if( c == '\\' ) {
					// '\0'
					c = getc();
					switch( c ) {
					case '0': c = '\0'; break;
					case 'n': c = '\n'; break;
					case 't': c = '\t'; break;
					}
					int k = getc();
					if( k != '\'' ) {
						throw unexpected_char( b, k );
					}
					v = h( b, e, cage_.allocate<leaf::LiteralChar>( c ) );
					return leaf::token_LiteralChar;
				} else {
					// 'a'
					int k = getc();
					if( k != '\'' ) {
						throw unexpected_char( b, k );
					}
					v = h( b, e, cage_.allocate<leaf::LiteralChar>( c ) );
					return leaf::token_LiteralChar;
				}
			}
		case EOF: return leaf::token_eof;
		}

		// 識別子
		if( isalpha( c ) ) {
			std::stringstream ss;
			while( c != EOF && ( isalpha( c ) || c == '_' ) ) {
				ss << (char)c;
				c = getc();
			}
			ungetc( c );

			std::string s = ss.str();
			if( s == "true" ) {
				v = h( b, e, cage_.allocate<leaf::LiteralBoolean>( true ) );
				return token_LiteralBoolean;
			} else if( s == "false" ) {
				v = h( b, e, cage_.allocate<leaf::LiteralBoolean>( false ) );
				return token_LiteralBoolean;
			}

			reserved_type::const_iterator i = reserved_.find( s );
			if( i != reserved_.end() ) {
				v = NULL;
				return (*i).second;
			} else {
				SymDic::const_iterator i = symdic_.find( s );
				if( i != symdic_.end() ) {
					v = h( b, e,
						   cage_.allocate<leaf::Identifier>( (*i).second ) );
				} else {
					Symbol* sym = cage_.allocate<leaf::Symbol>( s );
					symdic_[s] = sym;
					v = h( b, e, cage_.allocate<leaf::Identifier>( sym ) );
				}
				return leaf::token_Identifier;
			}
		}

		// 整数
		if( isdigit( c ) ) {
			int n = 0;
			while( c != EOF && isdigit( c ) ) {
				n *= 10;
				n += c - '0';
				c = getc();
			}
			ungetc( c );

			v = h( b, e, cage_.allocate<leaf::LiteralInteger>( n ) );

			return leaf::token_LiteralInteger;
		}


		std::cerr << char(c) << std::endl;
		throw unexpected_char( b, c );
	}

private:
	char_type getc()
	{
		int c;
		if( unget_ != EOF ) {
			c = unget_;
			unget_ = EOF;
		} else if( c_ == e_ ) {
			c = EOF; 
		} else {
			c = *c_++;
			if( c == '\n' ) {
				lines_.push_back( addr_+1 );
			}
		}
		addr_++;
		return c;
	}

	void ungetc( char_type c )
	{
		if( c != EOF ) {
			addr_--;
			unget_ = c;
		}
	}

	template < class T >
	T* h( int b, int e, T* p )
	{
		p->h.id = id_++;
		p->h.beg = b;
		p->h.end = e;
		return p;
	}

private:
	It				b_;
	It				e_;
	It				c_;
	char_type		unget_;
	int				addr_;
	SymDic&			symdic_;
	heap_cage&		cage_;
	int&			id_;

	typedef std::map< std::string, leaf::Token > reserved_type;
	reserved_type reserved_;


	std::vector<int> lines_;
};

} // namespace leaf

#endif // SCANNER_HPP_
