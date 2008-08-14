// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#if !defined(ZW_FASTLALR_HPP)
#define ZW_FASTLALR_HPP

// module: LALR
//   LALR(1)表の作成(高速版)

#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include "grammar.hpp"
#include "lr.hpp"

//#define ZW_PARSER_LIVECAST

namespace zw {

namespace gr {

/*============================================================================
 *
 * 例外
 *
 * 
 *
 *==========================================================================*/

class syntax_error : public std::exception {};

class unconnected_rule_base : public std::exception {};

template < class Token, class Traits >
class unconnected_rule : public unconnected_rule_base {
public:
        unconnected_rule( std::set< rule< Token, Traits > >& remains )
        {
                std::stringstream ss;
                ss << "unconnected rules: ";
                bool first = true;
                for( typename std::set< rule< Token, Traits > >::const_iterator i = remains.begin() ;
                     i != remains.end() ;
                     ++i ) {
                        if( first ) { first = false; } else { ss << ", "; }
                        ss << *i;
                }
                message = ss.str();
        }
        ~unconnected_rule() throw () {}
        const char* what() const throw() { return message.c_str(); }

        std::string message;
};

/*============================================================================
 *
 * check_reachable
 *
 * 接続チェック
 *
 *==========================================================================*/
template < class Token, class Traits >
void check_reachable( const grammar< Token, Traits >& g )
{
        typedef symbol< Token, Traits >         symbol_type; 
        typedef rule< Token, Traits >           rule_type; 
        typedef grammar< Token, Traits >        grammar_type; 
        typedef symbol_set< Token, Traits >     symbol_set_type;
        
        symbol_set_type symbols;

        typedef std::set< rule_type > remains_type;
        remains_type remains;
        for( typename grammar_type::const_iterator i = g.begin() ; i != g.end() ; ++i ) {
                remains.insert( *i );
        }
        symbols.insert( symbol_type( g.root_rule().left() ) );

        bool iterate = true;
        while( iterate ) {
                iterate = false;

                typedef std::vector< typename std::set< rule_type >::iterator > erased_type;
                erased_type erased;
                
                for( typename remains_type::iterator i = remains.begin() ; i != remains.end() ; ++i ) {
                        const rule_type& rule = *i;

                        if( symbols.find( symbol_type( rule.left() ) ) != symbols.end() ) {
                                symbols.insert( rule.right().begin(), rule.right().end() );
                                erased.push_back( i );
                                iterate = true;
                        }
                }
                        
                for( typename erased_type::const_iterator j = erased.begin() ; j != erased.end(); ++j ) {
                        remains.erase( *j );
                }
        }

        if( !remains.empty() ) {
                throw unconnected_rule< Token, Traits >( remains );
        }
}

/*============================================================================
 *
 * make_lalr_table
 *
 * LALR(1)表の作成
 *
 *==========================================================================*/
template < class Token, class Traits , class SRReporter, class RRReporter >
void 
make_lalr_table(
        parsing_table< Token, Traits >& table,
        const grammar< Token, Traits >& g,
        SRReporter                      srr,
        RRReporter                      rrr )
{
        typedef symbol< Token, Traits >                         symbol_type; 
        typedef terminal< Token, Traits >                       terminal_type; 
        typedef rule< Token, Traits >                           rule_type; 
        typedef grammar< Token, Traits >                        grammar_type; 
        typedef lr0_collection< Token, Traits >                 lr0_collection_type; 
        typedef symbol_set< Token, Traits >                     symbol_set_type; 
        typedef core< Token, Traits >                           core_type; 
        typedef item< Token, Traits >                           item_type; 
        typedef item_set< Token, Traits >                       item_set_type; 
        typedef core_set< Token, Traits >                       core_set_type; 
        typedef parsing_table< Token, Traits >                  parsing_table_type;
        typedef typename parsing_table_type::state              state_type;
        typedef typename parsing_table_type::states_type        states_type;
        typedef typename state_type::goto_table_type            goto_table_type;
        typedef typename state_type::generate_map_type          generate_map_type;
        typedef typename state_type::propagate_map_type         propagate_map_type;
        typedef typename state_type::propagate_type             propagate_type;

        // 記号の収集
        symbol_set_type terminals;    
        symbol_set_type nonterminals;    
        symbol_set_type all_symbols;
        collect_symbols( terminals, nonterminals, all_symbols, g );

        // 接続チェック
        check_reachable( g );

        // FIRST, FOLLOWの作成
        first_collection< Token, Traits > first;
        follow_collection< Token, Traits > follow;
        make_first_and_follow( first, follow, terminals, nonterminals, all_symbols, g );

	// 表の作成
        // ルールのコピー/インデックスの作成
        std::map< rule_type, int > rule_indices;
        {
                for( typename grammar_type::const_iterator j = g.begin() ; j != g.end() ; ++j ) {
                        rule_indices[ *j ] = table.add_rule( *j );
                }
        }

        // p.271

        // 1. Construct the kernels of the sets of LR(0) items for G.
        // If space is not a premium, the simplest way is to construct
        // the LR(0) sets of items, as in Section 4.6.2, and then
        // remove the nonkernel items.  If space is serverely
        // constrained, we may wish instead to store only the kernel
        // items for each set, and compute GOTO for a set of items I
        // by first computing the closure of I.

        // simplest wayのほう
        lr0_collection_type I;
        make_lr0_collection( I, g );

        //std::cerr << I ;

        // states
        states_type& states = table.states();

	// kernels: I(kernel)→stateの索引
	typedef std::map< core_set_type, int > kernels_type;
        kernels_type kernels;

        // state, kernelsを作る
        core_type root_core( 0, g.root_rule(), 0 );
        for( typename lr0_collection_type::iterator i = I.begin() ; i != I.end() ; ++i ) {
                // 新しい状態
                state_type& s = table.add_state();
                s.no = int( table.states().size() - 1 );
                s.cores = *i;
                choose_kernel( s.kernel, s.cores, g );
                kernels[s.kernel] = s.no;

                if( s.kernel.find( root_core ) != s.kernel.end() ) {
                        table.first_state( s.no );
                        s.generate_map[root_core].insert( terminal_type( "$", Traits::eof() ) );
                }
        }

        // goto_tableを作る
        for( typename states_type::iterator i = states.begin() ; i != states.end() ; ++i ) {
                state_type& s = *i;
                
                for( typename symbol_set_type::const_iterator j = all_symbols.begin() ;
                     j != all_symbols.end() ;
                     ++j ) {
                        core_set_type gotoIX;
                        make_lr0_goto( gotoIX, s.cores, *j, g );
                        if( gotoIX.empty() ) { continue; }

                        core_set_type gotoIX2;
                        choose_kernel( gotoIX2, gotoIX, g );
                        s.goto_table[*j] = kernels[gotoIX2];
                }
        }

        // 2. Apply Algorithm 4.62 to the kernel of each set of LR(0)
        // items and grammar symbol X to determine which lookaheads
        // are spontaneously generated for kernel items in GOTO( I, X
        // ), and from which items in I lookaheads are propagated to
        // kernel items in GOTO( I, X ) .
        
        // 3. Initialize a table that gives, for each kernel item in
        // each set of items, the associated lookaheads.  Initially,
        // each item has associated with it only those lookaheads that
        // we determined in step(2) were generated spontaneously.

        // determine lookahead p.296
        terminal_type dummy( "#", Token(-1) );

        for( typename states_type::iterator i = states.begin() ; i != states.end() ; ++i ) {
                state_type& s = *i;

                for( typename core_set_type::const_iterator k = s.kernel.begin() ; k != s.kernel.end() ; ++k ) {
                        item_set_type J; J.insert( item_type( *k, dummy ) );
                        make_lr1_closure( J, first, g );

                        for( typename item_set_type::const_iterator j = J.begin() ; j != J.end() ; ++j ) {
                                if( (*j).over() ) { continue; }

                                const symbol_type& X = (*j).curr();

                                int goto_state = s.goto_table[X];
                                const core_set_type& gotoIX = states[goto_state].kernel;

                                for( typename core_set_type::const_iterator l = gotoIX.begin() ;
                                     l != gotoIX.end() ;
                                     ++l ) {

                                        if( !( (*l).rule() == (*j).rule() ) ) { continue; }
                                        if( (*l).cursor() != (*j).cursor() + 1 ) { continue; }

                                        if( (*j).lookahead() == symbol_type( dummy ) ) {
                                                // 先読み伝播
#if 0
                                                std::cerr << "  propagate from " << (*k)
                                                          << " to " << (*l) << std::endl;
#endif
                                                s.propagate_map[*k].insert( std::make_pair( goto_state, *l ) );
                                        } else {
                                                // 内部生成
#if 0
                                                std::cerr << "  create " << (*l).rule() << ": " << X
                                                          << std::endl;
#endif
                                                states[goto_state].generate_map[*l].insert( (*j).lookahead() );
                                        }
                                }                                
                        }
                }
        }        
        
        // 4. Make repeated passes over the kernel items in all sets.
        // When we visit an item /i/, we look up the kernel items to
        // which /i/ propagates its lookaheads, using information
        // tabulated in step (2).  The current set of lookaheads for
        // /i/ is added to those already associated with each of the
        // items to which items until no more new lookaheads are
        // propagated.

#if 0
        typedef std::map< core_set_type, std::map< core_type, symbol_set_type > >       generate_map_type;
        typedef std::map< core_set_type, std::map< core_type, std::set< std::pair< core_set_type, core_type > > > >
                propagate_map_type;
#endif

        int z = 0;

        bool iterate = true;
        while( iterate ) {
                iterate = false;

                z++;
                //std::cerr << "ITERATE " << z << std::endl;

                // iterationの境があいまいだが単調増加なので問題ない

                for( typename states_type::const_iterator i = states.begin() ; i != states.end() ; ++i ) {
                        const core_set_type& Ke = (*i).kernel;

                        //std::cerr << Ke << std::endl;

                        for( typename core_set_type::const_iterator j = Ke.begin(); j != Ke.end() ; ++j ) {
                                typename generate_map_type::const_iterator f0 =
                                        (*i).generate_map.find( *j );
                                if( f0 == (*i).generate_map.end() ) { continue; }
                                typename propagate_map_type::const_iterator f1 = 
                                        (*i).propagate_map.find( *j );
                                if( f1 == (*i).propagate_map.end() ) { continue; }

                                const symbol_set_type& sg = (*f0).second;
                                const propagate_type& propagate = (*f1).second;

                                for( typename propagate_type::const_iterator k = propagate.begin() ;
                                     k != propagate.end() ;
                                     ++k ) {
                                        symbol_set_type& dg = states[(*k).first].generate_map[(*k).second];
                                        size_t n = dg.size();
                                        dg.insert( sg.begin(), sg.end() );
                                        if( dg.size() != n ) { iterate = true; }
                                }
                        }
                }
        }

        // kernel lr0 collectionに先読みを与えてclosureを作る
        for( typename states_type::iterator i = states.begin() ; i != states.end() ; ++i ) {
                const core_set_type& K = (*i).kernel;

                for( typename core_set_type::const_iterator k = K.begin() ; k != K.end() ; ++k ) {
                        const core_type& x = *k;
                        const symbol_set_type& syms = (*i).generate_map[x];
                        for( typename symbol_set_type::const_iterator s = syms.begin() ; s != syms.end() ; ++s ) {
                                (*i).items.insert( item_type( x, *s ) );
                        }
                }

                //std::cerr << "old: " << (*i).items << std::endl;
                make_lr1_closure( (*i).items, first, g );
                //std::cerr << (*i).no << ": " << (*i).items << std::endl;
        }

        // 状態iにおける構文解析動作をJ(i)から作る。
	// もし、その動作表に競合があれば、与えられた文法は
        // LALR(1)でなく、正しい構文解析ルーチンを作り出すことはできない。

        for( typename parsing_table_type::states_type::iterator i = states.begin() ; i != states.end() ; ++i ) {
                state_type& s = *i;
        
                // p287
                // a) 項[A→α・aβ,b]がJ(i)の要素であり、goto(J(i),a)=J(j)であれば、
		// action[i,a]に動作"shift j"を入れる。ここで、aは終端記号でなければならない。
                for( typename item_set_type::const_iterator j = s.items.begin() ; j != s.items.end() ; ++j ) {
                        const item_type& x = *j;
                        if( x.over() ) { continue; }

                        const symbol_type& a = x.curr();
                        if( !a.is_terminal() ) { continue; }
                        
                        typename goto_table_type::iterator gt = s.goto_table.find( a );
                        if( gt == s.goto_table.end() ) { continue; }

                        int next = (*gt).second;
                        
                        // terminalはgoto_tableから削除
                        s.goto_table.erase( gt );

                        // shift
                        typename parsing_table_type::state::action_table_type::const_iterator k = 
                                s.action_table.find( a.token() );
                        if( k != s.action_table.end() ) {
                                if( (*k).second.type == action_reduce ) {
#ifdef ZW_PARSER_LIVECAST
                                        std::cerr << "shift/reduce conflict" << std::endl;
#endif
                                        srr( x.rule(), table.rules()[ (*k).second.rule_index ] );
                                }
                        }

                        typename parsing_table_type::action action;
                        action.type             = action_shift;
                        action.dest_index       = next;
                        action.rule_index       = rule_indices[ x.rule() ];
                        s.action_table[ a.token() ] = action;
                }

                // b), c)は同時に行う
                for( typename item_set_type::const_iterator j = s.items.begin() ; j != s.items.end() ; ++j ) {
                        const item_type& x = (*j);
                        if( !x.over() ) { continue; }

                        // conflict判定ではacceptをreduceの一種とみなす
                        typename parsing_table_type::state::action_table_type::const_iterator k = 
                                s.action_table.find( x.lookahead().token() );

                        bool add_action = true;

                        if( k != s.action_table.end() ) {
                                const rule_type& krule = table.rules()[ (*k).second.rule_index ];
                                if( (*k).second.type == action_shift ) {
#ifdef ZW_PARSER_LIVECAST
                                        std::cerr << "shift/reduce conflict" << std::endl;
#endif
                                        srr( krule, x.rule() );
                                        add_action = false; // shiftを優先
                                }
                                if( (*k).second.type == action_reduce &&
                                    !( krule == x.rule() ) ) {
#ifdef ZW_PARSER_LIVECAST
                                        std::cerr << "reduce/reduce conflict" << std::endl;
#endif
                                        rrr( krule, x.rule() );
                                        add_action =
                                                table.rule_index( x.rule() ) <
                                                (*k).second.rule_index; 
                                }
                        }

                        if( add_action ) {
                                if( !( x.rule() == g.root_rule() ) ) {
                                        // b) 項[A→α・,a]がJiの要素であり、A≠Sならば、action[i,a]に
                                        // "reduce A→α"を入れる。

                                        typename parsing_table_type::action a;
                                        a.type          = action_reduce;
                                        a.rule_index    = rule_indices[ x.rule() ];
                                        s.action_table[ x.lookahead().token() ] = a;
                                } else {
                                        // c) 項[S'→S・,$]がJiの要素ならば、action[i,$]に"accept"を入れる。
                                        typename parsing_table_type::action a;
                                        a.type          = action_accept;
                                        a.rule_index    = rule_indices[ g.root_rule() ];
                                        s.action_table[Traits::eof()] = a;
                                }
                        }
                }

                // 状態iに対する行き先関数は、
                // 次の規則をすべての非終端記号Aに適用して作成する。
                // すなわち、goto(I(i),A)=I(j)であれば、goto[i,A]=jとする。
                for( typename symbol_set_type::const_iterator A = nonterminals.begin() ;
                     A != nonterminals.end() ;
                     ++A ) {

			item_set_type gt2;
                        make_lr1_goto( gt2, s.items, *A, first, g );

			core_set_type gt;
                        items_to_cores( gt, gt2 );

			typename kernels_type::const_iterator next = kernels.find(gt);
                        if( next == kernels.end() ) { continue; }
                        s.goto_table[*A] = (*next).second;
                }
        }

        // 初期状態の決定
        for( typename parsing_table_type::states_type::const_iterator i = table.states().begin() ;
             i != table.states().end() ;
             ++i ) {
                // ルート文法かどうかのチェック
                for( typename item_set_type::const_iterator j = (*i).items.begin() ; j != (*i).items.end() ; ++j ) {
                        if( (*j).cursor() == 0 && (*j).rule() == g.root_rule() ) {
                                table.first_state( (*i).no );
                        }
                }
        }
}

template < class Token, class Traits >
void
make_lalr_table(
        parsing_table< Token, Traits >& table,
        const grammar< Token, Traits >& g )
{
        make_lalr_table( table, g, null_reporter< Token, Traits >(), null_reporter< Token, Traits >() );
}

/*============================================================================
 *
 * class parser
 *
 * LALR構文解析エンジン
 *
 *==========================================================================*/

////////////////////////////////////////////////////////////////
// semantic_action --- 軽量化boost::function
template < class Value, class Arguments >
class semantic_action {
private:
        class semantic_action_imp {
        public:
                semantic_action_imp() : rc_count_( 0 ) {}
                virtual ~semantic_action_imp() {}

                virtual void invoke( Value& v, const Arguments& ) = 0;

                void addref() { rc_count_++; }
                void release() { rc_count_--; if( !rc_count_ ) { delete this; } }
                int rccount() { return rc_count_; }
        private:
                int rc_count_;
        };

        template < class F >
        class concrete_semantic_action_imp : public semantic_action_imp {
        public:
                concrete_semantic_action_imp( F f ) : f_( f ) {}
                ~concrete_semantic_action_imp(){}
                void invoke( Value& v, const Arguments& a ) { v = f_( a ); }
        private:
                F f_;
        };
public:
        semantic_action() {}
        semantic_action( const semantic_action& x ) : imp(x.imp) {}

        template < class F >
        semantic_action( F f ) : imp( new concrete_semantic_action_imp<F>( f ) ) {}

        ~semantic_action() {}

        void operator()( Value& v, const Arguments& args ) const
        {
                imp->invoke( v, args );
        }

        semantic_action& operator=( const semantic_action& x )
        {
                imp = x.imp;
                return *this;
        }

        bool empty() const { return imp.empty(); }

private:
        intrusive_rc_ptr<semantic_action_imp> imp;
        
};

template < class Table, class Value >
class parser {
public:
        typedef Table                                   table_type;
        typedef Value                                   value_type;
        typedef typename table_type::token_type         token_type;
        typedef typename table_type::traits_type        traits_type;
        typedef typename table_type::state              state_type;
        typedef typename table_type::action             action_type;
        typedef typename table_type::rule_type          rule_type;

private:
        struct stack_item {
                int             state;
                value_type      value;

                stack_item( int s, const value_type& v )
                        : state( s ), value( v ) {}
        };

        std::vector<stack_item> stack_;

public:
        class arguments {
        public:
                arguments( typename std::vector< stack_item >::const_iterator b,
                           typename std::vector< stack_item >::const_iterator e )
                        : b_(b), e_(e) {}

                const value_type& operator[]( size_t n ) const
                {
                        assert( b_ + n < e_ );
                        return (*( b_ + n )).value;
                }

                size_t size() const { return e_ - b_; }
        private:
                typename std::vector< stack_item >::const_iterator b_;
                typename std::vector< stack_item >::const_iterator e_;
        };

        typedef semantic_action< Value, arguments > semantic_action_type;
        typedef std::vector< semantic_action_type > semantic_actions_type;
    
public:
        parser() {}
        parser( const table_type& x ) { reset( x ); }

        void reset( const table_type& x )
        {
                stack_.clear();
                semantic_actions_.clear();

                table_ = x;
                push_stack( table_.first_state(), value_type() );
                semantic_actions_.resize( x.rules().size() );
        }

        template < class F >
        void set_semantic_action( const rule_type& rule, F f )
        {
                for( size_t i = 0 ; i < table_.rules().size() ; i++ ) {
                        if( table_.rules()[i] == rule ) {
                                semantic_actions_[i] = semantic_action_type( f );
                        }
                }
        }

        bool push( const token_type& x, const value_type& v )
        {
                bool ate = false;

                const typename table_type::rules_type& rules = table_.rules();

                while( !ate ) {
                        const state_type* s = stack_top();
#ifdef ZW_PARSER_LIVECAST
                        std::cerr << "state [" << s->no << "]\n";
#endif
                        typename state_type::action_table_type::const_iterator i = s->action_table.find(x);
                        if( i == s->action_table.end() ) { throw syntax_error(); }
                                
                        const action_type* a = &((*i).second);
                        switch( a->type ) {
                        case action_shift:
#ifdef ZW_PARSER_LIVECAST
                                std::cerr << "shift(" << x << ")\n";
#endif
                                push_stack( a->dest_index, v  );
                                ate = true;
                                break;
                        case action_reduce: {
                                const rule_type& rule = rules[ a->rule_index ];
#ifdef ZW_PARSER_LIVECAST
                                std::cerr << "reduce(" << rule << ")\n";
#endif
                                value_type v;
                                run_semantic_action( v, a->rule_index );
                                pop_stack( rule.right().size() );
                                s = stack_top();
                                {
                                        typename state_type::goto_table_type::const_iterator i =
                                                s->goto_table.find( rule.left() );
                                        assert( i != s->goto_table.end() );
                                        push_stack( (*i).second, v  );
                                }
                                break;
                        }
                        case action_accept: {
#ifdef ZW_PARSER_LIVECAST
                                std::cerr << "accept(" << rules[ a->rule_index ] << ")\n";
#endif
                                run_semantic_action( accept_value_, a->rule_index );
                                return true;
                        }
                        case action_error:
                        default:
                                throw syntax_error();
                        }
                }
                return false;
        }

        const value_type& accept_value() { return accept_value_; } // pushがtrueを返した時に有効になる
        
private:
        void run_semantic_action( value_type& v, int rule_index )
        {
                const rule_type& rule = table_.rules()[rule_index];
                
                if( !semantic_actions_[rule_index].empty() ) {
                        semantic_actions_[rule_index](
                                v,
                                arguments(
                                        stack_.end() - rule.right().size(),
                                        stack_.end() ) );
                }
        }

        void push_stack( int stack_index, const Value& value )
        {
                stack_.push_back( stack_item( stack_index, value ) );
        }

        void pop_stack( size_t n )
        {
                stack_.erase( stack_.end() - n, stack_.end() );
        }

        typename table_type::state* stack_top()
        {
                return &table_.states()[ stack_.back().state ];
        }

private:
        Table                   table_;
        semantic_actions_type   semantic_actions_;
        value_type              accept_value_;

};

template < class Token, class Traits, class Value >
struct package {
        typedef zw::gr::rule< Token, Traits >              rule;
        typedef zw::gr::epsilon< Token, Traits >           epsilon;
        typedef zw::gr::nonterminal< Token, Traits >       nonterminal;
        typedef zw::gr::terminal< Token, Traits >          terminal;
        typedef zw::gr::grammar< Token, Traits >           grammar;
        typedef zw::gr::parsing_table< Token, Traits >     parsing_table;
        typedef zw::gr::parser< parsing_table, Value >     parser;

        static void make_lalr_table( parsing_table& table, const grammar& g )
        {
                zw::gr::make_lalr_table( table, g );
        }

        template < class Reporter > static
        void make_lalr_table( parsing_table& table, const grammar& g, Reporter srr, Reporter rrr )
        {
                zw::gr::make_lalr_table( table, g, srr, rrr );
        }
};

} // namespace gr

} // namespace zw

#endif
