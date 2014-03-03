// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "caper_tgt.hpp"
#include "caper_error.hpp"
#include "honalee.hpp"
#include <boost/scoped_ptr.hpp>

struct sr_conflict_reporter {
    typedef tgt::rule rule_type;

        void operator()( const rule_type& x, const rule_type& y )
        {
                std::cerr << "shift/reduce conflict: " << x << " vs " << y << std::endl;
        }
};

struct rr_conflict_reporter {
    typedef tgt::rule rule_type;

        void operator()( const rule_type& x, const rule_type& y )
        {
                std::cerr << "reduce/reduce conflict: " << x << " vs " << y << std::endl;
        }
};

void make_target_parser(
        tgt::parsing_table&             table,
        std::map< std::string, size_t >& token_id_map,
        action_map_type&                actions,
        const value_type&               ast,
        const symbol_map_type&          terminal_types,
        const symbol_map_type&          nonterminal_types,
        bool                            algorithm_lr1 )
{
        boost::shared_ptr< Document > doc = get_node< Document >( ast );

        // 各種データ
        std::unordered_map< std::string, tgt::terminal >        terminals;      // 終端記号表   ( 名前→terminal )
        std::unordered_map< std::string, tgt::nonterminal >     nonterminals;   // 非終端記号表 ( 名前→nonterminal )

        // terminalsの作成
        token_id_map["eof"] = 0;
        int id_seed = 1;
        for( symbol_map_type::const_iterator i = terminal_types.begin() ; i != terminal_types.end() ; ++i ) {
                token_id_map[(*i).first] = id_seed;
                terminals[(*i).first] = tgt::terminal( (*i).first, id_seed++ );
        }

        // nonterminalsの作成
        for( symbol_map_type::const_iterator i = nonterminal_types.begin() ; i != nonterminal_types.end() ; ++i ) {
                nonterminals[(*i).first] = tgt::nonterminal( (*i).first );
        }

        // 規則
        boost::scoped_ptr< tgt::grammar > g;

        boost::shared_ptr< Rules > rules = doc->rules;
        for( std::vector< boost::shared_ptr< Rule > >::const_iterator i = rules->rules.begin() ;
             i != rules->rules.end() ;
             ++i ) {
                // rule
                boost::shared_ptr< Rule > rule = *i;

                tgt::nonterminal& n = nonterminals[rule->name];
                if( !g ) {
                        tgt::nonterminal implicit_root( "implicit_root" );
                        tgt::rule r( implicit_root );
                        r << n;
                        g.reset( new tgt::grammar( r ) );
                }

                for( std::vector< boost::shared_ptr< Choise > >::const_iterator j = rule->choises->choises.begin() ;
                     j != rule->choises->choises.end() ;
                     ++j ) {
                        // choise
                        boost::shared_ptr< Choise > choise = *j;

                        tgt::rule r( n );
                        semantic_action sa;
                        sa.name = choise->name;

                        int index = 0;
                        int max_index = -1;
                        for( std::vector< boost::shared_ptr< Term > >::const_iterator k = choise->terms.begin() ;
                             k != choise->terms.end() ;
                             ++k ) {
                                // term
                                boost::shared_ptr< Term > term = *k;

                                if( 0 <= term->index ) {
                                        // セマンティックアクションの引数として用いられる
                                        if( sa.args.find( term->index ) != sa.args.end() ) {
                                                // duplicated
                                                throw duplicated_semantic_action_argument(
                                                        term->range.beg, sa.name, term->index );
                                        }

                                        // 引数になる場合、型が必要
                                        std::string type;
                                        {
                                                symbol_map_type::const_iterator l =
                                                        nonterminal_types.find( term->name );
                                                if( l != nonterminal_types.end() ) {
                                                        type = (*l).second;
                                                }
                                        }
                                        {
                                                symbol_map_type::const_iterator l =
                                                        terminal_types.find( term->name );
                                                if( l != terminal_types.end() ) {
                                                        if( (*l).second == "" ) {
                                                                throw untyped_terminal(
                                                                        term->range.beg, term->name );
                                                        }
                                                        type = (*l).second;        
                                                }
                                        }
                                        assert( type != "" );

                                        semantic_action_argument arg;
                                        arg.src_index   = index;
                                        arg.type        = type;
                                        sa.args[term->index] = arg;
                                        if( max_index < term->index ) { max_index = term->index; }
                                }
                                index++;

                                {
                                        std::unordered_map< std::string, tgt::terminal >::const_iterator l =
                                                terminals.find( term->name );
                                        if( l != terminals.end() ) {
                                                r << (*l).second;
                                        }
                                }
                                {
                                        std::unordered_map< std::string, tgt::nonterminal >::const_iterator l =
                                                nonterminals.find( term->name );
                                        if( l != nonterminals.end() ) {
                                                r << (*l).second;
                                        }
                                }
                        }

                        // 引数に飛びがあったらエラー
                        for( int k = 0 ; k <= max_index ; k++ ) {
                                if( sa.args.find( k ) == sa.args.end() ) {
                                        throw skipped_semantic_action_argument( choise->range.beg, sa.name, k );
                                }
                        }

                        // すでに存在している規則だったらエラー
                        if( 0 <= g->rule_index( r ) ) {
                                throw duplicated_rule( choise->range.beg, r );
                        }

                        if( !sa.name.empty() ) {
                                actions[r] = sa;
                        }
                        *g << r;
                }
        }

        if( algorithm_lr1 ) {
            zw::gr::make_lr1_table( table, *g, sr_conflict_reporter(), rr_conflict_reporter() );
        } else {
            zw::gr::make_lalr_table( table, *g, sr_conflict_reporter(), rr_conflict_reporter() );
        }
    //std::cerr << "\n[target parse table]\n";
        //std::cerr << table;
}

