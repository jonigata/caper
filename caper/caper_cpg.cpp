// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "caper_cpg.hpp"
#include "caper_error.hpp"

////////////////////////////////////////////////////////////////
// cpg semantic actions
// 全体
value_type document_action::operator()( const cpg::parser::arguments& args ) const // declarations
{
        return args[0];
}

value_type sections_action::operator()( const cpg::parser::arguments& args ) const // declarations << rules;
{
        boost::shared_ptr< Document > p(
                new Document(
                        range( args ), 
                        get_node< Declarations >( args[0] ), 
                        get_node< Rules >       ( args[1] ) ) );
        return Value( range( args ), p );
}

// .宣言セクション
value_type declarations0_action::operator()( const cpg::parser::arguments& args ) const // declaration;
{
        std::vector< boost::shared_ptr< Declaration > > v;
        v.push_back( get_node< Declaration >( args[0] ) );
        boost::shared_ptr< Declarations > q( new Declarations( range( args ), v ) );
        return Value( range( args ), q );
}

value_type declarations1_action::operator()( const cpg::parser::arguments& args ) const // declarations << declaration;
{
        boost::shared_ptr< Declarations > p = get_node< Declarations >( args[0] );
        p->declarations.push_back( get_node< Declaration >( args[1] ) );
        return Value( range( args ), p );
}

// ..宣言
value_type declaration0_action::operator()( const cpg::parser::arguments& args ) const // token_decl << semicolon;
{
        return Value( range( args ), args[0].data );
}

value_type declaration1_action::operator()( const cpg::parser::arguments& args ) const // token_prefix_decl << semicolon;
{
        return Value( range( args ), args[0].data );
}

value_type declaration2_action::operator()( const cpg::parser::arguments& args ) const // ext_token_decl << semicolon;
{
        return Value( range( args ), args[0].data );
}

value_type declaration3_action::operator()( const cpg::parser::arguments& args ) const // namespace_decl << semicolon;
{
        return Value( range( args ), args[0].data );
}

value_type declaration4_action::operator()( const cpg::parser::arguments& args ) const // dont_use_stl_decl << semi;
{
        return Value( range( args ), args[0].data );
}

value_type declaration5_action::operator()( const cpg::parser::arguments& args ) const // access_modifier_decl << semi;
{
        return Value( range( args ), args[0].data );
}

// ..%token宣言
value_type token_decl0_action::operator()( const cpg::parser::arguments& args ) const // directive_token;
{
        boost::shared_ptr< TokenDecl > p( new TokenDecl( range( args ) ) );
        return Value( range( args ), p );
}

value_type token_decl1_action::operator()( const cpg::parser::arguments& args ) const // token_decl << token_decl_elem;
{
        boost::shared_ptr< TokenDecl > p = get_node< TokenDecl >( args[0] );
        p->elements.push_back( get_node< TokenDeclElement >( args[1] ) );
        return Value( range( args ), p );
}

value_type token_decl_element0_action::operator()( const cpg::parser::arguments& args ) const // identifier;
{
        boost::shared_ptr< TokenDeclElement > p(
                new TokenDeclElement( range( args ), boost::get< Identifier >( args[0].data ).s ) );
        return Value( range( args ), p );
}

value_type token_decl_element1_action::operator()( const cpg::parser::arguments& args ) const // identifier << typetag;
{
        boost::shared_ptr< TokenDeclElement > p(
                new TokenDeclElement(
                        range( args ),
                        boost::get< Identifier >( args[0].data ).s,
                        boost::get< TypeTag >( args[1].data ).s ) );
        return Value( range( args ), p );
}

// ..%token_prefix宣言
/*
value_type token_prefix_decl_action::operator()( const cpg::parser::arguments& args ) const // dir_prefix << ident;
{
        boost::shared_ptr< TokenPrefixDecl > p(
                new TokenPrefixDecl( range( args ), boost::get< Identifier >( args[1].data ).s ) );
        return Value( range( args ), p );
}
*/
value_type token_pfx_decl0_action::operator()( const cpg::parser::arguments& args ) const // dir_prefix << ident;
{
        boost::shared_ptr< TokenPrefixDecl > p( new TokenPrefixDecl( range( args ), "" ) );
        return Value( range( args ), p );
}

value_type token_pfx_decl1_action::operator()( const cpg::parser::arguments& args ) const // dir_prefix << ident;
{
        boost::shared_ptr< TokenPrefixDecl > p(
                new TokenPrefixDecl( range( args ), boost::get< Identifier >( args[1].data ).s ) );
        return Value( range( args ), p );
}

// ..%external_token宣言
value_type external_token_decl_action::operator()( const cpg::parser::arguments& args ) const // dir_ns << ident;
{
        boost::shared_ptr< ExternalTokenDecl > p( new ExternalTokenDecl( range( args ) ) );
        return Value( range( args ), p );
}

// ..%access_modifier宣言
value_type access_modifier_decl_action::operator()( const cpg::parser::arguments& args ) const // dir_am << identifier;
{
        boost::shared_ptr< AccessModifierDecl > p(
                new AccessModifierDecl( range( args ), boost::get< Identifier >( args[1].data ).s ) );
        return Value( range( args ), p );
}

// ..%namespace宣言
value_type namespace_decl_action::operator()( const cpg::parser::arguments& args ) const // dir_ns << identifier;
{
        boost::shared_ptr< NamespaceDecl > p(
                new NamespaceDecl( range( args ), boost::get< Identifier >( args[1].data ).s ) );
        return Value( range( args ), p );
}

// ..%dont_use_stl宣言
value_type dont_use_stl_decl_action::operator()( const cpg::parser::arguments& args ) const // dir_ns << identifier;
{
        boost::shared_ptr< DontUseSTLDecl > p( new DontUseSTLDecl( range( args ) ) );
        return Value( range( args ), p );
}

// .文法セクション
value_type entries0_action::operator()( const cpg::parser::arguments& args ) const // entry;
{
        std::vector< boost::shared_ptr< Rule > > v;
        v.push_back( get_node< Rule >( args[0] ) );

        boost::shared_ptr< Rules > q( new Rules( range( args ), v ) );

        return Value( range( args ), q );
}

value_type entries1_action::operator()( const cpg::parser::arguments& args ) const // entries << entry;
{
        boost::shared_ptr< Rules > p( get_node< Rules >( args[0] ) );
        p->rules.push_back( get_node< Rule >( args[1] ) );
        return Value( range( args ), p );
}

// ..文法
value_type  entry_action::operator()( const cpg::parser::arguments& args ) const // ident << typetag << deriv << semi;
{
        boost::shared_ptr< Rule > p(
                new Rule(
                        range( args ), 
                        boost::get< Identifier >( args[0].data ).s,
                        boost::get< TypeTag >( args[1].data ).s,
                        get_node< Choises >( args[2] ) ) );
        return Value( range( args ), p );
}


// ...右辺
value_type derivations0_action::operator()( const cpg::parser::arguments& args ) const // colon << derivation;
{
        std::vector< boost::shared_ptr< Choise > > v;
        v.push_back( get_node< Choise >( args[1] ) );

        boost::shared_ptr< Choises > r( new Choises( range( args ), v ) );
        return Value( range( args ), r );
}

value_type derivations1_action::operator()( const cpg::parser::arguments& args ) const // deriv << pipe << deriv; 
{
        boost::shared_ptr< Choises > q = get_node< Choises >( args[0] );
        q->choises.push_back( get_node< Choise >( args[2] ) );
        return Value( range( args ), q );
}

// ...右辺の項目
value_type derivation0_action::operator()( const cpg::parser::arguments& args ) const // lbracket  << rbracket;
{
        boost::shared_ptr< Choise > p(
                new Choise(
                        range( args ), 
                        "",
                        std::vector< boost::shared_ptr< Term > >() ) );
        return Value( range( args ), p );
}

value_type derivation1_action::operator()( const cpg::parser::arguments& args ) const // lbracket << ident << rbracket;
{
        boost::shared_ptr< Choise > p(
                new Choise(
                        range( args ), 
                        boost::get< Identifier >( args[1].data ).s,
                        std::vector< boost::shared_ptr< Term > >() ) );
        return Value( range( args ), p );
}

value_type derivation2_action::operator()( const cpg::parser::arguments& args ) const // derivation << term;
{
        boost::shared_ptr< Choise > q = get_node< Choise >( args[0] );
        q->terms.push_back( get_node< Term >( args[1] ) );

        return Value( range( args ), q );
}

value_type term0_action::operator()( const cpg::parser::arguments& args ) const // identifier;
{
        boost::shared_ptr< Term > p(
                new Term( range( args ), boost::get< Identifier >( args[0].data ).s, -1 ) );
        return Value( range( args ), p );
}

value_type term1_action::operator()( const cpg::parser::arguments& args ) const // ident << lparen << int << rparen;
{
        boost::shared_ptr< Term > p(
                new Term(
                        range( args ), 
                        boost::get< Identifier >( args[0].data ).s,
                        boost::get< Integer >( args[2].data ).n ) );
        return Value( range( args ), p );
}

////////////////////////////////////////////////////////////////
// make_cpg_parser
void make_cpg_parser( cpg::parser& p )
{
        cpg::terminal directive_token( "%token", token_directive_token );
        cpg::terminal directive_token_prefix( "%token_prefix", token_directive_token_prefix );
        cpg::terminal directive_external_token( "%external_token", token_directive_external_token );
        cpg::terminal directive_access_modifier( "%access_modifier", token_directive_access_modifier );
        cpg::terminal directive_namespace( "%namespace", token_directive_namespace );
        cpg::terminal directive_dont_use_stl( "%dont_use_stl", token_directive_dont_use_stl );
        cpg::terminal identifier( "IDENT", token_identifier );
        cpg::terminal integer( "int", token_integer );
        cpg::terminal typetag( "<type>", token_typetag );
        cpg::terminal semicolon( ";", token_semicolon );
        cpg::terminal colon( ":", token_colon );
        cpg::terminal pipe( "|", token_pipe );
        cpg::terminal lparen( "(", token_lparen );
        cpg::terminal rparen( ")", token_rparen );
        cpg::terminal lbracket( "[", token_lbracket );
        cpg::terminal rbracket( "]", token_rbracket );

        cpg::nonterminal document( "Document" );
        cpg::nonterminal sections( "Sections" );
        cpg::nonterminal declaration( "Declaration" );
        cpg::nonterminal declarations( "Declarations" );
        cpg::nonterminal token_decl( "TokenDecl" );
        cpg::nonterminal token_decl_element( "TokenDeclElement" );
        cpg::nonterminal token_prefix_decl( "TokenPrefixDecl" );
        cpg::nonterminal external_token_decl( "ExternalTokenDecl" );
        cpg::nonterminal access_modifier_decl( "AccessModifierDecl" );
        cpg::nonterminal namespace_decl( "NamespaceDecl" );
        cpg::nonterminal dont_use_stl_decl( "DontUseSTLDecl" );
        cpg::nonterminal entries( "Entries" );
        cpg::nonterminal entry( "Entry" );
        cpg::nonterminal entry_name( "EntryName" );
        cpg::nonterminal derivations( "Derivations" );
        cpg::nonterminal derivation( "Derivation" );
        cpg::nonterminal term( "Term" );

        // 全体
        cpg::rule r_document( document );            r_document      << sections;
        cpg::rule r_sections( sections );            r_sections      << declarations << entries;

        // .宣言セクション
        cpg::rule r_declarations0( declarations );   r_declarations0 << declaration;
        cpg::rule r_declarations1( declarations );   r_declarations1 << declarations << declaration;

        // ..宣言
        cpg::rule r_declaration0( declaration );     r_declaration0 << token_decl << semicolon;
        cpg::rule r_declaration1( declaration );     r_declaration1 << token_prefix_decl << semicolon;
        cpg::rule r_declaration2( declaration );     r_declaration2 << external_token_decl << semicolon;
        cpg::rule r_declaration3( declaration );     r_declaration3 << namespace_decl << semicolon;
        cpg::rule r_declaration4( declaration );     r_declaration4 << dont_use_stl_decl << semicolon;
        cpg::rule r_declaration5( declaration );     r_declaration5 << access_modifier_decl << semicolon;

        // ..%token宣言
        cpg::rule r_token_decl0( token_decl );       r_token_decl0 << directive_token;
        cpg::rule r_token_decl1( token_decl );       r_token_decl1 << token_decl << token_decl_element;
        cpg::rule r_token_decl_element0( token_decl_element ); r_token_decl_element0 << identifier;
        cpg::rule r_token_decl_element1( token_decl_element ); r_token_decl_element1 << identifier << typetag;

        // ..%token_prefix宣言
        //cpg::rule r_token_prefix_decl( token_prefix_decl ); r_token_prefix_decl << directive_token_prefix << identifier;
        cpg::rule r_token_pfx_decl0( token_prefix_decl ); r_token_pfx_decl0 << directive_token_prefix;
        cpg::rule r_token_pfx_decl1( token_prefix_decl ); r_token_pfx_decl1 << directive_token_prefix << identifier;

        // ..%external_token宣言
        cpg::rule r_external_token_decl( external_token_decl ); r_external_token_decl << directive_external_token;

        // ..%access_modifier宣言
        cpg::rule r_access_modifier_decl( access_modifier_decl );
        r_access_modifier_decl << directive_access_modifier << identifier;

        // ..%namespace宣言
        cpg::rule r_namespace_decl( namespace_decl ); r_namespace_decl << directive_namespace << identifier;

        // ..%dont_use_stl宣言
        cpg::rule r_dont_use_stl_decl( dont_use_stl_decl ); r_dont_use_stl_decl << directive_dont_use_stl;

        // .文法セクション
        cpg::rule r_entries0( entries );             r_entries0 << entry;
        cpg::rule r_entries1( entries );             r_entries1 << entries << entry;

        // ..文法
        cpg::rule r_entry( entry );                  r_entry << identifier << typetag << derivations << semicolon;

        // ...右辺
        cpg::rule r_derivations0( derivations );     r_derivations0 << colon << derivation;
        cpg::rule r_derivations1( derivations );     r_derivations1 << derivations << pipe << derivation; 

        // ...右辺の項目
        cpg::rule r_derivation0( derivation );       r_derivation0 << lbracket << rbracket;
        cpg::rule r_derivation1( derivation );       r_derivation1 << lbracket << identifier << rbracket;
        cpg::rule r_derivation2( derivation );       r_derivation2 << derivation << term;

        cpg::rule r_term0( term );                   r_term0 << identifier;
        cpg::rule r_term1( term );                   r_term1 << identifier << lparen << integer << rparen;
        

        // 入力ファイルの文法作成
        cpg::grammar g( r_document );
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
        cpg::make_lalr_table( table, g );
	//std::cerr << "\n[LALR parsing table]\n";
        //std::cerr << table;

        p.reset( table );

        p.set_semantic_action( r_document, document_action() );
        p.set_semantic_action( r_sections, sections_action() );

        p.set_semantic_action( r_declarations0, declarations0_action() );
        p.set_semantic_action( r_declarations1, declarations1_action() );

        p.set_semantic_action( r_declaration0, declaration0_action() );
        p.set_semantic_action( r_declaration1, declaration1_action() );
        p.set_semantic_action( r_declaration2, declaration2_action() );
        p.set_semantic_action( r_declaration3, declaration3_action() );
        p.set_semantic_action( r_declaration4, declaration4_action() );
        p.set_semantic_action( r_declaration5, declaration5_action() );

        p.set_semantic_action( r_token_decl0, token_decl0_action() );
        p.set_semantic_action( r_token_decl1, token_decl1_action() );
        p.set_semantic_action( r_token_decl_element0, token_decl_element0_action() );
        p.set_semantic_action( r_token_decl_element1, token_decl_element1_action() );

        //p.set_semantic_action( r_token_prefix_decl, token_prefix_decl_action() );
        p.set_semantic_action( r_token_pfx_decl0, token_pfx_decl0_action() );
        p.set_semantic_action( r_token_pfx_decl1, token_pfx_decl1_action() );
        p.set_semantic_action( r_external_token_decl, external_token_decl_action() );
        p.set_semantic_action( r_access_modifier_decl, access_modifier_decl_action() );
        p.set_semantic_action( r_namespace_decl, namespace_decl_action() );
        p.set_semantic_action( r_dont_use_stl_decl, dont_use_stl_decl_action() );

        p.set_semantic_action( r_entries0, entries0_action() );
        p.set_semantic_action( r_entries1, entries1_action() );

        p.set_semantic_action( r_entry, entry_action() );

        p.set_semantic_action( r_derivations0, derivations0_action() );
        p.set_semantic_action( r_derivations1, derivations1_action() );

        p.set_semantic_action( r_derivation0, derivation0_action() );
        p.set_semantic_action( r_derivation1, derivation1_action() );
        p.set_semantic_action( r_derivation2, derivation2_action() );

        p.set_semantic_action( r_term0, term0_action() );
        p.set_semantic_action( r_term1, term1_action() );
}

////////////////////////////////////////////////////////////////
// collect_informations
void collect_informations(
        GenerateOptions&        options,
        symbol_map_type&        terminal_types,
        symbol_map_type&        nonterminal_types,
        const value_type&       ast )
{
        symbol_set_type methods;        // セマンティックアクション名(重複(不可)のチェック)
        symbol_set_type known;          // 確定識別子名
        symbol_set_type unknown;        // 未確定識別子名

        boost::shared_ptr< Document > doc = get_node< Document >( ast );
        
        // 宣言
        boost::shared_ptr< Declarations > declarations = doc->declarations;
        for( std::vector< boost::shared_ptr< Declaration > >::const_iterator i = declarations->declarations.begin() ;
             i != declarations->declarations.end() ;
             ++i ) {
                boost::shared_ptr< TokenDecl > tokendecl = boost::dynamic_pointer_cast< TokenDecl >( *i );
                if( tokendecl ) {
                        // %token宣言
                        for( std::vector< boost::shared_ptr< TokenDeclElement > >::const_iterator j =
                                     tokendecl->elements.begin() ;
                             j != tokendecl->elements.end() ;
                             ++j ) {
                                //std::cerr << "token: " << (*j)->name << std::endl;
                                if( known.find( (*j)->name ) != known.end() ) {
                                        throw duplicated_symbol( tokendecl->range.beg, (*j)->name );
                                }
                                known.insert( (*j)->name );
                                terminal_types[(*j)->name] = (*j)->type.s;
                        }
                }
                boost::shared_ptr< TokenPrefixDecl > tokenprefixdecl =
                        boost::dynamic_pointer_cast< TokenPrefixDecl >( *i );
                if( tokenprefixdecl ) {
                        // %token_prefix宣言
                        options.token_prefix = tokenprefixdecl->prefix;
                }
                boost::shared_ptr< ExternalTokenDecl > externaltokendecl =
                        boost::dynamic_pointer_cast< ExternalTokenDecl >( *i );
                if( externaltokendecl ) {
                        // %external_token宣言
                        options.external_token = true;
                }
                boost::shared_ptr< AccessModifierDecl > accessmodifierdecl =
                        boost::dynamic_pointer_cast< AccessModifierDecl >( *i );
                if( accessmodifierdecl ) {
                        // %access_modifier宣言
                        options.access_modifier = accessmodifierdecl->modifier + " ";
                }
                boost::shared_ptr< NamespaceDecl > namespacedecl =
                        boost::dynamic_pointer_cast< NamespaceDecl >( *i );
                if( namespacedecl ) {
                        // %namespace宣言
                        options.namespace_name = namespacedecl->name;
                }
                boost::shared_ptr< DontUseSTLDecl > dontusestldecl =
                        boost::dynamic_pointer_cast< DontUseSTLDecl >( *i );
                if( dontusestldecl ) {
                        // %dont_use_stl宣言
                        options.dont_use_stl = true;
                }
        }

        // 規則
        boost::shared_ptr< Rules > rules = doc->rules;
        for( std::vector< boost::shared_ptr< Rule > >::const_iterator i = rules->rules.begin() ;
             i != rules->rules.end() ;
             ++i ) {
                boost::shared_ptr< Rule > rule = *i;
                //std::cerr << "known: " << rule->name << std::endl;
                if( known.find( rule->name ) != known.end() ) {
                        throw duplicated_symbol( rule->range.beg, rule->name );
                }
                known.insert( rule->name );
                nonterminal_types[rule->name] = rule->type.s;

                for( std::vector< boost::shared_ptr< Choise > >::const_iterator j = rule->choises->choises.begin() ;
                     j != rule->choises->choises.end() ;
                     ++j ) {
                        boost::shared_ptr< Choise > choise = *j;

                        //std::cerr << "method: " << choise->name << std::endl;
                        if( methods.find( choise->name ) != methods.end() ) {
                                // 重複
                                // TODO: 例外
                                //std::cerr << "duplicated method name: " << choise->name << std::endl;
                        }
                        methods.insert( choise->name );

                        for( std::vector< boost::shared_ptr< Term > >::const_iterator k = choise->terms.begin() ;
                             k != choise->terms.end() ;
                             ++k ) {
                                //std::cerr << "unknown: " << (*k)->name << std::endl;
                                unknown.insert( (*k)->name );
                        }                        
                }
        }

        // 未確定識別子が残っていたらエラー
        for( std::set< std::string >::const_iterator i = unknown.begin() ;
             i != unknown.end() ;
             ++i ) {
                if( known.find( *i ) == known.end() ) {
                        throw undefined_symbol( -1, *i );
                }
        }
}

