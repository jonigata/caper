// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#ifndef LR_HPP
#define LR_HPP

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include "grammar.hpp"

namespace zw {

namespace gr {

/*============================================================================
 *
 * utility functions for marging sets
 *
 * セットのマージ用ユーティリティ関数
 *
 *==========================================================================*/

template < class D, class S > inline void merge_sets( D& x, const S& y ) { x.insert( y.begin(), y.end() ); }

/*============================================================================
 *
 * class core
 *
 * LR(0)項
 *
 *==========================================================================*/

template < class Token, class Traits >
class core {
public:
    typedef zw::gr::core< Token, Traits >           self_type;
    typedef zw::gr::rule< Token, Traits >           rule_type;
    typedef zw::gr::symbol< Token, Traits >        symbol_type;
    
public:
    core( int id, const rule_type&  r, int i ) : id_( id ), rule_( r ), cursor_( i ) {}
    core( int id, const self_type& x ) : id_( x.id ), rule_( x.rule_ ), cursor_( x.cursor_ ) {}
    ~core(){}

    self_type& operator=( const self_type& x )
    {
        id_     = x.id_;
        rule_   = x.rule_;
        cursor_ = x.cursor_;
        return *this;
    }

    int                     id() const      { return id_; }
    const rule_type&        rule() const    { return rule_; }
    int                     cursor() const  { return cursor_; }

    const symbol_type&      curr() const    { return rule_.right()[cursor_]; }
    bool                    over() const    { return int( rule_.right().size() ) <= cursor_; }

private:
    int             id_;
    rule_type       rule_;
    int             cursor_;
    
};
    
template < class Token, class Traits >
bool operator<( const core< Token, Traits >& x,
                const core< Token, Traits >& y )
{
    if( x.id() < y.id() ) { return true; }
    if( y.id() < x.id() ) { return false; }
    return x.cursor() < y.cursor();
}

template < class Token, class Traits >
bool operator==( const core< Token, Traits >& x,
                 const core< Token, Traits >& y )
{
    return x.id() == y.id() && x.cursor() == y.cursor();
}

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const core< Token, Traits >& y )
{
    typedef rule< Token, Traits > rule_type;

    const rule_type& r = y.rule();
    os << "r" << y.id() << ": " << r.left() << " ::=";

    int n = 0;
    for( typename rule_type::elements_type::const_iterator i = r.right().begin() ;
         i != r.right().end() ;
         ++i ) {
        if( n++ == y.cursor() ) { os << " _"; }
        os << " " << (*i);
    }
    if( y.cursor() == int( r.right().size() ) ) { os << " _"; }
    return os;
}

/*============================================================================
 *
 * class item
 *
 * LR(1)項
 *
 *==========================================================================*/

template <class Token,class Traits >
class item {
public:
    typedef zw::gr::core< Token, Traits >   core_type;
    typedef zw::gr::item< Token, Traits >   self_type;
    typedef zw::gr::symbol< Token, Traits > symbol_type;
    typedef zw::gr::rule< Token, Traits >   rule_type;
    
public:
    item( int id, const rule_type& r, int c, const symbol_type& s ) : core_( id, r, c ), lookahead_( s ) {}
    item( const core_type& x, const symbol_type& y ) : core_( x ), lookahead_( y ) {}
    item( const self_type& x ) : core_( x.core_ ), lookahead_( x.lookahead_ ) {}
    ~item(){}

    self_type& operator=( const self_type& x )
    {
        core_ = x.core_;
        lookahead_ = x.lookahead_;
        return *this;
    }

    const core_type& core() const           { return core_; }
    const symbol_type& lookahead() const    { return lookahead_; }

    int                     id() const      { return core_.id(); }
    const rule_type&        rule() const    { return core_.rule(); }
    int                     cursor() const  { return core_.cursor(); }

    const symbol_type&      curr() const    { return core_.curr(); }
    bool                    over() const    { return core_.over(); }

    bool less_than( const self_type& y ) const
    {
        if( core_ < y.core_ ) { return true; }
        if( y.core_ < core_ ) { return false; }
        return lookahead_ < y.lookahead_;
    }

    bool equal( const self_type& y ) const
    {
        return core_ == y.core_ && lookahead_ == y.lookahead_;
    }

private:
    core_type       core_;
    symbol_type     lookahead_;
    
};

template < class Token, class Traits >
bool operator<( const item< Token, Traits >& x, const item< Token, Traits >& y )
{
    return x.less_than( y );
}

template < class Token, class Traits >
bool operator==( const item< Token, Traits >& x, const item< Token, Traits >& y )
{
    return x.equal( y );
}

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const item< Token, Traits >& y )
{
    //typedef core< Token, Traits > core_type;
    os << y.core() << " / " << y.lookahead();
    return os;
}

/*============================================================================
 *
 * class symbol_set
 *
 * シンボル集合
 *
 *==========================================================================*/

template < class Token, class Traits >
class symbol_set : public std::unordered_set< symbol< Token, Traits >, symbol_hash< Token, Traits > > {
public:
    symbol_set() {}
    ~symbol_set() {}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const symbol_set< Token, Traits >& y )
{
    os << '{';
    for( typename symbol_set< Token, Traits >::const_iterator i = y.begin() ; i != y.end() ; ++i ) {
        os << (*i) << ",";
    }
    os << '}';
    return os;
}

/*============================================================================
 *
 * class core_set
 *
 * LR(0)項集合
 *
 *==========================================================================*/

template <class Token,class Traits >
class core_set : public std::set< core< Token, Traits > > {
public:
    core_set() {}
    ~core_set() {}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const core_set< Token, Traits>& s )
{
    os << "{ ";
    for( typename core_set< Token, Traits >::const_iterator j = s.begin(); j != s.end(); ++j ) {
        std::cerr << (*j) << "; ";
    }
    os << "}";
    return os;
}

/*============================================================================
 *
 * class item_set
 *
 * LR(1)項集合
 *
 *==========================================================================*/

template <class Token,class Traits >
class item_set : public std::set< item< Token, Traits >  > {
public:
    item_set(){}
    ~item_set(){}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const item_set< Token, Traits>& s )
{
    os << "{ ";
    for( typename item_set< Token, Traits >::const_iterator j = s.begin(); j != s.end() ; ++j ) {
        os << (*j) << "; ";
    }
    os << "}";
    return os;
}

/*============================================================================
 *
 * class first_collection
 *
 * FIRST(a)のコレクション
 *
 *==========================================================================*/

template <class Token,class Traits >
class first_collection : public std::unordered_map< symbol< Token, Traits >, symbol_set< Token, Traits >, symbol_hash< Token, Traits > > {
public:
    first_collection() {}
    ~first_collection() {}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const first_collection< Token, Traits>& s )
{
    os << "{\n";
    for( typename first_collection< Token, Traits >::const_iterator j = s.begin(); j != s.end() ; ++j ) {
        os << "    " << (*j).first << " = " << (*j).second << "; " << std::endl;
    }
    os << "}\n";
    return os;
}

/*============================================================================
 *
 * class follow_collection
 *
 * FOLLOW(a)のコレクション
 *
 *==========================================================================*/

template <class Token,class Traits >
class follow_collection : public std::unordered_map< symbol< Token, Traits >, symbol_set< Token, Traits >, symbol_hash< Token, Traits > > {
public:
    follow_collection() {}
    ~follow_collection() {}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const follow_collection< Token, Traits>& s )
{
    os << "{\n";
    for( typename follow_collection< Token, Traits >::const_iterator j = s.begin(); j != s.end() ; ++j ) {
        os << "    " << (*j).first << " = " << (*j).second << "; " << std::endl;
    }
    os << "}\n";
    return os;
}

/*============================================================================
 *
 * class lr0_collection
 *
 * LR(0)集
 *
 *==========================================================================*/

template < class Token, class Traits >
class lr0_collection : public std::set< core_set< Token, Traits > > {
public:
    lr0_collection() {}
    ~lr0_collection() {}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const lr0_collection< Token, Traits >& C )
{
    for( typename lr0_collection< Token, Traits >::const_iterator i = C.begin() ; i != C.end() ; ++i ) {
        os << (*i) << std::endl;
    }
    return os;
}

/*============================================================================
 *
 * class lr1_collection
 *
 * LR(1)集
 *
 *==========================================================================*/

template < class Token, class Traits >
class lr1_collection : public std::set< item_set< Token, Traits > > {
public:
    lr1_collection() {}
    ~lr1_collection() {}
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const lr1_collection< Token, Traits >& C )
{
    for( typename lr1_collection< Token, Traits >::const_iterator i = C.begin() ; i != C.end() ; ++i ) {
        os << (*i) << std::endl;
    }
    return os;
}

/*============================================================================
 *
 * collect_symbols
 *
 *
 *
 *==========================================================================*/

template < class Token, class Traits >
void collect_symbols(
    symbol_set< Token, Traits >&    terminals,
    symbol_set< Token, Traits >&    nonterminals,
    symbol_set< Token, Traits >&    all_symbols,
    const grammar< Token, Traits >& g )
{
    typedef rule< Token, Traits >           rule_type;
    typedef grammar< Token, Traits >        grammar_type;

    for( typename grammar_type::const_iterator i = g.begin() ; i != g.end() ; ++i ) {
        nonterminals.insert( (*i).left() );
        all_symbols.insert( (*i).left() );
        for( typename rule_type::elements_type::const_iterator j = (*i).right().begin() ;
             j != (*i).right().end() ;
             ++j ) {
            if( (*j).is_terminal() ) { terminals.insert( *j ); }
            if( (*j).is_nonterminal() ) { nonterminals.insert( *j ); }
            all_symbols.insert( *j ); 
        }
    }
}

/*============================================================================
 *
 * make_first_and_follow
 *
 *
 *
 *==========================================================================*/

template < class Token, class Traits >
bool all_nullable(
    const std::unordered_set< symbol< Token, Traits >, symbol_hash< Token, Traits > >& nullable,
    const std::vector< symbol< Token, Traits > > &    rule_right,
    int b, int e )
{
    bool flag = true;
    for( int j = b ; j < e ; j++ ) {
        if( rule_right[j].is_epsilon() ) { continue; }

        typename std::unordered_set< symbol< Token, Traits >, symbol_hash< Token, Traits > >::const_iterator i = 
            nullable.find( rule_right[j] );
        if( i == nullable.end() ) {
            flag = false;
            break;
        }
    }
    return flag;
}

template < class Token, class Traits >
void make_first_and_follow(
    first_collection< Token, Traits >&      first,
    follow_collection< Token, Traits >&     follow,
    const symbol_set< Token, Traits >&      terminals,
    const symbol_set< Token, Traits >&      nonterminals,
    const symbol_set< Token, Traits >&      all_symbols,
    const grammar< Token, Traits >&         g )
{
    typedef symbol_set< Token, Traits >     symbol_set_type;
    typedef grammar< Token, Traits >        grammar_type;
    typedef rule< Token, Traits >           rule_type;

    // nullable
    std::unordered_set< symbol< Token, Traits >, symbol_hash< Token, Traits > > nullable;

    // For each terminal symbol Z, FIRST[Z] = {Z}.
    for( typename symbol_set_type::const_iterator i = terminals.begin() ;
         i != terminals.end() ;
         ++i ) {
        first[*i].insert( *i );
    }

    // repeat until FIRST, FOLLOW, and nullable did not change in this iteration.
    // TODO: prb使ったほうが速いかも
    bool repeat = true;
    while( repeat ) {
        repeat = false;

        // for each production X -> Y1Y2...Yk
        for( typename grammar_type::const_iterator i = g.begin() ; i != g.end() ; ++i ) {
            const rule_type& rule = *i;

            // if Y1...Yk are all nullable ( or if k = 0 )
            int k = int( rule.right().size() );
            if( all_nullable( nullable, rule.right(), 0, k ) ) {
                // nullable[X] = true
                if( !nullable.count(rule.left()) ) {
                    repeat = true; 
                    nullable.insert( rule.left() );
                }
            } 

            // for each i from 1 to k, each j from i + 1 to k
            for( int i = 0 ; i < k ; i++ ) {
                // if Y1...Yi-1 are all nullable ( or if i = 1 )
                if( all_nullable( nullable, rule.right(), 0, i ) ) {
                    // then FIRST[X] = FIRST[X] unify FIRST[Yi]
                    symbol_set_type s = first[ rule.left() ];
                    merge_sets( s, first[ rule.right()[i] ] );

                    if( first[rule.left()] != s ) {
                        repeat = true;
                        first[ rule.left() ] = s;
                    }
                }
                // if Yi+1...Yk are all nullable ( or if i = k )
                if( all_nullable( nullable, rule.right(), i+1, k ) ) {
                    // then FOLLOW[Yi] = FOLLOW[Yi] unify FOLLOW[X]
                    symbol_set_type s = follow[ rule.right()[i] ];
                    merge_sets( s, follow[ rule.left() ] );

                    if( follow[ rule.right()[i] ] != s ) {
                        repeat = true;
                        follow[ rule.right()[i] ] = s;
                    }
                }

                for( int j = i+1 ; j < k ; j++ ) {
                    // if Yi+1...Yj-1 are all nullable ( or if i+1 = j )
                    // then FOLLOW[Yi] = FOLLOW[Yi] unify FIRST[Yj]
                    if( all_nullable( nullable, rule.right(), i+1, j ) ) {
                        symbol_set_type s = follow[ rule.right()[i] ];
                        merge_sets( s, first[ rule.right()[j] ] );

                        if( follow[ rule.right()[i] ] != s ) {
                            repeat = true;
                            follow[ rule.right()[i] ] = s;
                        }
                    }
                }
            }
                        
        }
    }

    for( typename std::unordered_set< symbol< Token, Traits >, symbol_hash< Token, Traits > >::const_iterator i = nullable.begin() ;
         i != nullable.end() ;
         ++i ) {
        first[*i].insert( epsilon< Token, Traits >() );
    }
}

template < class Token, class Traits >
void make_vector_first(
    symbol_set< Token, Traits >&                    s,
    const first_collection< Token, Traits >&        first,
    const std::vector< symbol< Token, Traits > > &  v )
{
    typedef symbol_set< Token, Traits > symbol_set_type;

    bool next = false;
    for( size_t i = 0 ; i < v.size() ; i++ ) {
        next = false;

        if( v[i].is_terminal() ) {
            s.insert( v[i] );
            return;
        }

        typename first_collection< Token, Traits >::const_iterator j = first.find( v[i] );
        assert( j != first.end() );

        const symbol_set_type& f = ( *j ).second;
        for( typename symbol_set_type::const_iterator k = f.begin(); k != f.end() ; ++k ) {
            if( (*k).is_epsilon() ) {
                next = true;
            } else {
                s.insert( *k );
            }
        }
        if( !next ) { break; }
    }

    if( next ) {
        s.insert( epsilon< Token, Traits >() );
    }
}

/*============================================================================
 *
 * make_lr0_closure
 *
 * LR(0)closureの作成
 *
 *==========================================================================*/

template <class Token,class Traits>
void
make_lr0_closure(
    core_set< Token, Traits >&      J,
    const grammar< Token, Traits >& g )
{
    typedef symbol< Token, Traits >         symbol_type;
    typedef rule< Token, Traits >           rule_type;
    typedef core< Token, Traits >           core_type;
    typedef core_set< Token, Traits >       core_set_type;

    std::unordered_set<std::string> added;

    bool repeat;
    do {
        core_set_type new_cores;

        repeat = false;
        typename core_set_type::const_iterator end1 = J.end();
        for( typename core_set_type::const_iterator i = J.begin() ; i != end1 ; ++i ) {
            const core_type& x = (*i);
            if( int( x.rule().right().size() ) <= x.cursor() ) { continue; }

            const symbol_type& y = x.rule().right()[ x.cursor() ];
            if( !y.is_nonterminal() ) { continue; }
            if( added.find( y.name() ) != added.end() ) { continue; }

            for (const rule_type& z: (*g.dictionary().find(y.name())).second) {
                new_cores.insert(core_type((*g.indices().find(z)).second, z, 0)); 
                repeat = true;
            }
            added.insert( y.name() );
        }

        J.insert( new_cores.begin(), new_cores.end() );
    } while(repeat);
}

/*============================================================================
 *
 * make_lr0_goto
 *
 * LR(0)gotoの作成
 *
 *==========================================================================*/

template <class Token,class Traits>
void make_lr0_goto(
    core_set< Token, Traits >&              J,
    const core_set< Token, Traits >&        I,
    const symbol< Token, Traits >&          X,
    const grammar< Token, Traits >&         g )
{
    typedef symbol< Token, Traits >         symbol_type;
    typedef core< Token, Traits >           core_type;
    typedef core_set< Token, Traits >       core_set_type;

    for( typename core_set_type::const_iterator i = I.begin() ; i != I.end() ; ++i ) {
        const core_type& x=(*i);
        if( x.over() ) { continue; }

        const symbol_type& y = x.curr(); 
        if( !( y == X ) ) { continue; }

        J.insert( core_type( x.id(), x.rule(), x.cursor() + 1 ) ) ; 
    }

    make_lr0_closure( J, g );
}

/*============================================================================
 *
 * make_lr1_closure
 *
 * LR(1)closureの作成
 *
 *==========================================================================*/

template < class Token, class Traits >
void
make_lr1_closure(
    item_set< Token, Traits >&                      J,
    const first_collection< Token, Traits >&        first,        
    const grammar<Token,Traits>&                    g)
{
    typedef symbol< Token, Traits >                 symbol_type;
    typedef rule< Token, Traits >                   rule_type;
    typedef item< Token, Traits >                   item_type;
    typedef symbol_set< Token, Traits >             symbol_set_type;
    typedef item_set< Token, Traits >               item_set_type;
    typedef std::vector< symbol< Token, Traits >  > symbol_vector_type;

    size_t J_size;
    do {
        item_set_type new_items;  // 挿入する項
        
        J_size = J.size();

        for (const item_type& x: J) { 
            // x is [item(A→α・Bβ,a)]
            if( x.over() ) { continue; }

            // y is [symbol(B)]
            const symbol_type& y = x.curr();
            if( !y.is_nonterminal() ) { continue; }

            // v is [symbol_vector_type(βa)]
            symbol_vector_type v;
            for( int j = x.cursor() + 1 ; j < int( x.rule().right().size() ) ; j++ ) {
                v.push_back( x.rule().right()[j] );
            }
            v.push_back( x.lookahead() );

            // f is FIRST(βa)
            symbol_set_type f;
            make_vector_first( f, first, v ); 

            for (const rule_type& z: (*g.dictionary().find(y.name())).second) {
                // z is [rule(B→γ)]

                // 各lookahead
                for(const symbol_type& s: f) {
                    new_items.insert(item_type((*g.indices().find(z)).second, z, 0, s));
                }
            }
        }

        merge_sets( J, new_items );
    } while( J_size != J.size() );
}

/*============================================================================
 *
 * make_lr1_goto
 *
 * LR(1)gotoの作成
 *
 *==========================================================================*/

template < class Token, class Traits >
void
make_lr1_goto(
    item_set< Token, Traits >&                      J,
    const item_set< Token, Traits >&                I,
    const symbol< Token, Traits >&                  X,
    const first_collection< Token, Traits >&        first,
    const grammar< Token, Traits >&                 g )
{
    typedef symbol< Token, Traits >         symbol_type;
    typedef item< Token, Traits >           item_type;
    typedef item_set< Token, Traits >       item_set_type;

    for( typename item_set_type::const_iterator i = I.begin() ; i != I.end() ; ++i ) {
        const item_type& x = (*i);
        if( x.over() ) { continue; }

        const symbol_type& y = x.curr();
        if( !( y == X ) ) { continue; }
        J.insert( item_type( x.id(), x.rule(), x.cursor()+1, x.lookahead() ) );
    }

    make_lr1_closure( J, first, g );
}

/*============================================================================
 *
 * make_lr0_collection
 *
 * 正準LR(0)集の作成
 *
 *==========================================================================*/

template <class Token,class Traits>
void
make_lr0_collection(
    lr0_collection< Token, Traits >&        C,
    const grammar< Token, Traits >&         g )
{
    typedef symbol< Token, Traits >         symbol_type;
    //typedef rule< Token, Traits >           rule_type;
    typedef grammar< Token, Traits >        grammar_type ; 
    typedef core< Token, Traits >           core_type ; 
    //typedef item< Token, Traits >           item_type ; 
    typedef lr0_collection< Token, Traits > lr0_collection_type ; 
    typedef symbol_set< Token, Traits > symbol_set_type ; 
    typedef core_set< Token, Traits >   core_set_type ; 

    // 記号の収集
    symbol_set_type syms;
    for( typename grammar_type::const_iterator i = g.begin(); i != g.end() ; ++i ) {
        syms.insert( (*i).left() );
        syms.insert( (*i).right().begin(), (*i).right().end() );
    }

    // 正準集の作成
    core_set_type s;
    s.insert( core_type( 0, g.root_rule(), 0 ) );
    make_lr0_closure( s, g );
    C.insert( s );
        
    bool repeat;
    do {
        lr0_collection_type new_collection; // 挿入する項集合

        repeat=false;
        for( typename lr0_collection_type::const_iterator i = C.begin() ; i != C.end() ; ++i ) {
            const core_set_type& I = *i;
            
            for( typename symbol_set_type::const_iterator j = syms.begin() ; j != syms.end() ; ++j ) {
                const symbol_type& X = *j;
                core_set_type I_dash;
                make_lr0_goto( I_dash, I, X, g );

                if( !I_dash.empty() && C.find( I_dash ) == C.end() ) {
                    new_collection.insert( I_dash );
                    repeat=true;
                }
            }
        }

        C.insert( new_collection.begin(), new_collection.end() );
    } while(repeat);
}

template < class Token, class Traits > 
void 
make_lr1_collection( 
    lr1_collection< Token, Traits >&               C, 
    const first_collection< Token, Traits>&        first, 
    const symbol_set< Token, Traits >&             symbols, 
    const grammar< Token, Traits >&                g) 
{ 
    typedef terminal< Token, Traits >              terminal_type; 
    //typedef rule< Token, Traits >                  rule_type; 
    //typedef grammar< Token, Traits >               grammar_type; 
    typedef lr1_collection< Token, Traits >        lr1_collection_type; 
    typedef symbol_set< Token, Traits >            symbol_set_type; 
    typedef item< Token, Traits >                  item_type; 
    typedef item_set< Token, Traits >              item_set_type; 
  
    item_set_type s; 
    s.insert( item_type( 0, g.root_rule(), 0, terminal_type( "$", Traits::eof() ) ) ); 
  
    make_lr1_closure( s, first, g ); 
    C.insert( s ); 
  
    bool repeat; 
    do { 
        std::set<item_set_type > new_items;
                  
        repeat = false; 
        for( typename lr1_collection_type::const_iterator c = C.begin() ; c != C.end() ; ++c ) { 
            const item_set_type& I = *c; 
              
            for( typename symbol_set_type::const_iterator i = symbols.begin() ; 
                 i != symbols.end() ; 
                 ++i ) { 
                item_set_type I_dash; 
                make_lr1_goto( I_dash, I, *i, first, g ); 
  
                if( !I_dash.empty() && C.find(I_dash) == C.end() ){ 
                    new_items.insert( I_dash ); 
                    repeat = true; 
                } 
            } 
        } 
          
        merge_sets( C, new_items ); 
    } while( repeat ); 
} 
 
/*============================================================================
 *
 * choose_kernel
 *
 * LR(0)項集合から主要素項を求める
 *
 *==========================================================================*/

template < class Token, class Traits >
void
choose_kernel(
    core_set< Token, Traits>&       K,
    const core_set< Token, Traits>& I,
    const grammar< Token, Traits >& g )
{
    typedef core_set< Token, Traits >   core_set_type;

    for( typename core_set_type::const_iterator i = I.begin() ; i != I.end() ; ++i ) {
        if( (*i).rule() == g.root_rule() || 0 < (*i).cursor() ) {
            K.insert( *i );
        }
    }
}

/*============================================================================
 *
 * items_to_cores
 *
 * 
 *
 *==========================================================================*/

template < class Token, class Traits >
void
items_to_cores(
    core_set<Token,Traits> &        xx,
    const item_set<Token,Traits>&   x)
{
    typedef core< Token, Traits >           core_type;
    typedef item_set< Token, Traits >       item_set_type;

    for( typename item_set_type::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
        xx.insert( core_type( (*i).id(), (*i).rule(), (*i).cursor() ) );
    }
}

/*============================================================================
 *
 * class parsing_table
 *
 * 解析表
 *
 *==========================================================================*/

enum action_t {
    action_shift,
    action_reduce,
    action_accept,
    action_error,
};

template < class Token, class Traits >
class parsing_table {
public:
    struct state;

    typedef Token                           token_type;
    typedef Traits                          traits_type;
    typedef parsing_table<Token,Traits>     self_type;
    typedef symbol< Token, Traits >         symbol_type;
    typedef symbol_set< Token, Traits >     symbol_set_type;
    typedef rule< Token, Traits >           rule_type;
    typedef std::vector< rule_type >        rules_type;
    typedef core< Token, Traits >           core_type; 
    typedef item< Token, Traits >           item_type; 
    
    struct action {
        action_t type;

        int dest_index; // index to states_
        int rule_index; // index to rules_
    };

    struct state {
    public:
        typedef item_set< Token, Traits >                       item_set_type;
        typedef core_set< Token, Traits >                       core_set_type;
        typedef std::map< Token, action >                       action_table_type;
        typedef std::map< symbol_type, int >                    goto_table_type; // index to states_
        typedef std::map< core_type, symbol_set_type >          generate_map_type;
        typedef std::set< std::pair< int, core_type > >         propagate_type;
        typedef std::map< core_type, propagate_type >           propagate_map_type;

        int                     no;
        core_set_type           cores;
        core_set_type           kernel;
        item_set_type           items;
        generate_map_type       generate_map;
        propagate_map_type      propagate_map;

        goto_table_type         goto_table;
        action_table_type       action_table;
    };

    typedef std::vector< state >           states_type;

public:
    parsing_table() { first_ = -1; }
    parsing_table( const parsing_table< Token, Traits >& x ) { operator=(x); }
    ~parsing_table() { clear(); }

    self_type& operator=(const self_type& x )
    {
        clear();
        states_ = x.states_;
        rules_ = x.rules_;
        first_ = x.first_;
        return *this;
    }

    int     first_state() const  { return first_; }

    const states_type& states() const { return states_; }
    const rules_type& rules() const { return rules_; }

    void    first_state( int s )
    {
        enunique();
        first_ = s;
    }

    states_type& states()
    {
        enunique();
        return states_;
    }

    int    add_rule( const rule_type& rule )
    {
        enunique();
        rules_.push_back( rule );
        return int( rules_.size() ) - 1;
    }
    state& add_state()
    {
        enunique();
        state s; s.no = int( states_.size() );
        states_.push_back(s);
        return states_.back();
    }

    int rule_index( const rule<Token,Traits>& r ) const
    {
        typename rules_type::const_iterator i = std::find( rules_.begin(), rules_.end(), r );
        if( i == rules_.end() ) {
            return -1;
        }
        return int( i - rules_.begin() );
    }

protected:
    void clear()
    {
        states_.clear();
        rules_.clear();
    }

    void enunique(){}

private:
    states_type     states_;
    rules_type      rules_;
    int             first_;
    
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const parsing_table< Token, Traits >& x )
{
    typedef typename parsing_table< Token, Traits >::states_type    states_type;
    typedef typename parsing_table< Token, Traits >::state          state;
    //typedef typename parsing_table< Token, Traits >::action         action;

    os << "<toplevel=state" << x.first_state() << ">\n";
    for( typename states_type::const_iterator i = x.states().begin() ; i != x.states().end() ; ++i ) {
        os << "<state: " << (*i).no << ">\n";
        for( typename state::action_table_type::const_iterator j = (*i).action_table.begin();
             j != (*i).action_table.end();
             ++j ) {
            os << "  action(";
            if( (*j).first == Traits::eof() ) {
                os << "eof";
            } else {
                os << (*j).first;
            }
            os << ")=";

            switch( (*j).second.type ) {
                case action_shift:
                    os << "shift(" << (*j).second.dest_index << ")\n";
                    break;
                case action_reduce:    
                    os << "reduce( " << x.rules()[ (*j).second.rule_index ] << ")\n";
                    break;
                case action_accept:
                    os << "accept( " << x.rules()[ (*j).second.rule_index ] << ")\n";
                    break;
                case action_error:
                    os << "error\n";
                    break;
            }
        }
        for( typename state::goto_table_type::const_iterator j = (*i).goto_table.begin() ;
             j != (*i).goto_table.end() ;
             ++j ) {
            os << "  goto(" << (*j).first << ")=";
            os << (*j).second << "\n";
        }
    }
    return os;
}

template < class Token, class Traits >
struct null_reporter {
    typedef rule< Token, Traits > rule_type;

    void operator()( const rule_type& x, const rule_type& y )
    {
        // do nothing
    }
};

} // namespace gr

} // namespace zw

#endif // LR_HPP
