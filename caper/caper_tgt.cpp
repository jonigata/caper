// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

#include "caper_tgt.hpp"
#include "caper_error.hpp"

struct sr_conflict_reporter {
    typedef tgt::rule rule_type;

    void operator()(const rule_type& x, const rule_type& y) {
        std::cerr << "shift/reduce conflict: " << x << " vs " << y
                  << std::endl;
    }
};

struct rr_conflict_reporter {
    typedef tgt::rule rule_type;

    void operator()(const rule_type& x, const rule_type& y) {
        std::cerr << "reduce/reduce conflict: " << x << " vs " << y
                  << std::endl;
    }
};

void make_target_parser(
    tgt::parsing_table&             table,
    std::map<std::string, size_t>&  token_id_map,
    action_map_type&                actions,
    const value_type&               ast,
    const symbol_map_type&          terminal_types,
    const symbol_map_type&          nonterminal_types) {

    auto doc = get_node<Document>(ast);

    // 各種データ
    // ...終端記号表(名前→terminal)
    std::unordered_map<std::string, tgt::terminal>      terminals;
    // ...非終端記号表(名前→nonterminal)
    std::unordered_map<std::string, tgt::nonterminal>   nonterminals;

    int error_token = -1;

    // terminalsの作成
    token_id_map["eof"] = 0;
    int id_seed = 1;
    for (const auto& x: terminal_types) {
        if (x.second != "$error") { continue; }
        token_id_map[x.first] = error_token = id_seed;
        terminals[x.first] = tgt::terminal(x.first, id_seed++);
    }
    for (const auto& x: terminal_types) {
        if (x.second == "$error") { continue; }
        token_id_map[x.first] = id_seed;
        terminals[x.first] = tgt::terminal(x.first, id_seed++);
    }

    // nonterminalsの作成
    for (const auto& x: nonterminal_types) {
        nonterminals[x.first] = tgt::nonterminal(x.first);
    }

    // 規則
    std::unique_ptr<tgt::grammar> g;

    std::shared_ptr<Rules> rules = doc->rules;
    for (const auto& rule: rules->rules) {
        const tgt::nonterminal& n = nonterminals[rule->name];
        if (!g) {
            tgt::nonterminal implicit_root("implicit_root");
            tgt::rule r(implicit_root);
            r << n;
            g.reset(new tgt::grammar(r));
        }

        for (const auto& choise: rule->choises->choises) {
            tgt::rule r(n);
            semantic_action sa(choise->name);

            int index = 0;
            int max_index = -1;
            for (const auto& term: choise->elements) {
                if (0 <= term->index) {
                    // セマンティックアクションの引数として用いられる
                    if (sa.args.count(term->index)) {
                        // duplicated
                        throw duplicated_semantic_action_argument(
                            term->range.beg, sa.name, term->index);
                    }

                    // 引数になる場合、型が必要
                    std::string type;
                    {
                        auto l = nonterminal_types.find(term->name);
                        if (l != nonterminal_types.end()) {
                            type = (*l).second;
                        }
                    }
                    {
                        auto l = terminal_types.find(term->name);
                        if (l != terminal_types.end()) {
                            if ((*l).second == "") {
                                throw untyped_terminal(
                                    term->range.beg, term->name);
                            }
                            type =(*l).second;        
                        }
                    }
                    assert(type != "");

                    semantic_action_argument arg(index, type);
                    sa.args[term->index] = arg;
                    if (max_index <term->index) {
                        max_index = term->index;
                    }
                }
                index++;

                {
                    auto l = terminals.find(term->name);
                    if (l != terminals.end()) {
                        r << (*l).second;
                    }
                }
                {
                    auto l = nonterminals.find(term->name);
                    if (l != nonterminals.end()) {
                        r <<(*l).second;
                    }
                }

            }

            // 引数に飛びがあったらエラー
            for (int k = 0 ; k <= max_index ; k++) {
                if (!sa.args.count(k)) {
                    throw skipped_semantic_action_argument(
                        choise->range.beg, sa.name, k);
                }
            }

            // すでに存在している規則だったらエラー
            if (0 <= g->rule_index(r)) {
                throw duplicated_rule(choise->range.beg, r);
            }

            if (!sa.name.empty()) {
                actions[r] = sa;
            }
            *g << r;
        }
    }

    zw::gr::make_lalr_table(
        table,
        *g,
        error_token,
        sr_conflict_reporter(),
        rr_conflict_reporter());
}

