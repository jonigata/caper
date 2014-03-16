// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// caperの入力ファイルの文法解析テーブル

#include "caper_cpg.hpp"
#include "caper_error.hpp"

typedef cpg::parser::arguments arguments_type;

////////////////////////////////////////////////////////////////
// cpg semantic actions
// 全体
struct document_action { // sections;
    value_type operator()(const arguments_type args) const {
        return args[0];
    }
};
struct sections_action { // declarations << rules;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<Document>(
            range(args),
            get_node<Declarations>(args[0]),
            get_node<Rules>(args[1]));
        return Value(p);
    }
};

// .宣言セクション
struct declarations0_action { // declaration;
    value_type operator()(const arguments_type args) const {
        std::vector<std::shared_ptr<Declaration>> v;
        v.push_back(get_node<Declaration>(args[0]));
        auto q = std::make_shared<Declarations>(range(args), v);
        return Value(q);
    }
};
struct declarations1_action { // declarations << declaration;
    value_type operator()(const arguments_type args) const {
        std::shared_ptr<Declarations> p = get_node<Declarations>(args[0]);
        p->declarations.push_back(get_node<Declaration>(args[1]));
        return Value(p);
    }
};

// ..宣言
struct declaration0_action { // token_decl << semicolon;
    value_type operator()(const arguments_type args) const {
        return Value(args[0]);
    }
};
struct declaration1_action { // token_prefix_decl << semicolon;
    value_type operator()(const arguments_type args) const {
        return Value(args[0]);
    }
};
struct declaration2_action { // external_token_decl << semicolon;
    value_type operator()(const arguments_type args) const {
        return Value(args[0]);
    }
};
struct declaration3_action { // namespace_decl << semicolon;
    value_type operator()(const arguments_type args) const {
        return Value(args[0]);
    }
};
struct declaration4_action { // dont_use_stl_decl << semicolon;
    value_type operator()(const arguments_type args) const {
        return Value(args[0]);
    }
};
struct declaration5_action { // access_modifier_decl << semicolon;
    value_type operator()(const arguments_type args) const {
        return Value(args[0]);
    }
};

// ..%token宣言
struct token_decl0_action { // directive_token;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<TokenDecl>(range(args));
        return Value(p);
    }
};
struct token_decl1_action { // token_decl << token_decl_element;
    value_type operator()(const arguments_type args) const {
        std::shared_ptr<TokenDecl> p = get_node<TokenDecl>(args[0]);
        p->elements.push_back(get_node<TokenDeclElement>(args[1]));
        return Value(p);
    }
};
struct token_decl_element0_action { // identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<TokenDeclElement>(
            range(args), get_symbol<Identifier>(args[0]));
        return Value(p);
    }
};
struct token_decl_element1_action { // identifier << typetag;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<TokenDeclElement>(
            range(args),
            get_symbol<Identifier>(args[0]),
            get_symbol<TypeTag>(args[1]));
        return Value(p);
    }
};

// ..%token_prefix宣言
struct token_pfx_decl0_action { // directive_token_prefix << identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<TokenPrefixDecl>(range(args), "");
        return Value(p);
    }
};
struct token_pfx_decl1_action { // directive_token_prefix << identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<TokenPrefixDecl>(
            range(args), get_symbol<Identifier>(args[1]));
        return Value(p);
    }
};

// ..%external_token宣言
struct external_token_decl_action { // directive_namespace << identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<ExternalTokenDecl>(range(args));
        return Value(p);
    }
};

// ..%access_modifier宣言
struct access_modifier_decl_action { // directive_access_modifier << identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<AccessModifierDecl>(
            range(args), get_symbol<Identifier>(args[1]));
        return Value(p);
    }
};

// ..%namespace宣言
struct namespace_decl_action { // directive_namespace << identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<NamespaceDecl>(
            range(args), get_symbol<Identifier>(args[1]));
        return Value(p);
    }
};

// ..%recover宣言
struct recover_decl_action { // directive_recover << identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<RecoverDecl>(
            range(args), get_symbol<Identifier>(args[1]));
        return Value(p);
    }
};

// ..%dont_use_stl宣言
struct dont_use_stl_decl_action { // directive_dont_use_stl;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<DontUseSTLDecl>(range(args));
        return Value(p);
    }
};

// .文法セクション
struct entries0_action { // entry;
    value_type operator()(const arguments_type args) const {
        std::vector<std::shared_ptr<Rule>> v;
        v.push_back(get_node<Rule>(args[0]));

        auto q = std::make_shared<Rules>(range(args), v);
        return Value(q);
    }
};
struct entries1_action { // entries << entry;
    value_type operator()(const arguments_type args) const {
        std::shared_ptr<Rules> p(get_node<Rules>(args[0]));
        p->rules.push_back(get_node<Rule>(args[1]));
        return Value(p);
    }
};

// ..文法
struct entry_action { // identifier << typetag << derivations << semicolon;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<Rule>(
            range(args),
            get_symbol<Identifier>(args[0]),
            get_symbol<TypeTag>(args[1]),
            get_node<Choises>(args[2]));
        return Value(p);
    }
};

// ...右辺
struct derivations0_action { // colon << derivation;
    value_type operator()(const arguments_type args) const {
        std::vector<std::shared_ptr<Choise>> v;
        v.push_back(get_node<Choise>(args[1]));

        auto r = std::make_shared<Choises>(range(args), v);
        return Value(r);
    }
};
struct derivations1_action { // derivations << pipe << derivation; 
    value_type operator()(const arguments_type args) const {
        std::shared_ptr<Choises> q = get_node<Choises>(args[0]);
        q->choises.push_back(get_node<Choise>(args[2]));
        return Value(q);
    }
};

// ...右辺の項目
struct derivation0_action { // lbracket  << rbracket;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<Choise>(
            range(args),
            "",
            std::vector<std::shared_ptr<Term>>());
        return Value(p);
    }
};
struct derivation1_action { // lbracket << identifier << rbracket;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<Choise>(
            range(args),
            get_symbol<Identifier>(args[1]),
            std::vector<std::shared_ptr<Term>>());
        return Value(p);
    }
};
struct derivation2_action { // derivation << term;
    value_type operator()(const arguments_type args) const {
        std::shared_ptr<Choise> q = get_node<Choise>(args[0]);
        q->elements.push_back(get_node<Term>(args[1]));

        return Value(q);
    }
};
struct term0_action { // identifier;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<Term>(
            range(args), get_symbol<Identifier>(args[0]), -1);
        return Value(p);
    }
};
struct term1_action { // identifier << lparen << integer << rparen;
    value_type operator()(const arguments_type args) const {
        auto p = std::make_shared<Term>(
            range(args),
            get_symbol<Identifier>(args[0]),
            boost::get<Integer>(args[2].data).n);
        return Value(p);
    }
};

////////////////////////////////////////////////////////////////
// make_cpg_parser
void make_cpg_parser(cpg::parser& p) {
    cpg::terminal directive_token("%token", token_directive_token);
    cpg::terminal directive_token_prefix("%token_prefix", token_directive_token_prefix);
    cpg::terminal directive_external_token("%external_token", token_directive_external_token);
    cpg::terminal directive_access_modifier("%access_modifier", token_directive_access_modifier);
    cpg::terminal directive_namespace("%namespace", token_directive_namespace);
    cpg::terminal directive_dont_use_stl("%dont_use_stl", token_directive_dont_use_stl);
    cpg::terminal identifier("IDENT", token_identifier);
    cpg::terminal recovery("@error", token_recovery);
    cpg::terminal integer("int", token_integer);
    cpg::terminal typetag("<type>", token_typetag);
    cpg::terminal semicolon(";", token_semicolon);
    cpg::terminal colon(":", token_colon);
    cpg::terminal pipe("|", token_pipe);
    cpg::terminal lparen("(", token_lparen);
    cpg::terminal rparen(")", token_rparen);
    cpg::terminal lbracket("[", token_lbracket);
    cpg::terminal rbracket("]", token_rbracket);

    cpg::nonterminal document("Document");
    cpg::nonterminal sections("Sections");
    cpg::nonterminal declaration("Declaration");
    cpg::nonterminal declarations("Declarations");
    cpg::nonterminal token_decl("TokenDecl");
    cpg::nonterminal token_decl_element("TokenDeclElement");
    cpg::nonterminal token_prefix_decl("TokenPrefixDecl");
    cpg::nonterminal external_token_decl("ExternalTokenDecl");
    cpg::nonterminal access_modifier_decl("AccessModifierDecl");
    cpg::nonterminal namespace_decl("NamespaceDecl");
    cpg::nonterminal dont_use_stl_decl("DontUseSTLDecl");
    cpg::nonterminal entries("Entries");
    cpg::nonterminal entry("Entry");
    cpg::nonterminal entry_name("EntryName");
    cpg::nonterminal derivations("Derivations");
    cpg::nonterminal derivation("Derivation");
    cpg::nonterminal term("Term");

    // 全体
    cpg::rule r_document(document);            r_document      << sections;
    cpg::rule r_sections(sections);            r_sections      << declarations << entries;

    // .宣言セクション
    cpg::rule r_declarations0(declarations);   r_declarations0 << declaration;
    cpg::rule r_declarations1(declarations);   r_declarations1 << declarations << declaration;

    // ..宣言
    cpg::rule r_declaration0(declaration);     r_declaration0 << token_decl << semicolon;
    cpg::rule r_declaration1(declaration);     r_declaration1 << token_prefix_decl << semicolon;
    cpg::rule r_declaration2(declaration);     r_declaration2 << external_token_decl << semicolon;
    cpg::rule r_declaration3(declaration);     r_declaration3 << namespace_decl << semicolon;
    cpg::rule r_declaration4(declaration);     r_declaration4 << dont_use_stl_decl << semicolon;
    cpg::rule r_declaration5(declaration);     r_declaration5 << access_modifier_decl << semicolon;

    // ..%token宣言
    cpg::rule r_token_decl0(token_decl);       r_token_decl0 << directive_token;
    cpg::rule r_token_decl1(token_decl);       r_token_decl1 << token_decl << token_decl_element;
    cpg::rule r_token_decl_element0(token_decl_element); r_token_decl_element0 << identifier;
    cpg::rule r_token_decl_element1(token_decl_element); r_token_decl_element1 << identifier << typetag;

    // ..%token_prefix宣言
    //cpg::rule r_token_prefix_decl(token_prefix_decl); r_token_prefix_decl << directive_token_prefix << identifier;
    cpg::rule r_token_pfx_decl0(token_prefix_decl); r_token_pfx_decl0 << directive_token_prefix;
    cpg::rule r_token_pfx_decl1(token_prefix_decl); r_token_pfx_decl1 << directive_token_prefix << identifier;

    // ..%external_token宣言
    cpg::rule r_external_token_decl(external_token_decl); r_external_token_decl << directive_external_token;

    // ..%access_modifier宣言
    cpg::rule r_access_modifier_decl(access_modifier_decl);
    r_access_modifier_decl << directive_access_modifier << identifier;

    // ..%namespace宣言
    cpg::rule r_namespace_decl(namespace_decl); r_namespace_decl << directive_namespace << identifier;

    // ..%dont_use_stl宣言
    cpg::rule r_dont_use_stl_decl(dont_use_stl_decl); r_dont_use_stl_decl << directive_dont_use_stl;

    // .文法セクション
    cpg::rule r_entries0(entries);             r_entries0 << entry;
    cpg::rule r_entries1(entries);             r_entries1 << entries << entry;

    // ..文法
    cpg::rule r_entry(entry);                  r_entry << identifier << typetag << derivations << semicolon;

    // ...右辺
    cpg::rule r_derivations0(derivations);     r_derivations0 << colon << derivation;
    cpg::rule r_derivations1(derivations);     r_derivations1 << derivations << pipe << derivation; 

    cpg::rule r_derivation0(derivation);       r_derivation0 << lbracket << rbracket;
    cpg::rule r_derivation1(derivation);       r_derivation1 << lbracket << identifier << rbracket;
    cpg::rule r_derivation2(derivation);       r_derivation2 << derivation << term;

    // ...右辺の項目
    cpg::rule r_term0(term);                   r_term0 << identifier;
    cpg::rule r_term1(term);                   r_term1 << identifier << lparen << integer << rparen;

    // 入力ファイルの文法作成
    cpg::grammar g(r_document);
    g << r_sections
      << r_declarations0
      << r_declarations1
      << r_declaration0
      << r_declaration1
      << r_declaration2
      << r_declaration3
      << r_declaration4
      << r_declaration5
      << r_token_decl0
      << r_token_decl1
      << r_token_decl_element0
      << r_token_decl_element1
        //<< r_token_prefix_decl
      << r_token_pfx_decl0
      << r_token_pfx_decl1
      << r_external_token_decl
      << r_access_modifier_decl
      << r_namespace_decl
      << r_dont_use_stl_decl
      << r_entries0
      << r_entries1
      << r_entry
      << r_derivations0
      << r_derivations1
      << r_derivation0
      << r_derivation1
      << r_derivation2
      << r_term0
      << r_term1
        ;

    // parsing tableの作成
    cpg::parsing_table table;
    cpg::make_lalr_table(table, g, token_error);
    //std::cerr << "\n[LALR parsing table]\n";
    //std::cerr << table;

    p.reset(table);

    p.set_semantic_action(r_document, document_action());
    p.set_semantic_action(r_sections, sections_action());

    p.set_semantic_action(r_declarations0, declarations0_action());
    p.set_semantic_action(r_declarations1, declarations1_action());

    p.set_semantic_action(r_declaration0, declaration0_action());
    p.set_semantic_action(r_declaration1, declaration1_action());
    p.set_semantic_action(r_declaration2, declaration2_action());
    p.set_semantic_action(r_declaration3, declaration3_action());
    p.set_semantic_action(r_declaration4, declaration4_action());
    p.set_semantic_action(r_declaration5, declaration5_action());

    p.set_semantic_action(r_token_decl0, token_decl0_action());
    p.set_semantic_action(r_token_decl1, token_decl1_action());
    p.set_semantic_action(r_token_decl_element0, token_decl_element0_action());
    p.set_semantic_action(r_token_decl_element1, token_decl_element1_action());

    //p.set_semantic_action(r_token_prefix_decl, token_prefix_decl_action());
    p.set_semantic_action(r_token_pfx_decl0, token_pfx_decl0_action());
    p.set_semantic_action(r_token_pfx_decl1, token_pfx_decl1_action());
    p.set_semantic_action(r_external_token_decl, external_token_decl_action());
    p.set_semantic_action(r_access_modifier_decl, access_modifier_decl_action());
    p.set_semantic_action(r_namespace_decl, namespace_decl_action());
    p.set_semantic_action(r_dont_use_stl_decl, dont_use_stl_decl_action());

    p.set_semantic_action(r_entries0, entries0_action());
    p.set_semantic_action(r_entries1, entries1_action());

    p.set_semantic_action(r_entry, entry_action());

    p.set_semantic_action(r_derivations0, derivations0_action());
    p.set_semantic_action(r_derivations1, derivations1_action());

    p.set_semantic_action(r_derivation0, derivation0_action());
    p.set_semantic_action(r_derivation1, derivation1_action());
    p.set_semantic_action(r_derivation2, derivation2_action());

    p.set_semantic_action(r_term0, term0_action());
    p.set_semantic_action(r_term1, term1_action());
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
        if (auto recoverdecl = downcast<RecoverDecl>(x)) {
            if (0 < known.count(recoverdecl->name)) {
                throw duplicated_symbol(
                    recoverdecl->range.beg, recoverdecl->name);
            }
            known.insert(recoverdecl->name);
            terminal_types[recoverdecl->name] = "$recover";
        }
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
        if (auto accessmodifierdecl = downcast<AccessModifierDecl>(x)) {
            // %access_modifier宣言
            options.access_modifier = accessmodifierdecl->modifier + " ";
        }
        if (auto namespacedecl = downcast<NamespaceDecl>(x)) {
            // %namespace宣言
            options.namespace_name = namespacedecl->name;
        }
        if (auto dontusestldecl = downcast<DontUseSTLDecl>(x)) {
            // %dont_use_stl宣言
            options.dont_use_stl = true;
        }
    }

    // 規則
    std::shared_ptr<Rules> rules = doc->rules;
    for (const auto& rule: rules->rules) {
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

