#ifndef HONALEE_HPP
#define HONALEE_HPP

#include "lr.hpp"
#include <deque>
#include <boost/shared_ptr.hpp>

namespace zw {

namespace gr {

////////////////////////////////////////////////////////////////
// tmp

template <class T>
void print_headline( const char* h, const T& v, const char* p )
{
    std::cerr << h << ":";
    for( typename T::const_iterator i = v.begin() ; i != v.end() ; ++i ) {
        std::cerr << " " << p << (*i)->number;
    }
    std::cerr << std::endl;
}

template < class Set, class Token, class Traits >
void
make_honalee_closure(
    Set&                                            J,
    const first_collection< Token, Traits >&        first,        
    const grammar<Token,Traits>&                    g)
{
    typedef symbol< Token, Traits >                 symbol_type;
    typedef rule< Token, Traits >                   rule_type;
    typedef grammar< Token, Traits >                grammar_type;
    typedef item< Token, Traits >                   item_type;
    typedef symbol_set< Token, Traits >             symbol_set_type;
    typedef item_set< Token, Traits >               item_set_type;
    typedef std::vector< symbol< Token, Traits >  > symbol_vector_type;

    Set I = J;

    int m = 0;
    for(;;){
        Set new_items;  // 挿入する項

        m++;
#if 0
        std::cerr << "* Closure Iteration: " << m << std::endl;
#endif
                
        for( typename Set::const_iterator i = I.begin() ; i != I.end() ; ++i ) {
            // x is [item(A→α・Bβ,a)]
            const item_type& x = (*i);
            if( x.over() ){ continue; }

#if 0
            std::cerr << "  - test: " << x << std::endl;
#endif

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

            int n = 0;
            for( typename grammar_type::const_iterator j = g.begin() ; j != g.end() ; ++j, ++n ) {
                // z is [rule(B→γ)]
                const rule_type& z = (*j);
                if( !(  symbol_type( z.left() ) == y ) ) { continue; }

                // 各lookahead
                for( typename symbol_set_type::const_iterator k = f.begin() ; k != f.end() ; ++k ) {
                    item_type item( n, z, 0, *k );
                    if( J.find( item ) == J.end() ) {
                        new_items.insert( item );
                    }
                }
            }
        }

        if( new_items.empty() ) { break; }
        J.insert( new_items.begin(), new_items.end() );
        I.swap( new_items );
    }
}

template < class T >
void print_list_indirect( std::ostream& os, const T& x )
{
    for( typename T::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
        os << **i;
    }
}

////////////////////////////////////////////////////////////////
// from here
template < class Token, class Traits >
struct state;

template < class Token, class Traits >
struct mark {
    typedef symbol< Token, Traits >                         symbol_type; 
    typedef rule< Token, Traits >                           rule_type;
    typedef item< Token, Traits >                           item_type;
    typedef boost::shared_ptr< state< Token, Traits > >     state_ptr;

    bool kernel;
    enum { action_nil, action_shift, action_reduce } action;

    // when action_reduce
    rule_type       reduce_rule;

    // when action_shift
    symbol_type     shift_symbol;
    state_ptr       goto_state;

    mark() : action( action_nil ) {}
    mark( bool k ) : kernel( k ), action( action_nil ) {}

    void markNil()
    {
        action = action_nil;
        goto_state.reset();
    }
    void markShift( const symbol_type& x, state_ptr y )
    {
        action = action_shift;
        shift_symbol = x;
        goto_state = y;
    }
    void markReduce( const rule_type& y )
    {
        action = action_reduce;
        reduce_rule = y;
        goto_state.reset();
    }
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const mark< Token, Traits >& i )
{
    if( i.kernel ) { os << "* "; } else { os <<  "  "; }
    os << "( ";
    switch( i.action ) {
    case mark< Token, Traits >::action_nil:
        os << "-, -"; break;
    case mark< Token, Traits >::action_shift:
        os << i.shift_symbol << ", " << i.goto_state->number; break;
    case mark< Token, Traits >::action_reduce:
        os << ", " << i.reduce_rule; break;
    }
    os << " )";
    return os;
}

template < class Token, class Traits >
struct state {
    typedef std::map< item< Token, Traits >, mark< Token, Traits > > items_type;

    items_type      items;
    int             number;
    bool            complete;
};

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const state< Token, Traits >& s )
{
    os << s.number << ": " << "{\n";
    for( typename state< Token, Traits >::items_type::const_iterator i = s.items.begin() ;
         i != s.items.end() ;
         ++i ) {
        os << "    [ " << (*i).first << "]" << (*i).second << std::endl;
    }
    os << "}\n";
    return os;
}

template < class Token, class Traits  >
bool equal_core( const state< Token, Traits >& x,
                 const state< Token, Traits >& y )
{
    typedef core_set< Token, Traits >       core_set_type;
    typedef state< Token, Traits >          state_type;

    core_set_type x0;
    core_set_type y0;
    for( typename state_type::items_type::const_iterator i = x.items.begin() ; i != x.items.end() ; ++i ) {
        if( !(*i).second.kernel ) { continue; }
        x0.insert( (*i).first.core() );
    }

    for( typename state_type::items_type::const_iterator i = y.items.begin() ; i != y.items.end() ; ++i ) {
        if( !(*i).second.kernel ) { continue; }
        y0.insert( (*i).first.core() );
    }

    return x0 == y0;
}

template < class Token, class Traits  >
bool rrconflict( const state< Token, Traits >& x,
                 const state< Token, Traits >& y )
{
    typedef core_set< Token, Traits >       core_set_type;
    typedef mark< Token, Traits >           mark_type;
    typedef state< Token, Traits >          state_type;

    for( typename state_type::items_type::const_iterator i = x.items.begin() ;
         i != x.items.end() ;
         ++i ) {
        if( (*i).second.action != mark_type::action_reduce ) { continue; }

        for( typename state_type::items_type::const_iterator j = y.items.begin() ;
             j != y.items.end() ;
             ++j ) {
            if( (*j).second.action != mark_type::action_reduce ) { continue; }

            if( (*j).first.lookahead() == (*i).first.lookahead() &&
                ! ( (*j).second.reduce_rule == (*i).second.reduce_rule ) ) {
                return true;
            }
        }
    }
    return false;
}

template < class Token, class Traits  >
void merge( bool&                               added_items,
            bool&                               added_shift_items,
            state< Token, Traits >&             x,
            const state< Token, Traits >&       y )
{
    typedef core_set< Token, Traits >       core_set_type;
    typedef mark< Token, Traits >           mark_type;
    typedef state< Token, Traits >          state_type;

    added_items = false;
    added_shift_items = false;

    for( typename state_type::items_type::const_iterator i = y.items.begin() ;
         i != y.items.end() ;
         ++i ) {

        if( x.items.find( (*i).first ) != x.items.end() ) { continue; }

        x.items[(*i).first] = (*i).second;
        added_items = true;

        if( !(*i).first.over() ) {
            added_shift_items = true;
        }
    }
}

/*============================================================================
 *
 * make_lr1_table
 *
 * LR(1)表の作成 ( honalee algorithm )
 *
 *==========================================================================*/
template < class Token, class Traits , class SRReporter, class RRReporter >
void
make_lr1_table(
    parsing_table< Token, Traits >& table,
    const grammar< Token, Traits >& g,
    SRReporter                      srr,
    RRReporter                      rrr )
{
    typedef terminal< Token, Traits >       terminal_type; 
    typedef symbol< Token, Traits >         symbol_type; 
    typedef symbol_set< Token, Traits >     symbol_set_type; 
    typedef rule< Token, Traits >           rule_type; 
    typedef grammar< Token, Traits >        grammar_type; 
    typedef item< Token, Traits >           item_type; 
    typedef item_set< Token, Traits >       item_set_type; 
    typedef mark< Token, Traits >           mark_type;
    typedef state< Token, Traits >          state_type;
    typedef boost::shared_ptr< state_type > state_ptr;
    typedef parsing_table< Token, Traits >  parsing_table_type;

    terminal_type eof( "$", Traits::eof() );

    // 記号の収集
    symbol_set_type terminals;    
    symbol_set_type nonterminals;    
    symbol_set_type all_symbols;
    collect_symbols( terminals, nonterminals, all_symbols, g );

    // FIRST, FOLLOWの作成
    first_collection< Token, Traits > first;
    follow_collection< Token, Traits > follow;
    make_first_and_follow( first, follow, terminals, nonterminals, all_symbols, g );

    std::deque< state_ptr >       doneList;
    std::deque< state_ptr >       incList;
    std::deque< state_ptr >       toDoList;
    state_ptr                     comeFrom;
    int setCount = 0;

    state_ptr i0( new state_type );
    i0->items[item_type( 0, g.root_rule(), 0, eof )] = mark_type( true );
    i0->number = -1;
    i0->complete = false;
    toDoList.push_back( i0 );

    int n = 0;

    while( !incList.empty() || !toDoList.empty() ) {
        // phase 1
#if 0
        std::cerr << "<<< Iteration " << (n+1) << ", PHASE 1 >>>\n";
#endif

        comeFrom.reset();
        if( !incList.empty() ) {
            state_ptr s = incList.front(); incList.pop_front();
            comeFrom = s;

#if 0
            std::cerr << "incList.top\n";
            std::cerr << *s;
#endif
                        
            for( typename state_type::items_type::iterator i = s->items.begin() ;
                 i != s->items.end() ;
                 ++i ) {
                const item_type& item = (*i).first;
                const mark_type& mark = (*i).second;
                if( mark.action != mark_type::action_nil )      { continue; }
                if( item.over() )                               { continue; }
                                
                const symbol_type& S = item.curr();
                state_ptr newSet( new state_type );
                newSet->number = -1;
                newSet->complete = false;
                toDoList.push_back( newSet );
                                
                for( typename state_type::items_type::iterator j = s->items.begin() ;
                     j != s->items.end() ;
                     ++j ) {
                    const item_type& shItem = (*j).first;
                    mark_type& shMark = (*j).second;
                    if( shItem.over() )           { continue; }
                    if( !( shItem.curr() == S ) ) { continue; }
                                        
                    newSet->items[
                        item_type(
                            shItem.id(),
                            shItem.rule(),
                            shItem.cursor()+1,
                            shItem.lookahead() ) ] =
                        mark_type( true );
                                        
                    shMark.markShift( S, newSet );
                }

#if 0
                std::cerr << "newSet:\n" << *newSet;
#endif
            }
                        
            s->complete = true;
            doneList.push_back( s );
        }
                
#if 0
        print_headline( "doneList", doneList, "i" );
        print_headline( " incList", incList, "i" );
        print_headline( "toDoList", toDoList, "i" );

        std::cerr << "<doneList>\n";
        print_list_indirect( std::cerr, doneList );
        std::cerr << "<incList>\n";
        print_list_indirect( std::cerr, incList );
        std::cerr << "<toDoList>\n";
        print_list_indirect( std::cerr, toDoList );
#endif

        // phase 2
#if 0
        std::cerr << "<<< Iteration " << (n+1) << ", PHASE 2 >>>\n";
#endif

        while( !toDoList.empty() ) {
            state_ptr s = toDoList.front(); toDoList.pop_front();
                        
            item_set_type c;
            for( typename state_type::items_type::iterator i = s->items.begin() ;
                 i != s->items.end() ;
                 ++i ) {
                c.insert( (*i).first );
            }
            make_honalee_closure( c, first, g );
                        
            for( typename item_set_type::const_iterator i = c.begin() ; i != c.end() ; ++i ) {
                bool kernel = false;

                typename state_type::items_type::const_iterator j = s->items.find( *i );
                if( j != s->items.end() ) {
                    kernel = (*j).second.kernel;
                }

                mark_type m( kernel );
                if( (*i).over() ) {
                    // reduced
                    m.markReduce( (*i).rule() );
                }
                s->items[*i] = m;
            }

            std::vector< state_ptr > unimpcompleted;
            int state = 0;
            typename std::deque< state_ptr >::const_iterator i = doneList.begin();
            typename std::deque< state_ptr >::const_iterator j = incList.begin();
            for(;;){
                state_ptr gSet;
                if( state == 0 ) {
                    if( i == doneList.end() ) {
                        state = 1;
                    } else {
                        gSet = *i;
                        ++i;
                    }
                }
                if( state == 1 ) {
                    if( j == incList.end() ) break;
                    gSet = *j;
                    ++j; 
                }

                if( !equal_core( *s, *gSet ) ) { continue; }
                if( rrconflict( *s, *gSet ) ) {
                    std::cerr << "!!!!!!!!!!!!!!!! reduce/reduce conflict !!!!!!!!!!!!!!!!\n";
                    continue;
                }

#if 0
                std::cerr << "<<<<<<<< marge occured >>>>>>>>\n";
                                
                std::cerr << "merge: \n";
                std::cerr << *s;
                                
                std::cerr << "into: \n";
                std::cerr << *gSet;
#endif

                bool added_items, added_shift_items;
                merge( added_items, added_shift_items, *gSet, *s );
                if( added_items ) {
                    //mergedSetCount++;
                }
                                
#if 0
                std::cerr << "result: \n";
                std::cerr << "added_items: " << added_items << std::endl;
                std::cerr << "added_shift_items: " << added_shift_items << std::endl;

                std::cerr << *gSet;
#endif
                                
                if( comeFrom ) {
                    for( typename state_type::items_type:: iterator i = comeFrom->items.begin() ;
                         i != comeFrom->items.end() ;
                         ++i ) {
                        if( (*i).second.action == mark_type::action_shift &&
                            (*i).second.goto_state == s ) {
                            (*i).second.goto_state = gSet;
                        }
                    }
                }

                if( gSet->complete && added_shift_items ) {
                    for( typename state_type::items_type::iterator i = gSet->items.begin() ;
                         i != gSet->items.end() ;
                         ++i ) {
                        if( (*i).second.action == mark_type::action_shift ) {
                            (*i).second.markNil();
                        }
                    }
                    gSet->complete = false;
                    unimpcompleted.push_back( gSet );
                }

                s.reset();
                break;
            }

            for( typename std::vector< state_ptr >::const_iterator i = unimpcompleted.begin();
                 i != unimpcompleted.end() ;
                 ++i ) {
                doneList.erase( std::find( doneList.begin(), doneList.end(), *i ) );
                incList.push_back( *i );
            }
                        
            if( s ) {
                s->number = setCount;
                setCount++;
                incList.push_back( s );
            }
        }

#if 0
        print_headline( "doneList", doneList, "i" );
        print_headline( " incList", incList, "i" );
        print_headline( "toDoList", toDoList, "i" );

        std::cerr << "<doneList>\n";
        print_list_indirect( std::cerr, doneList );
        std::cerr << "<incList>\n";
        print_list_indirect( std::cerr, incList );
        std::cerr << "<toDoList>\n";
        print_list_indirect( std::cerr, toDoList );
        //std::cerr << "<comeFrom>\n";
        //std::cerr << comeFrom->number << std::endl;
#endif

        ++n;
        if( n == 15 ) exit(0);
    }

#if 0
    std::cerr << "<doneList>\n";
    print_list_indirect( std::cerr, doneList );
#endif

    // テーブルの作成

    // 表の作成
    // ルールのコピー/インデックスの作成
    std::map< rule_type, int > rule_indices;
    {
        for( typename grammar_type::const_iterator j = g.begin() ; j != g.end() ; ++j ) {
            rule_indices[ *j ] = table.add_rule( *j );
        }
    }

    // 番号付け替え
    int m = 0;
    for( typename std::deque< state_ptr >::iterator i = doneList.begin() ; i != doneList.end() ; ++i ) {
        (*i)->number = m; m++;             
    }

    table.first_state( 0 );

    // 移植
    for( typename std::deque< state_ptr >::const_iterator i = doneList.begin() ; i != doneList.end() ; ++i ) {
        const state_type&                       ss = *(*i);             // honalee state
        typename parsing_table_type::state&     ds = table.add_state(); // table state

        ds.no = ss.number;
        for( typename state_type::items_type::const_iterator j = ss.items.begin() ;
             j != ss.items.end() ;
             ++j ) {
            const item_type& item = (*j).first;
            const mark_type& mark = (*j).second;

            switch( mark.action ) {
            case mark_type::action_shift: {
                if( mark.shift_symbol.is_terminal() ) {
                    typename parsing_table_type::action action;
                    action.type             = action_shift;
                    action.dest_index       = mark.goto_state->number;
                    action.rule_index       = rule_indices[ item.rule() ];
                    ds.action_table[ mark.shift_symbol.token() ] = action;
                } else {
                    ds.goto_table[ mark.shift_symbol ] = mark.goto_state->number;
                }
                break;
            }
            case mark_type::action_reduce: 
                if( item.rule() == g.root_rule() ) {
                    typename parsing_table_type::action a;
                    a.type          = action_accept;
                    a.dest_index    = 0xdeadbeaf;
                    a.rule_index    = rule_indices[ item.rule() ];
                    ds.action_table[ Traits::eof() ] = a;
                } else {
                    typename parsing_table_type::action a;
                    a.type          = action_reduce;
                    a.dest_index    = 0xdeadbeaf;
                    a.rule_index    = rule_indices[ item.rule() ];
                    ds.action_table[ item.lookahead().token() ] = a;
                }
                break;
            case mark_type::action_nil:;
            }                        
        }
    }

}

template < class Token, class Traits >
void
make_lr1_table(
    parsing_table< Token, Traits >& table,
    const grammar< Token, Traits >& g )
{
    make_lr1_table( table, g, null_reporter< Token, Traits >(), null_reporter< Token, Traits >() );
}

} // namespace gr

} // namespace zw

#endif // HONALEE_HPP
