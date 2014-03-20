// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// caperの入力ファイルの文法解析テーブル

#include "caper_cpg.hpp"
#include "caper_error.hpp"

typedef cpg::parser::arguments arguments_type;

////////////////////////////////////////////////////////////////
// make_rule
void make_rule_aux(cpg::rule&) {
}

template <class ...T>
void make_rule_aux(cpg::rule& rule, Token token, T... elements) {
    rule << cpg::terminal(token_labels(token), token);
    make_rule_aux(rule, elements...);
}

template <class ...T>
void make_rule_aux(cpg::rule& rule, const std::string& nt_name, T... elements) {
    rule << cpg::nonterminal(nt_name);
    make_rule_aux(rule, elements...);
}

template <class F, class ...T>
void make_rule(cpg::grammar& g, cpg::parser& p, const std::string& rule_name, F f, T... elements) {
    cpg::nonterminal nt(rule_name);
    cpg::rule rule(nt);
    make_rule_aux(rule, elements...);
    g << rule;
    p.set_semantic_action(rule, f);
}


////////////////////////////////////////////////////////////////
// make_cpg_parser
void make_cpg_parser(cpg::parser& p) {
    cpg::grammar g;

    // 全体
    make_rule(
        g, p,
        "Document", 
        [](const arguments_type& args) -> Value {
            return args[0];
        },
        "Sections"
        );
    make_rule(
        g, p, 
        "Sections", 
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<Document>(
                range(args),
                get_node<Declarations>(args[0]),
                get_node<Rules>(args[1]));
            return Value(p);
        },
        "Declarations", "Entries");

    // .宣言セクション
   make_rule(
       g, p,
       "Declarations", 
       [](const arguments_type& args) -> Value {
           std::vector<std::shared_ptr<Declaration>> v;
           v.push_back(get_node<Declaration>(args[0]));
           auto q = std::make_shared<Declarations>(range(args), v);
           return Value(q);
       },
       "Declaration");
    make_rule(
        g, p,
        "Declarations",         
        [](const arguments_type& args) -> Value {
            auto p = get_node<Declarations>(args[0]);
            p->declarations.push_back(
                get_node<Declaration>(args[1]));
            return Value(p);
        },
        "Declarations", "Declaration");

    // ..宣言
    make_rule(
        g, p,
        "Declaration", 
        [](const arguments_type& args) -> Value {
            return Value(args[0]);
        },
        "TokenDecl", token_semicolon);
    make_rule(
        g, p,
        "Declaration", 
        [](const arguments_type& args) -> Value {
            return Value(args[0]);
        },
        "TokenPrefixDecl", token_semicolon);
    make_rule(
        g, p,
        "Declaration", 
        [](const arguments_type& args) -> Value {
            return Value(args[0]);
        },
        "ExternalTokenDecl", token_semicolon);
    make_rule(
        g, p,
        "Declaration", 
        [](const arguments_type& args) -> Value {
            return Value(args[0]);
        },
        "NamespaceDecl", token_semicolon);
    make_rule(
        g, p,
        "Declaration", 
        [](const arguments_type& args) -> Value {
            return Value(args[0]);
        },
        "RecoverDecl", token_semicolon);
    make_rule(
        g, p,
        "Declaration", 
        [](const arguments_type& args) -> Value {
            return Value(args[0]);
        },
        "DontUseSTLDecl", token_semicolon);

    // ..%token宣言
    make_rule(
        g, p,
        "TokenDecl", 
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<TokenDecl>(range(args));
            return Value(p);
        },
        token_directive_token);
    make_rule(
        g, p,
        "TokenDecl", 
        [](const arguments_type& args) -> Value {
            auto p = get_node<TokenDecl>(args[0]);
            p->elements.push_back(get_node<TokenDeclElement>(args[1]));
            return Value(p);
        },
        "TokenDecl", "TokenDeclElement");
    make_rule(
        g, p,
        "TokenDeclElement", 
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<TokenDeclElement>(
                range(args), get_symbol<Identifier>(args[0]));
            return Value(p);
        },
        token_identifier);
    make_rule(
        g, p,
        "TokenDeclElement",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<TokenDeclElement>(
                range(args),
                get_symbol<Identifier>(args[0]),
                get_symbol<TypeTag>(args[1]));
            return Value(p);
        },
        token_identifier, token_typetag);

    // ..%token_prefix宣言
    make_rule(
        g, p,
        "TokenPrefixDecl",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<TokenPrefixDecl>(range(args), "");
            return Value(p);
        },
        token_directive_token_prefix);
    make_rule(
        g, p,
        "TokenPrefixDecl",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<TokenPrefixDecl>(
                range(args), get_symbol<Identifier>(args[1]));
            return Value(p);
        },
        token_directive_token_prefix, token_identifier);

    // ..%external_token宣言
    make_rule(
        g, p,
        "ExternalTokenDecl",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<ExternalTokenDecl>(range(args));
            return Value(p);
        },
        token_directive_external_token);

    // ..%namespace宣言
    make_rule(
        g, p,
        "NamespaceDecl",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<NamespaceDecl>(
                range(args), get_symbol<Identifier>(args[1]));
            return Value(p);
        },
        token_directive_namespace, token_identifier);

    // ..%recover宣言
    make_rule(
        g, p,
        "RecoverDecl",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<RecoverDecl>(
                range(args), get_symbol<Identifier>(args[1]));
            return Value(p);
        },
        token_directive_recover, token_identifier);

    // ..%dont_use_stl宣言
    make_rule(
        g, p,
        "DontUseSTLDecl",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<DontUseSTLDecl>(range(args));
            return Value(p);
        },
        token_directive_dont_use_stl);

    // .文法セクション
    make_rule(
        g, p,
        "Entries",
        [](const arguments_type& args) -> Value {
            std::vector<std::shared_ptr<Rule>> v;
            v.push_back(get_node<Rule>(args[0]));

            auto q = std::make_shared<Rules>(range(args), v);
            return Value(q);
        },
        "Entry");
    make_rule(
        g, p,
        "Entries",
        [](const arguments_type& args) -> Value {
            std::shared_ptr<Rules> p(get_node<Rules>(args[0]));
            p->rules.push_back(get_node<Rule>(args[1]));
            return Value(p);
        },
        "Entries", "Entry");

    // ..文法
    make_rule(
        g, p,
        "Entry",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<Rule>(
                range(args),
                get_symbol<Identifier>(args[0]),
                get_symbol<TypeTag>(args[1]),
                get_node<Choises>(args[2]));
            return Value(p);
        },
        token_identifier, token_typetag, "Derivations", token_semicolon);

    // ...右辺
    make_rule(
        g, p,
        "Derivations",
        [](const arguments_type& args) -> Value {
            std::vector<std::shared_ptr<Choise>> v;
            v.push_back(get_node<Choise>(args[1]));

            auto r = std::make_shared<Choises>(range(args), v);
            return Value(r);
        },
        token_colon, "Derivation");
    make_rule(
        g, p,
        "Derivations",
        [](const arguments_type& args) -> Value {
            std::shared_ptr<Choises> q = get_node<Choises>(args[0]);
            q->choises.push_back(get_node<Choise>(args[2]));
            return Value(q);
        },
        "Derivations", token_pipe, "Derivation");
    make_rule(
        g, p,
        "Derivation",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<Choise>(
                range(args),
                "",
                std::vector<std::shared_ptr<Term>>());
            return Value(p);
        },
        token_lbracket, token_rbracket);
    make_rule(
        g, p,
        "Derivation",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<Choise>(
                range(args),
                get_symbol<Identifier>(args[1]),
                std::vector<std::shared_ptr<Term>>());
            return Value(p);
        },
        token_lbracket, token_identifier, token_rbracket);
    make_rule(
        g, p,
        "Derivation",
        [](const arguments_type& args) -> Value {
            std::shared_ptr<Choise> q = get_node<Choise>(args[0]);
            q->elements.push_back(get_node<Term>(args[1]));

            return Value(q);
        },
        "Derivation", "Term");

    // ...右辺の項目
    make_rule(
        g, p,
        "Term",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<Term>(
                range(args), get_symbol<Identifier>(args[0]), -1);
            return Value(p);
        },
        token_identifier);
    make_rule(
        g, p,
        "Term",
        [](const arguments_type& args) -> Value {
            auto p = std::make_shared<Term>(
                range(args),
                get_symbol<Identifier>(args[0]),
                boost::get<Integer>(args[2].data).n);
            return Value(p);
        },
        token_identifier, token_lparen, token_integer, token_rparen);

    // parsing tableの作成
    cpg::parsing_table table;
    cpg::make_lalr_table(table, g, token_error);

    p.reset(table);
}

////////////////////////////////////////////////////////////////
// collect_informations
void collect_informations(
    GenerateOptions&    options,
    symbol_map_type&    terminal_types,
    symbol_map_type&    nonterminal_types,
    const value_type&   ast) {
    symbol_set_type methods;        // アクション名(重複(不可)のチェック)
    symbol_set_type known;          // 確定識別子名
    symbol_set_type unknown;        // 未確定識別子名

    auto doc = get_node<Document>(ast);

    std::string recover_token = "";

    // 宣言
    std::shared_ptr<Declarations> declarations = doc->declarations;
    for(const auto& x: declarations->declarations) {
        if (auto tokendecl = downcast<TokenDecl>(x)) {
            // %token宣言
            for (const auto& y: tokendecl->elements) {
                //std::cerr << "token: " <<y->name << std::endl;
                if (0 < known.count(y->name)) {
                    throw duplicated_symbol(tokendecl->range.beg,y->name);
                }
                known.insert(y->name);
                terminal_types[y->name] =y->type.s;
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
        if (auto namespacedecl = downcast<NamespaceDecl>(x)) {
            // %namespace宣言
            options.namespace_name = namespacedecl->name;
        }
        if (auto recoverdecl = downcast<RecoverDecl>(x)) {
            if (0 < known.count(recoverdecl->name)) {
                throw duplicated_symbol(
                    recoverdecl->range.beg, recoverdecl->name);
            }
            known.insert(recoverdecl->name);
            terminal_types[recoverdecl->name] = "$error";
            options.recovery = true;
            options.recovery_token = recoverdecl->name;
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
        nonterminal_types[rule->name] = rule->type.s;

        for (const auto& choise: rule->choises->choises) {
            if (methods.find(choise->name) != methods.end()) {
                // 重複
                // TODO: 例外
            }
            methods.insert(choise->name);

            for(const auto& term: choise->elements) {
                unknown.insert(term->name);
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

