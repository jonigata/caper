#ifndef CAPER_AST_HPP
#define CAPER_AST_HPP

#include "fastlalr.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

////////////////////////////////////////////////////////////////
// Token
enum Token {
	token_empty,
	token_identifier,
	token_integer,
        token_typetag,
        token_colon,
        token_semicolon,
        token_pipe,
        token_lparen,
        token_rparen,
        token_lbracket,
        token_rbracket,
        token_equal,
        token_directive_token,
        token_directive_token_prefix,
        token_directive_external_token,
        token_directive_access_modifier,
        token_directive_namespace,
        token_directive_dont_use_stl,
        token_eof,
};

struct TokenTraits {
        static Token eof() { return token_eof; }
};

inline
std::ostream& operator<<( std::ostream& os, Token op )
{
        const char* display[] = {
                "e",
                "IDENT",
                "number",
                "<type>",
                ":",
                ";",
                "|",
                "(",
                ")",
                "[",
                "]",
                "=",
                ".",
                "%token",
                "%token_prefix",
                "%external_token",
                "%namespace",
                "%dont_use_stl",
                "$"
        };
        
        os <<  display[op];
        return os;
}


////////////////////////////////////////////////////////////////
// Range
struct Range {
        int beg;
        int end;

        Range() : beg(-1), end(-1) { }
        Range( int ab, int ae ) : beg( ab ), end( ae ) {}
};

////////////////////////////////////////////////////////////////
// Nil
struct Nil {
};

////////////////////////////////////////////////////////////////
// Operator
struct Operator {
        char  c;

        Operator(){}
        Operator( char ac ) : c( ac ) {}
};

inline
std::ostream& operator<<( std::ostream& os, const Operator& op )
{
        os <<  op.c;
        return os;
}

////////////////////////////////////////////////////////////////
// Identifier
struct Identifier {
        std::string     s;
        
        Identifier(){}
        Identifier( const std::string& as ) : s( as ) {}
};

////////////////////////////////////////////////////////////////
// Directive
struct Directive {
        std::string     s;
        
        Directive(){}
        Directive( const std::string& as ) : s( as ) {}
};

////////////////////////////////////////////////////////////////
// TypeTag
struct TypeTag {
        std::string     s;
        
        TypeTag(){}
        TypeTag( const std::string& as ) : s( as ) {}
};

////////////////////////////////////////////////////////////////
// Integer
struct Integer {
        int     n;
        
        Integer(){}
        Integer( int an ) : n( an ) {}
};

////////////////////////////////////////////////////////////////
// Node
struct Node {
public:
        Node( const Range& r ) : range( r ){}
        virtual ~Node(){}

        Range range;
};

typedef boost::shared_ptr< Node > node_ptr;

////////////////////////////////////////////////////////////////
// value_type
struct Value {
        typedef boost::variant< Nil, Operator, Identifier, Directive, TypeTag, Integer, node_ptr > data_type;

        Range           range;
        data_type       data;

        Value() {}

        template < class T >
        Value( const Range& r, const T& d ) : range( r ), data( d ) {}
};

typedef Value value_type;

////////////////////////////////////////////////////////////////
// concrete Node
struct Term : public Node {
        std::string     name;
        int             index;

        Term( const Range& r, const std::string& as,int ai ) : Node( r ), name( as ), index( ai ) {}
};

struct Choise : public Node {
        std::string                                     name;
        std::vector< boost::shared_ptr< Term > >        terms;

        Choise( const Range& r, const std::string& as, const std::vector< boost::shared_ptr< Term > >& at)
                : Node( r ), name( as ), terms( at ) {}
};

struct Choises : public Node {
        std::vector< boost::shared_ptr< Choise > >      choises;

        Choises( const Range& r, const std::vector< boost::shared_ptr< Choise > >& av )
                : Node( r ), choises( av ) {}
};

struct Rule : public Node {
        std::string                     name;
        TypeTag                         type;
        boost::shared_ptr< Choises >    choises;

        Rule( const Range& r, const std::string& as, const TypeTag& at, boost::shared_ptr< Choises > ar )
                : Node( r ), name( as ), type( at ), choises( ar ) {}
};

struct Rules : public Node {
        std::vector< boost::shared_ptr< Rule > >        rules;

        Rules( const Range& r, const std::vector< boost::shared_ptr< Rule > >& av ) : Node( r ), rules( av ) {}
};

struct TokenDeclElement : public Node {
        std::string                     name;
        TypeTag                         type;

        TokenDeclElement( const Range& r, const std::string& as ) : Node( r ), name( as ) {}
        TokenDeclElement( const Range& r, const std::string& as, const TypeTag& at )
                : Node( r ), name( as ), type( at ) {}
};

struct Declaration : public Node {
        Declaration( const Range& r ) : Node( r ) {}
};

struct TokenDecl : public Declaration {
        std::vector< boost::shared_ptr< TokenDeclElement > >    elements;

        TokenDecl( const Range& r ) : Declaration( r ) {}
        TokenDecl( const Range& r, const std::vector< boost::shared_ptr< TokenDeclElement > >& av )
                : Declaration( r ), elements( av ) {}
};

struct TokenPrefixDecl : public Declaration {
        std::string     prefix;

        TokenPrefixDecl( const Range& r, const std::string& as ) : Declaration( r ), prefix( as ) {}
};

struct ExternalTokenDecl : public Declaration {
        ExternalTokenDecl( const Range& r ) : Declaration( r ) {}
};

struct AccessModifierDecl : public Declaration {
        std::string     modifier;

        AccessModifierDecl( const Range& r, const std::string& as ) : Declaration( r ), modifier( as ) {}
};

struct NamespaceDecl : public Declaration {
        std::string     name;

        NamespaceDecl( const Range& r, const std::string& as ) : Declaration( r ), name( as ) {}
};

struct DontUseSTLDecl : public Declaration {
        DontUseSTLDecl( const Range& r ) : Declaration( r ) {}
};

struct Declarations : public Node {
        std::vector< boost::shared_ptr< Declaration > > declarations;

        Declarations( const Range& r, const std::vector< boost::shared_ptr< Declaration > >& av )
                : Node( r ), declarations( av ) {}
};

struct Document : public Node {
        boost::shared_ptr< Declarations >       declarations;
        boost::shared_ptr< Rules >              rules;

        Document( const Range& r,
                  const boost::shared_ptr< Declarations >& ad,
                  const boost::shared_ptr< Rules >& ar )
                : Node( r ), declarations( ad ), rules( ar ) {}
};

////////////////////////////////////////////////////////////////
// misc types
struct TargetTokenTraits {
        static int eof() { return 0; }
};

typedef zw::gr::package< Token, TokenTraits, Value >    cpg;
typedef zw::gr::package< int, TargetTokenTraits, int >  tgt;

struct GenerateOptions {
        std::string     token_prefix;
        bool            external_token;
        std::string     access_modifier;
        std::string     namespace_name;
        bool            dont_use_stl;
};

typedef std::set< std::string>                  symbol_set_type;
typedef std::map< std::string, std::string>     symbol_map_type;

struct semantic_action_argument {
        int             src_index;
        std::string     type;
};
struct semantic_action {
        std::string                                     name;
        std::map<size_t,semantic_action_argument>       args;
};
typedef std::map< tgt::rule, semantic_action > action_map_type;

////////////////////////////////////////////////////////////////
// utility functions
template < class T >
boost::shared_ptr< T > get_node( const value_type& v )
{
        return boost::static_pointer_cast<T>( boost::get< node_ptr >( v.data ) );
}

template < class T >
Range range( const T& x ) { return Range( x[0].range.beg, x[x.size()-1].range.end ); }

#endif // CAPER_AST_HPP
