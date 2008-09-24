// 2008/09/24 Naoyuki Hirayama

/*!
	@file	  leaf_environment.hpp
	@brief	  <概要>

	<説明>
*/

#ifndef LEAF_ENVIRONMENT_HPP_
#define LEAF_ENVIRONMENT_HPP_

#include <boost/noncopyable.hpp>
#include <map>
#include "leaf_core.hpp"

namespace leaf {

////////////////////////////////////////////////////////////////
// Environment
template < class T >
class Environment : public boost::noncopyable {
public:
    typedef std::map< Symbol*, T > dic_type;

public:
    Environment(){}
    ~Environment(){}

    void push()
    {
        scope_.resize( scope_.size()+1 );
        //std::cerr << "push" << std::endl;
    }
    void pop()
    {
        scope_.pop_back();
        //std::cerr << "pop" << std::endl;
    }
    void fix()
    {
        scope_.back().modified = false;
    }
    bool modified()
    {
        return scope_.back().modified;
    }
    void update( Symbol* ident, const T& v )
    {
        //std::cerr << "update<" << scope_.size() << ">" << std::endl;
        for( int i = int(scope_.size()) - 1 ; 0 <= i ; i-- ) {
            typename dic_type::iterator j = scope_[i].m.find( ident );
            if( j != scope_[i].m.end() ) {
                if( (*j).second != v ) {
                    (*j).second = v;
                    scope_[i].modified = true;
                }
                return;
            }
        }

        scope_.back().m[ident] = v;
    }
    void bind( Symbol* ident, const T& v )
    {
        scope_.back().m[ident] = v;
    }
    T find( Symbol* ident )
    {
        //std::cerr << "find<" << scope_.size() << ">" << std::endl;
        for( int i = int(scope_.size()) - 1 ; 0 <= i ; i-- ) {
            typename dic_type::const_iterator j = scope_[i].m.find( ident );
            if( j != scope_[i].m.end() ) {
                return (*j).second;
            }
        }
        return T();
    }
    T find_in_toplevel( Symbol* ident )
    {
        typename dic_type::const_iterator j = scope_[0].m.find( ident );
        if( j != scope_[0].m.end() ) {
            return (*j).second;
        }
        return T();
    }

    void print( std::ostream& os )
    {
        for( int i = 0 ; i < int(scope_.size()) ; ++i ) {
            //os << "scope_ " << i << ":" << std::endl;
            for( typename dic_type::const_iterator j = scope_[i].m.begin() ;
                 j != scope_[i].m.end() ;
                 ++j ) {
                os << "  " << (*j).first->s << "(" << (*j).first << ")"
                   << std::endl;
            }
        }
    }

    void refer( Symbol* ident, type_t t )
    {
        // 自由変数の参照宣言
        // findと統一したほうが性能はよいが、
        // わかりやすさのため別メソッドにする

        for( int i = int(scope_.size()) - 1 ; 0 <= i ; i-- ) {
            typename dic_type::const_iterator j = scope_[i].m.find( ident );
            if( j != scope_[i].m.end() ) {
                return;
            } else {
                scope_[i].freevars[ident] = t;
            }
        }
    }

    symmap_t& freevars()
    {
        return scope_.back().freevars;
    }

private:
    struct Scope {
        std::map< Symbol*, T >  m;
        symmap_t                freevars;
        bool                    modified;
    };
    std::vector< Scope > scope_;

    void dump()
    {
        for( int i = 0 ; i < int( scope_.size() ) ; i++ ) {
            //std::cerr << "level " << (i+1) << std::endl;
            for( typename dic_type::const_iterator j = scope_[i].m.begin() ;
                 j != scope_[i].m.end() ;
                 ++j ) {
                //std::cerr << "  " << (*j).first->s << " => " << (*j).second
                //        << std::endl;
                
            }
        }

    }

};

} // namespace leaf

#endif // LEAF_ENVIRONMENT_HPP_
