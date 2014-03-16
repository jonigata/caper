// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#if !defined(ZW_GRAMMAR_HPP)
#define ZW_GRAMMAR_HPP

// module: grammar
//   BNF文法サポートモジュール

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <memory>
#include <set>
#include <unordered_map>

namespace zw {

namespace gr {

////////////////////////////////////////////////////////////////
// 前方参照

template < class Token, class Traits > class symbol;

/*============================================================================
 *
 * class epsilon
 *
 * 空列
 *
 *==========================================================================*/

template <class Token,class Traits >
class epsilon {
public:
    epsilon(){}
    epsilon( const epsilon< Token, Traits >& x ){}
    ~epsilon(){}

    epsilon<Token,Traits>& operator=(const epsilon<Token,Traits>& x) { return *this; }
    
private:
    friend class symbol< Token, Traits >;
};

template <class Token,class Traits>
std::ostream& operator<<( std::ostream& os, const epsilon< Token, Traits >& r )
{
    os << "{}";
    return os;
}

/*============================================================================
 *
 * class terminal
 *
 * 終端記号（tokenのホルダ）
 *
 *==========================================================================*/

template <class Token,class Traits > class terminal;

template < class Token, class Traits >
std::ostream& operator<<( std::ostream&, const terminal< Token, Traits >& );

template <class Token,class Traits >
class terminal {
public:
    terminal() : token_( Traits::eof() ) {}
    terminal( const std::string& d, const Token& t ) : display_(d), token_(t) {}
    terminal( const terminal< Token, Traits >& x ) : display_( x.display_ ), token_( x.token_ ) {}
    ~terminal() {}

    terminal< Token, Traits >& operator=( const terminal< Token, Traits >& x )
    {
        display_ = x.display_;
        token_ = x.token_;
        return *this;
    }

private:
    std::string     display_;
    Token           token_;

    friend std::ostream& operator<< <>( std::ostream& os, const terminal< Token, Traits >& r );
        
    friend class symbol< Token, Traits >;
};

template <class Token,class Traits>
std::ostream& operator<<( std::ostream& os, const terminal< Token, Traits >& r )
{
    os << r.display_;
    return os;
}

/*============================================================================
 *
 * class nonterminal
 *
 * 非終端記号
 *
 *==========================================================================*/

template < class Token, class Traits > class symbol;

template < class Token, class Traits > class nonterminal;

template < class Token, class Traits >
bool operator==( const nonterminal< Token, Traits >& x, 
                 const nonterminal< Token, Traits >& y );

template < class Token, class Traits >
bool operator<( const nonterminal< Token, Traits >& x, 
                const nonterminal< Token, Traits >& y );

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const nonterminal< Token, Traits >& r );

template < class Token, class Traits >
class nonterminal {
private:
    static const std::string* intern(const std::string& s) {
        static std::set<std::string> env;
        return &(*(env.insert(s).first));
    }

public:
    nonterminal() {}
    nonterminal( const std::string& x ) : name_( intern(x) ) {}
    nonterminal( const nonterminal< Token, Traits >& x ) : name_( x.name_ ) {}
    ~nonterminal() {}

    const std::string& name() const { return *name_; }
    const std::string* identity() const { return name_; }

    nonterminal<Token,Traits>& operator=(const nonterminal<Token,Traits>& x)
    {
        name_ = x.name_;
        return *this;
    }

private:
    const std::string* name_;
        
    friend bool operator== <>( const nonterminal< Token, Traits >& x,
                               const nonterminal< Token, Traits >& y );
    friend bool operator< <>( const nonterminal< Token, Traits >& x,
                              const nonterminal< Token, Traits >& y );
    friend std::ostream& operator<< <>( std::ostream&, const nonterminal< Token, Traits >& y );

    friend class symbol< Token, Traits >;
};

template < class Token, class Traits >
bool operator==( const nonterminal< Token, Traits >& x, const nonterminal< Token, Traits >& y )
{
    return x.name_ == y.name_;
}

template < class Token, class Traits >
bool operator<( const nonterminal< Token, Traits >& x, const nonterminal< Token, Traits >& y )
{
    return x.name_ < y.name_;
}

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const nonterminal< Token, Traits >& r )
{
    os << r.name();
    return os;
}

/*============================================================================
 *
 * class symbol
 *
 * terminal | nonterminal | epsilon
 *
 *==========================================================================*/

template <class Token,class Traits > class rule;

template < class Token, class Traits >
bool operator==( const symbol< Token, Traits >& x, const symbol< Token, Traits >& y );

template < class Token, class Traits >
bool operator<( const symbol< Token, Traits >& x, const symbol< Token, Traits >& y );

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const symbol< Token, Traits >& r );

template <class Token, class Traits >
struct symbol_hash;

template <class Token, class Traits >
class symbol {
private:
    enum category_type {
        type_epsilon,
        type_terminal,
        type_nonterminal,
    };

public:
    symbol() : type_( type_epsilon ) {}
    symbol( const symbol< Token, Traits >& x ) : type_( x.type_ ), token_( x.token_ ), display_(x.display_), name_( x.name_ ) {}
    symbol( const epsilon< Token, Traits >& x ) : type_( type_epsilon ) {}
    symbol( const terminal< Token, Traits>& x ) : type_( type_terminal ), token_( x.token_ ), display_( x.display_ ){}
    symbol( const nonterminal< Token, Traits >& x ) : type_( type_nonterminal ), name_( x.name_ ) {}
    ~symbol(){}

    symbol< Token, Traits >& operator=( const symbol< Token, Traits >& x )
    {
        type_  = x.type_;
        token_ = x.token_;
        display_ = x.display_;
        name_  = x.name_;
        return *this;
    }
    symbol< Token, Traits >& operator=( const epsilon< Token, Traits >& x )
    {
        type_  = type_epsilon;
        return *this;
    }
    symbol< Token, Traits >& operator=( const terminal< Token, Traits >& x )
    {
        type_  = type_terminal;
        token_ = x.token_;
        display_ = x.display_;
        return *this;
    }
    symbol< Token, Traits >& operator=( const nonterminal< Token, Traits >& x )
    {
        type_  = type_nonterminal;
        name_  = x.name_;
        return *this;
    }
        
    bool is_epsilon() const         { return type_ == type_epsilon; }
    bool is_terminal() const        { return type_ == type_terminal; }
    bool is_nonterminal() const     { return type_ == type_nonterminal; }
    Token token() const             { assert( is_terminal() ); return token_; }
    const std::string& display() const { assert( is_terminal() ); return display_; }
    const std::string& name() const { assert( is_nonterminal() ); return *name_; }
    const std::string* identity() const { assert( is_nonterminal() ); return name_; }

private:
    category_type       type_;
    Token               token_;
    std::string         display_;
    const std::string*  name_;

    friend class rule< Token, Traits >;
    friend bool operator== <>( const symbol< Token, Traits >& x, 
                               const symbol< Token, Traits >& y );
    friend bool operator< <>( const symbol< Token, Traits >& x, 
                              const symbol< Token, Traits >& y );
    friend std::ostream& operator<< <>( std::ostream& os, const symbol< Token, Traits >& r );
    friend struct symbol_hash< Token, Traits >;
};

template <class Token, class Traits >
struct symbol_hash
{
    std::size_t operator()(const symbol< Token, Traits >& s) const
    {
        typedef symbol< Token, Traits > symbol_type;

        switch( s.type_ ) {
            case symbol_type::type_epsilon:      return 0x11111111;
            case symbol_type::type_terminal:     return std::size_t(s.token_);
            case symbol_type::type_nonterminal:  return reinterpret_cast<std::size_t>(s.name_);
            default: assert(0);     return false;
        }
    }
};

template < class Token, class Traits > inline
bool operator==( const symbol< Token, Traits >& x, const symbol< Token, Traits >& y )
{
    typedef symbol< Token, Traits > symbol_type;

    if( x.type_ != y.type_ ) { return false; }
    switch( x.type_ ) {
        case symbol_type::type_epsilon:      return true;
        case symbol_type::type_terminal:     return x.token_ == y.token_;
        case symbol_type::type_nonterminal:  return x.name_ == y.name_;
        default: assert(0);     return false;
    }
}

template < class Token, class Traits > inline
bool operator<( const symbol< Token, Traits >& x, const symbol< Token, Traits >& y )
{
    typedef symbol< Token, Traits > symbol_type;

    if( x.type_ < y.type_ ) { return true; }
    if( y.type_ < x.type_ ) { return false; }
    switch( x.type_ ) {
        case symbol_type::type_epsilon:      return false;
        case symbol_type::type_terminal:     return x.token_ < y.token_;
        case symbol_type::type_nonterminal:  return x.name_ < y.name_;
        default: assert(0);     return false;
    }
}

template < class Token, class Traits >
std::ostream& operator<<( std::ostream& os, const symbol< Token, Traits >& r )
{
    typedef symbol< Token, Traits > symbol_type;

    switch( r.type_ ) {
        case symbol_type::type_epsilon:         return os << "{e}";
        case symbol_type::type_terminal:        return os << r.display();
        case symbol_type::type_nonterminal:     return os << r.name();
        default: assert(0);                     return os;
    }
}

/*============================================================================
 *
 * class rule
 *
 * 文法規則（|は使えないので、ruleを複数作ること）
 *
 *==========================================================================*/

template < class Token, class Traits > class grammar;

template < class Token, class Traits > class rule;

template <class Token, class Traits > struct rule_hash;

template < class Token, class Traits >
bool operator==( const rule< Token, Traits >& x, 
                 const rule< Token, Traits >& y );


template < class Token, class Traits >
bool operator<( const rule< Token, Traits >& x, 
                const rule< Token, Traits >& y );

template < class Token, class Traits >
class rule {
public:
    typedef nonterminal< Token, Traits >            nonterminal_type;
    typedef std::vector< symbol< Token, Traits > >  elements_type;

private:
    struct rule_imp {
        nonterminal_type    left;
        elements_type       elements;
        std::size_t         id;

        rule_imp() { id = std::size_t(-1); }
        rule_imp( const nonterminal_type& n ) : left(n) { id = std::size_t(-1); }
        rule_imp( const rule_imp& r )
            : left(r.left), elements(r.elements), id(r.id) { }
    };

    typedef std::shared_ptr<rule_imp> imp_ptr;

public:
    rule(){}
    explicit rule( const nonterminal_type& x ) : imp( std::make_shared<rule_imp>( x ) ) {}
    rule( const rule< Token, Traits >& x ) : imp( x.imp ) {}
    ~rule(){}

    rule< Token, Traits >& operator=( const rule< Token, Traits >& x )
    {
        imp = x.imp;
        return *this;
    }

    rule<Token,Traits>& operator<<( const symbol<Token,Traits>& s )
    {
        enunique();
        imp->elements.push_back( s );
        return *this;
    }
    
    void stamp(std::size_t id) {
        enunique();
        imp->id = id;
    }

    std::size_t id() const { return imp->id; }

    const nonterminal< Token, Traits >&     left() const  { return imp->left; }
    const elements_type&                    right() const { return imp->elements; }

private:
    void enunique()
    {
        if( !imp.unique() ) { imp = std::make_shared<rule_imp>( *imp ); }
    }

private:
    imp_ptr imp;
    
    friend struct rule_hash< Token, Traits >;
    friend bool operator== <>( const rule<Token,Traits>& x,
                               const rule<Token,Traits>& y );
    friend bool operator< <>( const rule<Token,Traits>& x,
                              const rule<Token,Traits>& y );

};

template <class Token,class Traits> inline
bool operator==( const rule<Token,Traits>& x, const rule<Token,Traits>& y )
{
    if( x.imp == y.imp ) { return true; }
    if( !( x.left() == y.left() ) ) { return false; }
    return x.right() == y.right();
} 

template <class Token,class Traits> inline
bool operator<( const rule<Token,Traits>& x, const rule<Token,Traits>& y )
{
    if( x.imp == y.imp ) { return false; }
    if( x.left() == y.left() ) {
        return x.right() < y.right();
    } else {
        return x.left() < y.left();
    }
}

template <class Token,class Traits>
std::ostream& operator<<(std::ostream& os,const rule<Token,Traits>& r)
{
    typedef rule< Token, Traits > rule_type;
    os << r.left() << " ::= ";
    for( typename rule_type::elements_type::const_iterator i = r.right().begin() ;
         i != r.right().end() ;
         ++i ) {
        os << (*i) << " ";
    }
    return os;
}

template <class Token, class Traits >
struct rule_hash {
    std::size_t operator()(const rule< Token, Traits >& s) const {
        return reinterpret_cast<std::size_t>(s.imp.get());
    }
};

/*============================================================================
 *
 * class opgroup
 *
 * 演算子の集合
 *
 *==========================================================================*/

template <class Token,class Traits >
class opgroup {
public:
    opgroup(){}
    ~opgroup(){}

    opgroup& operator<<(symbol<Token,Traits>&);
};

/*============================================================================
 *
 * class grammar
 *
 * 文法規則の集合
 *
 *==========================================================================*/

template <class Token,class Traits>
class grammar {
public:
    typedef zw::gr::rule< Token, Traits >           rule_type;
    typedef typename std::vector< rule_type >       elements_type;
    typedef typename elements_type::iterator        iterator;
    typedef typename elements_type::const_iterator  const_iterator;
    typedef typename elements_type::reference       reference;
    typedef typename elements_type::const_reference const_reference;

    typedef std::unordered_map<const std::string*, std::vector<rule_type> > dictionary_type;

private:
    struct grammar_imp {
        rule_type       root;
        elements_type   elements;
        dictionary_type dictionary;

        grammar_imp() { }
        grammar_imp( const rule_type& x ) : root( x ) { elements.push_back( x ); }
        grammar_imp( const grammar_imp& x )
        : root(x.root), elements(x.elements), dictionary(x.dictionary) {}

        void add(const rule_type& x) {
            rule_type y(x);
            y.stamp(elements.size());
            elements.push_back(y);
            dictionary[x.left().identity()].push_back(y);
        }
    };

    typedef std::shared_ptr< grammar_imp > imp_ptr;

public:
    explicit grammar( const rule<Token,Traits>& r ) : imp( std::make_shared<grammar_imp>( r ) ) {}
    grammar( const grammar& x ) : imp( x.imp ) {}
    ~grammar(){}

    grammar& operator=( const grammar& x )
    {
        imp = x.imp;
        return *this;
    }

    grammar& operator<<( const opgroup<Token,Traits>& )
    {
        enunique();
        return *this;
    }
    grammar& operator<<( const rule_type& r )
    {
        enunique();
        assert( std::find( imp->elements.begin(), imp->elements.end(), r ) == imp->elements.end() );
        imp->add( r );
        return *this;
    }

    const_iterator begin()const { return imp->elements.begin(); }
    const_iterator end()  const { return imp->elements.end(); }
    size_t size() const { return imp->elements.size(); }

    rule_type root_rule() const { return imp->root; }

    int rule_index( const rule<Token,Traits>& r ) const
    {
        typename elements_type::const_iterator i =
            std::find( imp->elements.begin(), imp->elements.end(), r );
        if( i == imp->elements.end() ) {
            return -1;
        }
        return int( i - imp->elements.begin() );
    }

    const dictionary_type& dictionary() const { return imp->dictionary; }

private:
    void enunique()
    {
        if( !imp.unique() ) { imp = std::make_shared<grammar_imp>( *imp ); }
    }

private:
    imp_ptr imp;

};

} // namespace gr

} // namespace zw

#endif
