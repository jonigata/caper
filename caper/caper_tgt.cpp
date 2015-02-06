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

////////////////////////////////////////////////////////////////
// collect_informations
void collect_informations(
    GenerateOptions&                options,
    std::map<std::string, Type>&    terminal_types,
    std::map<std::string, Type>&    nonterminal_types,
    const value_type&               ast) {
    std::unordered_set<std::string> known;      // 確定識別子名
    std::unordered_set<std::string> unknown;    // 未確定識別子名

    auto doc = get_node<Document>(ast);

    std::string recover_token = "";

    // 宣言
    for(const auto& x: doc->declarations->declarations) {
        if (auto tokendecl = downcast<TokenDecl>(x)) {
            // %token宣言
            for (const auto& y: tokendecl->elements) {
                //std::cerr << "token: " <<y->name << std::endl;
                if (0 < known.count(y->name)) {
                    throw duplicated_symbol(tokendecl->range.beg,y->name);
                }
                known.insert(y->name);
                terminal_types[y->name] = Type{y->type.s, Extension::None};
            }
        }
        if (auto tokenprefixdecl = downcast<TokenPrefixDecl>(x)) {
            // %token_prefix宣言
            options.token_prefix = tokenprefixdecl->prefix;
        }
        if (auto externaltokendecl = downcast<ExternalTokenDecl>(x)) {
            // %external_token宣言
            options.external_token = true;
        }
        if (auto allow_ebnf = downcast<AllowEBNF>(x)) {
            // %allow_ebnf宣言
            options.allow_ebnf = true;
        }
        if (auto namespacedecl = downcast<NamespaceDecl>(x)) {
            // %namespace宣言
            options.namespace_name = namespacedecl->name;
        }
        if (auto smartptrdecl = downcast<SmartPointerDecl>(x)) {
            // %smart_pointer宣言
            options.smart_pointer_tag = smartptrdecl->tag;
        }
        if (auto recoverdecl = downcast<RecoverDecl>(x)) {
            if (0 < known.count(recoverdecl->name)) {
                throw duplicated_symbol(
                    recoverdecl->range.beg, recoverdecl->name);
            }
            known.insert(recoverdecl->name);
            terminal_types[recoverdecl->name] =
                Type{"$error", Extension::None};
            options.recovery = true;
            options.recovery_token = recoverdecl->name;
        }
        if (auto accessmodifierdecl = downcast<AccessModifierDecl>(x)) {
            options.access_modifier = accessmodifierdecl->modifier;
        }
        if (auto dontusestldecl = downcast<DontUseSTLDecl>(x)) {
            // %dont_use_stl宣言
            options.dont_use_stl = true;
        }
    }

    // 規則
    for (const auto& rule: doc->rules->rules) {
        if (known.find(rule->name) != known.end()) {
            throw duplicated_symbol(rule->range.beg, rule->name);
        }
        known.insert(rule->name);
        nonterminal_types[rule->name] = Type{rule->type.s, Extension::None};

        for (const auto& choise: rule->choises->choises) {
            for(const auto& term: choise->elements) {
                unknown.insert(term->item->name);
            }
        }
    }

    // 未確定識別子が残っていたらエラー
    for (const auto& x: unknown) {
        if (known.count(x) == 0) {
            throw undefined_symbol(-1, x);
        }
    }
}

template <class T, class V>
class find_iterator {
public:
    typedef typename T::const_iterator  original_iterator_type;

public:
    find_iterator(const T& c, const V& v)
        : c_(c), it_(c.find(v)) {
    }

    operator bool() const {
        return it_ != c_.end();
    }

    const typename T::value_type::second_type& operator*() const {
        return (*it_).second;
    }

private:
    const T&                c_;
    original_iterator_type  it_;
    
};

template <class T, class V>
find_iterator<T, V> finder(const T& c, const V& v) {
    return find_iterator<T, V>(c, v);
}

std::string make_extended_name(
    const std::string source_name,
    const std::unordered_map<std::string, tgt::terminal>&     terminals,
    const std::unordered_map<std::string, tgt::nonterminal>&  nonterminals) {

    int n = 0;
    while(true) {
        std::string x = source_name + "_seq" + std::to_string(n++);
        if (terminals.count(x) == 0 && nonterminals.count(x) == 0) {
            return x;
        }            
    }
}

tgt::symbol find_symbol(
    const std::string& name,
    const std::unordered_map<std::string, tgt::terminal>&     terminals,
    const std::unordered_map<std::string, tgt::nonterminal>&  nonterminals) {
    if (auto l = finder(terminals, name)) {
        return *l;
    }
    if (auto l = finder(nonterminals, name)) {
        return *l;
    }
    assert(0);
    return tgt::symbol();
}
    
struct PendingKey {
    std::string element;
    Extension   extension;
    std::string skip;

    bool operator<(const PendingKey& rhs) const {
        return
            std::make_tuple(element, extension, skip) <
            std::make_tuple(rhs.element, rhs.extension, rhs.skip);
    }
};

struct PendingValue {
    std::string extended_name;
};

////////////////////////////////////////////////////////////////
// make_target_rule
void make_target_rule(
    action_map_type&                    actions,
    tgt::grammar&                       g,
    const tgt::nonterminal&             rule_left,
    std::shared_ptr<Choise>             choise,
    const std::map<std::string, Type>&  terminal_types,
    const std::map<std::string, Type>&  nonterminal_types,
    const std::unordered_map<std::string, tgt::terminal>&       terminals,
    const std::unordered_map<std::string, tgt::nonterminal>&    nonterminals,
    std::map<PendingKey, PendingValue>& pendings) {

    tgt::rule r(rule_left);

    std::unordered_map<size_t, SemanticAction::Argument> args;

    int source_index = 0;
    int max_index = -1;
    for (const auto& term: choise->elements) {
        auto item = term->item;

        if (0 <= term->argument_index) {
            // セマンティックアクションの引数として用いられる
            if (0 < args.count(term->argument_index)) {
                // duplicated
                throw duplicated_semantic_action_argument(
                    term->range.beg, choise->action_name, term->argument_index);
            }

            Type type;

            // 引数になる場合、型が必要
            if (auto l = finder(nonterminal_types, item->name)) {
                type.name = (*l).name;
            }
            if (auto l = finder(terminal_types, item->name)) {
                if ((*l).name == "") {
                    throw untyped_terminal(term->range.beg, item->name);
                }
                type.name = (*l).name;
            }
            type.extension = item->extension;
            assert(type.name != "");

            SemanticAction::Argument arg(source_index, type);
            args[term->argument_index] = arg;
            max_index = (std::max)(max_index, term->argument_index);
        }

        if (item->extension != Extension::None) {
            std::string extended_name =
                make_extended_name(item->name, terminals, nonterminals);

            PendingKey k { item->name, item->extension, item->skip };
            PendingValue v { extended_name };
            
            r << tgt::nonterminal(extended_name);
            pendings[k] = v;
        } else {
            r << find_symbol(item->name, terminals, nonterminals);
        }
        source_index++;
    }

    // 引数に飛びがあったらエラー
    for (int k = 0 ; k <= max_index ; k++) {
        if (args.count(k) == 0) {
            throw skipped_semantic_action_argument(
                choise->range.beg, choise->action_name, k);
        }
    }

    // すでに存在している規則だったらエラー
    if (g.exists(r)) {
        throw duplicated_rule(choise->range.beg, r);
    }

    if (!choise->action_name.empty()) {
        SemanticAction sa(choise->action_name, false);
        for (int k = 0 ; k <= max_index ; k++) {
            sa.args.push_back(args[k]);
            sa.source_indices.push_back(args[k].source_index);
        }
        actions[r] = sa;
    }
    g << r;
}

void make_target_parser(
    tgt::parsing_table&             table,
    std::map<std::string, size_t>&  token_id_map,
    action_map_type&                actions,
    const value_type&               ast,
    std::map<std::string, Type>&    terminal_types,
    std::map<std::string, Type>&    nonterminal_types) {

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
        if (x.second.name != "$error") { continue; }
        token_id_map[x.first] = error_token = id_seed;
        terminals[x.first] = tgt::terminal(x.first, id_seed++);
    }
    for (const auto& x: terminal_types) {
        if (x.second.name == "$error") { continue; }
        token_id_map[x.first] = id_seed;
        terminals[x.first] = tgt::terminal(x.first, id_seed++);
    }

    // nonterminalsの作成
    for (const auto& x: nonterminal_types) {
        nonterminals[x.first] = tgt::nonterminal(x.first);
    }

    // pending(あとでまとめてEBNFを展開したルールを作成する
    std::map<PendingKey, PendingValue> pending;

    // 規則
    tgt::grammar g;
    for (const auto& rule: doc->rules->rules) {
        const tgt::nonterminal& rule_left = nonterminals[rule->name];
        if (g.size() == 0) {
            g << (tgt::rule(tgt::nonterminal("$implicit_root")) << rule_left);
        }

        for (const auto& choise: rule->choises->choises) {
            make_target_rule(
                actions,
                g,
                rule_left,
                choise,
                terminal_types,
                nonterminal_types,
                terminals,
                nonterminals,
                pending);
        }
    }

    for (const auto& p: pending) {
        const PendingKey& k = p.first;
        const PendingValue& v = p.second;
        tgt::nonterminal name(v.extended_name);
        nonterminals[v.extended_name] = name;
        nonterminal_types[v.extended_name] = Type{k.element, k.extension};

        tgt::symbol element = find_symbol(k.element, terminals, nonterminals);

        if (k.extension == Extension::Question) {
            tgt::rule list0(name);
            tgt::rule list1(name); list1 << element;
            g << list0;
            g << list1;
            actions[list0] = SemanticAction { "opt_nothing", true };
            actions[list1] = SemanticAction { "opt_just", true };
        } else if (k.extension == Extension::Slash) {
            tgt::symbol skip = find_symbol(k.skip, terminals, nonterminals);

            tgt::rule list0(name); list0 << element;
            tgt::rule list1(name); list1 << name << skip << element;
            g << list0;
            g << list1;
            actions[list0] = SemanticAction { "seq_head", true };
            actions[list1] = SemanticAction { "seq_trail2", true };
        } else {
            tgt::rule list0(name);
            if (k.extension == Extension::Plus) {
                list0 << element;
            }
            tgt::rule list1(name); list1 << name << element;
            g << list0;
            g << list1;
            actions[list0] = SemanticAction { "seq_head", true };
            actions[list1] = SemanticAction { "seq_trail", true };
        }
    }

    zw::gr::make_lalr_table(
        table,
        g,
        error_token,
        sr_conflict_reporter(),
        rr_conflict_reporter());
}
