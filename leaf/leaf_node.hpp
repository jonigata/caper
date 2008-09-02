// 2008/08/11 Naoyuki Hirayama

/*!
	@file	  calc_node.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef LEAF_NODE_HPP_
#define LEAF_NODE_HPP_

#include "leaf_type.hpp"

namespace llvm {
class Module;
class Value;
class Type;
}

namespace leaf {

struct EncodeContext;
struct EntypeContext;

struct Symbol {
	std::string s;

	Symbol( const std::string& x ) : s(x) {}
};

typedef std::map< std::string, leaf::Symbol* > SymDic;
leaf::Symbol* intern( heap_cage& cage, SymDic& sd, const std::string& s );

typedef Type* type_t;
typedef Symbol* symbol_t;
typedef std::vector< type_t > typevec_t;
typedef std::map< symbol_t, type_t > symmap_t;

struct Header {
	int		id;
	int		beg;
	int		end;
	type_t	t;

	Header(){ id = beg = end = -1; t = NULL; }
	Header( int aid, const Header& h )
		: id(aid), beg(h.beg), end(h.end), t(NULL) {}
	Header( int aid, int abeg, int aend )
		: id(aid), beg(abeg), end(aend), t(NULL) {}

	Header operator+( const Header& h ) const
	{
		Header r( *this );
		r.beg = (std::min)( beg, h.beg );
		r.end = (std::max)( end, h.end );
		return r;
	}
	Header& operator+=( const Header& h )
	{
		beg = (std::min)( beg, h.beg );
		end = (std::max)( end, h.end );
		return *this;
	}
};

struct Node {
	Header h;
	
	virtual ~Node(){}
	virtual llvm::Value*	encode( EncodeContext&, bool drop_value )
	{ return NULL; }
	virtual void			entype( EntypeContext&, bool drop_value, type_t )
	{}

	void encode( llvm::Module* );
	void entype( heap_cage&, SymDic& );
};

}

#endif // LEAF_NODE_HPP_
