// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#if !defined(ZW_LALR_HPP)
#define ZW_LALR_HPP

// module: LALR
//   LALR(1)表の作成

#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
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

class disconnected_rule_base : public std::exception {};

template < class Token, class Traits >
class disconnected_rule : public disconnected_rule_base {
public:
        disconnected_rule( std::set< rule< Token, Traits > >& s ) : remains(s){}
        ~disconnected_rule() throw () {}
        const char* what() const throw()
        {
                std::stringstream ss;
                ss << "disconnectd rules: ";
                bool first = true;
                for( typename std::set< rule< Token, Traits > >::const_iterator i = remains.begin() ;
                     i != remains.end() ;
                     ++i ) {
                        if( first ) { first = false; } else { ss << ", "; }
                        ss << *i;
                }
                     
                return ss.str().c_str();
        }

        std::set< rule< Token, Traits > > remains;
};

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
        typedef rule< Token, Traits >           rule_type;
        typedef std::vector< rule_type >        rules_type;
    
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
                typedef std::map< symbol< Token, Traits >, int >        goto_table_type; // index to states_

                int                     no;
                item_set_type           item_set;
                core_set_type           core_set;
                action_table_type       action_table;
                goto_table_type         goto_table;
        };

        typedef std::vector< state >           states_type;

public:
        parsing_table() {}
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
        state* add_state()
        {
                enunique();
                state s; s.no = int( states_.size() );
                states_.push_back(s);
                return &states_.back();
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
        typedef typename parsing_table< Token, Traits >::action         action;

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

//================================================================
// check_reachable
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
                throw disconnected_rule< Token, Traits >( remains );
        }
}

//================================================================
// make_lalr_table
template < class Token, class Traits, class SRReporter, class RRReporter >
void 
make_lalr_table(
        parsing_table< Token, Traits >& table,
        const grammar< Token, Traits >& g, SRReporter srr, RRReporter rrr)
{
        typedef symbol< Token, Traits >         symbol_type; 
        typedef terminal< Token, Traits >       terminal_type; 
        typedef rule< Token, Traits >           rule_type; 
        typedef grammar< Token, Traits >        grammar_type; 
        typedef lr0_collection< Token, Traits > lr0_collection_type; 
        typedef lr1_collection< Token, Traits > lr1_collection_type; 
        typedef symbol_set< Token, Traits >     symbol_set_type; 
        typedef item< Token, Traits >           item_type; 
        typedef item_set< Token, Traits >       item_set_type; 
        typedef core_set< Token, Traits >       core_set_type; 
        typedef parsing_table< Token, Traits >  parsing_table_type;

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

	// p291.アルゴリズム4.11.4.より、
	// sourcesのsecondはvectorである必要はない
	// （どれかひとつでも判ればよいから）が、
	// とりあえずこのままにしておく。

	// sourcesはstate→Iの索引(gotoの計算に必要)
	typedef std::map< int, std::vector< item_set_type > > sources_type;
	sources_type sources;

	// kernelsはI→stateの索引
	// ただし核の部分
	typedef std::map< core_set_type, int > kernels_type;
	kernels_type kernels;

	// 状態の作成および
	// [状態→元の項集合]辞書（gotoの作成に利用）
        {
		// LR(1)集 C={I(0),I(1),...,i(n)} を作る
		lr1_collection_type C;
                make_lr1_collection( C, first, all_symbols, g );

		// LR(1)項集合の中の核を比較し、
		// 同じ核をもつ集合をすべて見つけ出し、
		// それらの集合和をとって、もとの集合と置き換える。
		// これを C'={J(0),J(1),...,J(m)} とする。

		// またI[i]から状態iを作る。

                while( !C.empty() ){
                        typename lr1_collection_type::iterator i = C.begin();
                        typename lr1_collection_type::iterator j = i; ++j;
                        
                        // 新しい状態
                        typename parsing_table_type::state* s = table.add_state();
                        s->no = int( table.states().size() - 1 );
			s->item_set = *i;
			items_to_cores( s->core_set, *i );
			sources[s->no].push_back( *i );

			// 核が同じIを探す
                        while( j != C.end() ) {
				core_set_type J;
                                items_to_cores( J, *j );
                                if( s->core_set == J ){
					// I(j)が等しい

					// 集合和の計算
					merge_sets( s->item_set, *j );

					// 索引に追加
					sources[s->no].push_back( *j );

					// srcから削除
                                        typename lr1_collection_type::iterator jj = j;
                                        ++j;
                                        C.erase( jj );
                                } else {
					// I(j)が等しくない
                                        ++j;
                                }
                        }

                        //std::cerr << s->item_set << std::endl;

			// srcから削除
                        C.erase( i );

			// 索引に追加
			kernels[s->core_set] = s->no;
                }
        }

        // 状態iにおける構文解析動作をJ(i)から作る。
	// もし、その動作表に競合があれば、与えられた文法は
        // LALR(1)でなく、正しい構文解析ルーチンを作り出すことはできない。

        typename parsing_table_type::states_type& states = table.states();

        for( typename parsing_table_type::states_type::iterator i = states.begin() ; i != states.end() ; ++i ) {
                typename parsing_table_type::state* s = &*i;
        
                // p287
                // a) 項[A→α・aβ,b]がJiの要素であり、goto(Ji,a)=Jjであれば、
		// action[i,a]に動作"shift j"を入れる。ここで、aは終端記号でなければならない。
                for( typename item_set_type::const_iterator j = s->item_set.begin() ;
                     j != s->item_set.end() ;
                     ++j ) {
                        const item_type& x = *j;
                        if( x.over() ) { continue; }

                        const symbol_type& a = x.curr();
                        if( !a.is_terminal() ) { continue; }
                        
                        item_set_type gt2; 
                        make_lr1_goto( gt2, sources[s->no][0], a, first, g );

                        core_set_type gt;
                        items_to_cores( gt, gt2 );

                        typename kernels_type::const_iterator next = kernels.find( gt );
                        if( next == kernels.end() ) { continue; }
                        
                        // shift
                        typename parsing_table_type::state::action_table_type::const_iterator k = 
                                s->action_table.find( a.token() );
                        if( k != s->action_table.end() ) {
                                if( (*k).second.type == action_reduce ) {
#ifdef ZW_PARSER_LIVECAST
                                        std::cerr << "shift/reduce conflict" << std::endl;
#endif
                                        srr( x.rule(), table.rules()[ (*k).second.rule_index ] );
                                }
                        }

                        typename parsing_table_type::action action;
                        action.type             = action_shift;
                        action.dest_index       = (*next).second;
                        action.rule_index       = rule_indices[ x.rule() ];
                        s->action_table[ a.token() ] = action;
                }

                // b), c)は同時に行う
                for( typename item_set_type::const_iterator j = s->item_set.begin() ;
                     j != s->item_set.end() ;
                     ++j ) {
                        const item_type& x = (*j);
                        if( !x.over() ) { continue; }

                        // conflict判定ではacceptをreduceの一種とみなす
                        typename parsing_table_type::state::action_table_type::const_iterator k = 
                                s->action_table.find( x.lookahead().token() );

                        bool add_action = true;

                        if( k != s->action_table.end() ) {
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
                                        s->action_table[ x.lookahead().token() ] = a;
                                } else {
                                        // c) 項[S'→S・,$]がJiの要素ならば、action[i,$]に"accept"を入れる。
                                        typename parsing_table_type::action a;
                                        a.type          = action_accept;
                                        a.rule_index    = rule_indices[ g.root_rule() ];
                                        s->action_table[Traits::eof()] = a;
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
                        make_lr1_goto( gt2, sources[s->no][0], *A, first, g );

			core_set_type gt;
                        items_to_cores( gt, gt2 );

			typename kernels_type::const_iterator next = kernels.find(gt);

                        if( next != kernels.end() ){
                                s->goto_table[*A] = (*next).second;
                        }            
                }
        }

        // 初期状態の決定
        for( typename parsing_table_type::states_type::const_iterator i = table.states().begin() ;
             i != table.states().end() ;
             ++i ) {
                // ルート文法かどうかのチェック
                for( typename item_set_type::const_iterator j = (*i).item_set.begin() ;
                     j != (*i).item_set.end() ;
                     ++j ) {
                        if( (*j).cursor() == 0 && (*j).rule() == g.root_rule() ) {
                                table.first_state( (*i).no );
                        }
                }
        }
}

template < class Token, class Traits >
struct null_reporter {
	typedef rule< Token, Traits > rule_type;

        void operator()( const rule_type& x, const rule_type& y )
        {
                // do nothing
        }
};

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
