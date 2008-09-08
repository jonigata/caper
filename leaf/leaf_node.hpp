// 2008/08/11 Naoyuki Hirayama

/*!
    @file     calc_node.hpp
    @brief    <ŠT—v>

    <à–¾>
*/

#ifndef LEAF_NODE_HPP_
#define LEAF_NODE_HPP_

#include <boost/noncopyable.hpp>
#include "scoped_allocator.hpp"
#include "leaf_type.hpp"

namespace llvm {
class Module;
class Value;
class Type;
}

namespace leaf {

////////////////////////////////////////////////////////////////
// Symbol
struct Symbol : public boost::noncopyable {
    std::string s;
    Symbol( const std::string& x ) : s(x) {}
};

////////////////////////////////////////////////////////////////
// CompileEnv
struct CompileEnv : public boost::noncopyable {
	typedef std::map< std::string, leaf::Symbol* > symdic_type;

	heap_cage	cage;
	symdic_type	symdic;
	int			idseed;

	Symbol* intern( const std::string& s );
    Symbol* gensym();
};

typedef Type* type_t;
typedef Symbol* symbol_t;
typedef std::vector< type_t > typevec_t;
typedef std::map< symbol_t, type_t > symmap_t;

class Value;

struct Header {
    int     id;
    int     beg;
    int     end;
    type_t  t;

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

struct EncodeContext;
struct EntypeContext;

struct Node {
    Header h;
    
    virtual ~Node(){}
    virtual void    encode( EncodeContext&, bool drop_value, Value& ) {}
    virtual void    entype( EntypeContext&, bool drop_value, type_t ) {}

    void encode( llvm::Module* );
    void entype( CompileEnv& );
};

}

#endif // LEAF_NODE_HPP_
