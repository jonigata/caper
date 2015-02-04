// 2008/08/11 Naoyuki Hirayama

/*!
    @file     leaf_node.hpp
    @brief    <ŠT—v>

    <à–¾>
*/

#ifndef LEAF_NODE_HPP_
#define LEAF_NODE_HPP_

#include <algorithm>
#include <cassert>
#include <boost/noncopyable.hpp>
#include "scoped_allocator.hpp"
#include "leaf_core.hpp"
#include "leaf_type.hpp"

namespace llvm {
class Module;
class Value;
class Type;
}

namespace leaf {

////////////////////////////////////////////////////////////////
// Addr
struct Addr {
    Addr() { file = NULL; pos = -1; }
    Addr( Symbol* f, int p ) { file = f; pos = p; }
    ~Addr() {}
    Addr( const Addr& a ) : file( a.file ), pos( a.pos ) {}
    
    bool empty() const { return file == NULL && pos < 0; }

    Symbol* file;
    int     pos;
};

inline
std::ostream& operator<<( std::ostream& os, const Addr& a )
{
    os << "Addr( ";
    if( a.file ) {
        os << a.file->s;
    } else {
        os << "NULL";
    }
    os << ", " << a.pos << " )";
    return os;
}

////////////////////////////////////////////////////////////////
// SrcMarker
class SrcMarker : public boost::noncopyable {
public:
    struct Lines {
        std::vector< int > v;
    };

public:
    SrcMarker() {}

    Lines& get_lines( Symbol* file ) { return d_[file]; }
    
    int lineno( const Addr& a ) const 
    {
        assert( a.file );
        std::map< Symbol*, Lines >::const_iterator l = d_.find( a.file );
        assert( l != d_.end() );

        const std::vector< int >& lines = (*l).second.v;

        std::vector<int>::const_iterator i =
            std::upper_bound( lines.begin(), lines.end(), a.pos );
        assert( i != lines.begin() );
        return int( i - lines.begin() );
    }
    int column( const Addr& a ) const
    {
        assert( a.file );
        std::map< Symbol*, Lines >::const_iterator l = d_.find( a.file );
        assert( l != d_.end() );

        const std::vector< int >& lines = (*l).second.v;

        std::vector<int>::const_iterator i =
            std::upper_bound( lines.begin(), lines.end(), a.pos );
        assert( i != lines.begin() );
        --i;
        return a.pos - *i;
    }

private:
    std::map< Symbol*, Lines > d_;

};

////////////////////////////////////////////////////////////////
// CompileEnv
struct CompileEnv : public boost::noncopyable {
    typedef std::map< std::string, leaf::Symbol* > symdic_type;

    heap_cage       cage;
    symdic_type     symdic;
    int             idseed;
    SrcMarker       sm;

    Symbol* intern( const std::string& s )
	{
		Symbol* sym;

		symdic_type::const_iterator i = symdic.find( s );
		if( i != symdic.end() ) {
			sym = (*i).second;
		} else {
			sym = cage.allocate<Symbol>( s );
			symdic[s] = sym;
		}
		return sym;
	}
    Symbol* gensym()
	{
            return intern("$gensym" + std::to_string(idseed++));
	}
};

////////////////////////////////////////////////////////////////
// AST primitive types
typedef Type*                           type_t;
typedef Symbol*                         symbol_t;
typedef std::vector< type_t >           typevec_t;
typedef std::map< symbol_t, type_t >    symmap_t;

////////////////////////////////////////////////////////////////
// Header
struct Header {
    int     id  = -1;
    Addr    beg;
    Addr    end;
    type_t  t   = nullptr;

    Header(){}
    Header(const Header& h)
        : beg(h.beg), end(h.end), t(NULL) { }
    Header(int aid, const Addr& abeg, const Addr& aend)
        : id(aid), beg(abeg), end(aend), t(NULL) {}
    Header(int aid, const Header& h)
        : id(aid), beg(h.beg), end(h.end), t(NULL) { }

    Header operator+( const Header& h ) const
    {
        assert( beg.file == end.file );
        assert( h.beg.file == h.end.file );

        if( beg.file && !h.beg.file ) {
            return *this;
        } else if( !beg.file && h.beg.file ) {
            return h;
        } else if( !beg.file && !h.beg.file ) {
            return *this;
        } else {
            assert( beg.file == h.beg.file );
            Header r( *this );
            r.beg.pos = (std::min)( beg.pos, h.beg.pos );
            r.end.pos = (std::max)( end.pos, h.end.pos );
            return r;
        }
    }
    Header& operator+=( const Header& h )
    {
        assert( beg.file == end.file );
        assert( h.beg.file == h.end.file );

        if( beg.file && !h.beg.file ) {
            // do nothing
        } else if( !beg.file && h.beg.file ) {
            *this = h;
        } else if( !beg.file && !h.beg.file ) {
            // do nothing
        } else {
            assert( beg.file == h.beg.file );
            beg.pos = (std::min)( beg.pos, h.beg.pos );
            end.pos = (std::max)( end.pos, h.end.pos );
        }
        return *this;
    }
};

struct AlphaContext;
struct EncodeContext;
struct EntypeContext;
class Value;

struct Node {
    Header h;	// h.t ‚Í
				//   Expr‚Ì”h¶ƒNƒ‰ƒX => Ž®‚ÌŒ^
				//   TypeExpr‚Ì”h¶ƒNƒ‰ƒX => Œ^Ž®‚Ì’l(‚Â‚Ü‚è•\Ž¦‚µ‚Ä‚¢‚éŒ^)
				//   ‚»‚Ì‘¼ => NULL
    
    virtual ~Node(){}
	virtual void	alpha( AlphaContext& ) {}
    virtual void    encode( EncodeContext&, bool drop_value, Value& ) {}
    virtual void    entype( EntypeContext&, bool drop_value, type_t ) {}
	virtual void	display( int indent, std::ostream& ) {}

	void alpha( CompileEnv& );
    void encode( CompileEnv&, llvm::Module* );
    void entype( CompileEnv& );
};

}

#endif // LEAF_NODE_HPP_
