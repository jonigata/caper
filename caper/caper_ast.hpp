#ifndef CAPER_AST_HPP
#define CAPER_AST_HPP

#include <memory>
#include <vector>
#include <boost/variant.hpp>
#include "caper_token.hpp"
#include "fastlalr.hpp"

////////////////////////////////////////////////////////////////
// Range
struct Range {
    int beg = -1;
    int end = -1;

    Range() {}
    Range(int ab, int ae): beg(ab), end(ae) {}
};

////////////////////////////////////////////////////////////////
// extension(EBNF)
enum class Extension {
    None,
    Star,
    Plus,
    Question,
};

inline const char* extension_label(Extension e) {
    static const char* labels[] = {
        "",
        "*",
        "+",
        "?",
    };
    return labels[int(e)];
}

////////////////////////////////////////////////////////////////
// Nil
struct Nil {
};

////////////////////////////////////////////////////////////////
// Operator
struct Operator {
    char c = 0;

    Operator() {}
    Operator(char ac): c(ac) {}
};

inline
std::ostream& operator<<(std::ostream& os, const Operator& op) {
    os <<  op.c;
    return os;
}

////////////////////////////////////////////////////////////////
// Identifier
struct Identifier {
    std::string s;

    Identifier() {}
    Identifier(const std::string& as): s(as) {}
};

////////////////////////////////////////////////////////////////
// Directive
struct Directive {
    std::string s;

    Directive() {}
    Directive(const std::string& as) : s(as) {}
};

////////////////////////////////////////////////////////////////
// TypeTag
struct TypeTag {
    std::string s;

    TypeTag() {}
    TypeTag(const std::string& as) : s(as) {}
};

////////////////////////////////////////////////////////////////
// Integer
struct Integer {
    int n;

    Integer() {}
    Integer(int an): n(an) {}
};

////////////////////////////////////////////////////////////////
// Node
struct Node {
public:
    Node(const Range& r): range(r) {}
    virtual ~Node() {}

    Range range;
};

typedef std::shared_ptr<Node> node_ptr;

////////////////////////////////////////////////////////////////
// value_type
struct Value {
    typedef boost::variant<Nil, Operator, Identifier, Directive, TypeTag, Integer, node_ptr> data_type;

    Range       range;
    data_type   data;

    Value() {}

    Value(node_ptr x) : range(x->range), data(x) {}
    
    template <class T>
    Value(const Range& r, const T& d) : range(r), data(d) {}

    Value(const Value& x) : range(x.range), data(x.data) {}
};

typedef Value value_type;

////////////////////////////////////////////////////////////////
// concrete Node
struct Item : public Node {
    std::string name;
    Extension   extension;

    Item(const Range& r, const std::string& n, Extension extension)
        : Node(r), name(n), extension(extension) {}
};

struct Term : public Node {
    std::shared_ptr<Item>   item;
    int                     argument_index;

    Term(const Range& r, std::shared_ptr<Item> p, int ai)
        : Node(r), item(p), argument_index(ai) {}
};

struct Choise : public Node {
    typedef std::vector<std::shared_ptr<Term>>
        elements_type;

    std::string     action_name;
    elements_type   elements;

    Choise(const Range& r, const std::string& as, const elements_type& ae)
        : Node(r), action_name(as), elements(ae) {}
};

struct Choises : public Node {
    typedef std::vector<std::shared_ptr<Choise>> choises_type;

    choises_type choises;

    Choises(const Range& r, const choises_type& av)
        : Node(r), choises(av) {}
};

struct Rule : public Node {
    std::string                 name;
    TypeTag                     type;
    std::shared_ptr<Choises>    choises;

    Rule(const Range& r,
         const std::string& as,
         const TypeTag& at,
         const std::shared_ptr<Choises>& ar)
        : Node(r), name(as), type(at), choises(ar) {}
};

struct Rules : public Node {
    typedef std::vector<std::shared_ptr<Rule>> rules_type;

    rules_type rules;
    
    Rules(const Range& r, const rules_type& av) : Node(r), rules(av) {}
};

struct TokenDeclElement : public Node {
    std::string name;
    TypeTag     type;

    TokenDeclElement(const Range& r, const std::string& as)
        : Node(r), name(as) {}
    TokenDeclElement(const Range& r, const std::string& as, const TypeTag& at)
        : Node(r), name(as), type(at) {}
};

struct Declaration : public Node {
    Declaration(const Range& r) : Node(r) {}
};

struct TokenDecl : public Declaration {
    typedef std::vector<std::shared_ptr<TokenDeclElement>> elements_type;

    elements_type elements;

    TokenDecl(const Range& r) : Declaration(r) {}
    TokenDecl(const Range& r, const elements_type& av)
        : Declaration(r), elements(av) {}
};

struct TokenPrefixDecl : public Declaration {
    std::string     prefix;

    TokenPrefixDecl(const Range& r, const std::string& as)
        : Declaration(r), prefix(as) {}
};

struct ExternalTokenDecl : public Declaration {
    ExternalTokenDecl(const Range& r) : Declaration(r) {}
};

struct AllowEBNF : public Declaration {
    AllowEBNF(const Range& r) : Declaration(r) {}
};

struct NamespaceDecl : public Declaration {
    std::string     name;

    NamespaceDecl(const Range& r, const std::string& as)
        : Declaration(r), name(as) {}
};

struct RecoverDecl : public Declaration {
    std::string     name;

    RecoverDecl(const Range& r, const std::string& as)
        : Declaration(r), name(as) {}
};

struct AccessModifierDecl : public Declaration {
    std::string     modifier;

    AccessModifierDecl(const Range& r, const std::string& as)
        : Declaration(r), modifier(as) {}
};

struct DontUseSTLDecl : public Declaration {
    DontUseSTLDecl(const Range& r) : Declaration(r) {}
};

struct Declarations : public Node {
    typedef std::vector<std::shared_ptr<Declaration>> declarations_type;

    declarations_type declarations;

    Declarations(const Range& r, const declarations_type& av)
        : Node(r), declarations(av) {}
};

struct Document : public Node {
    std::shared_ptr<Declarations>       declarations;
    std::shared_ptr<Rules>              rules;

    Document(const Range& r,
             const std::shared_ptr<Declarations>& ad,
             const std::shared_ptr<Rules>& ar)
        : Node(r), declarations(ad), rules(ar) {}
};

////////////////////////////////////////////////////////////////
// misc types
struct TargetTokenTraits {
    static int eof() { return 0; }
};

typedef zw::gr::package<Token, TokenTraits, Value>    cpg;
typedef zw::gr::package<int, TargetTokenTraits, int>  tgt;

struct GenerateOptions {
    bool            debug_parser    = false;
    std::string     token_prefix    = "token_";
    bool            external_token  = false;
    bool            allow_ebnf      = false;
    std::string     access_modifier = "";
    std::string     namespace_name  = "caper_parser";
    bool            dont_use_stl    = false;
    bool            recovery        = false;
    std::string     recovery_token  = "error";
};

struct Type {
    std::string name;
    Extension   extension   = Extension::None;

    Type(){}
    Type(const std::string& n, Extension e)
        : name(n), extension(e) {}
};

struct SemanticAction {
    struct Argument {
        int     source_index = -1;
        Type    type;

        Argument(){}
        Argument(int ai, const Type& at)
            : source_index(ai), type(at) {}
    };
    std::string             name;
    bool                    special;
    std::vector<Argument>   args;
    std::vector<int>        source_indices;

    SemanticAction() {}
    SemanticAction(const std::string& n, bool s) : name(n), special(s) {}
};

typedef std::map<tgt::rule, SemanticAction>     action_map_type;
typedef std::set<std::string>                   symbol_set_type;
typedef std::map<std::string, std::string>      symbol_map_type;

////////////////////////////////////////////////////////////////
// utility functions
template <class T>
std::shared_ptr<T> get_node(const value_type& v) {
    try {
        return std::static_pointer_cast<T>(boost::get<node_ptr>(v.data));
    }
    catch(boost::bad_get& x) {
        std::cerr << typeid(T).name() << std::endl;
        throw x;
    }
}

template <class T>
const std::string& get_symbol(const value_type& v) {
    try {
        return boost::get<T>(v.data).s;
    }
    catch(boost::bad_get& x) {
        std::cerr << typeid(T).name() << std::endl;
        throw x;
    }
}

template <class T, class U>
std::shared_ptr<T> downcast(U p) {
    return std::dynamic_pointer_cast<T>(p);
}

template <class T>
Range range(const T& x) {
    return Range(x[0].range.beg, x[x.size()-1].range.end);
}

#endif // CAPER_AST_HPP
