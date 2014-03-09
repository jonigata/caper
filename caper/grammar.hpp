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

namespace zw {

namespace gr {

////////////////////////////////////////////////////////////////
// 前方参照

template < class Token, class Traits > class symbol;

/*============================================================================
 *
 * class intrusive_rc_ptr
 *
 * boost::instrusive_ptrと同じようなもの
 *
 *==========================================================================*/

template < class T >
class intrusive_rc_ptr {
public:
    intrusive_rc_ptr() : p_(NULL) {}
    intrusive_rc_ptr( T* p ) : p_( p ) { if( p_ ) { p_->addref(); } }
    intrusive_rc_ptr( const intrusive_rc_ptr& x ) : p_( x.p_ ) { if( p_ ) { p_->addref(); } }
    ~intrusive_rc_ptr() { if( p_ ) { p_->release(); } }

    void reset( T* p )
    {
        if( p_ ) { p_->release(); }
        p_ = p;
        if( p_ ) { p_->addref(); }
    }                

    intrusive_rc_ptr& operator=( const intrusive_rc_ptr& x )
    {
        reset( x.p_ );
        return *this;
    }

    bool operator==( const intrusive_rc_ptr& x ) const { return p_ == x.p_; }

    T* operator->() const { return p_; }
    T& operator*() const { return *p_; }

    bool empty() const { return !p_; }
    bool unique() const { if( !p_ ) { return false; } return p_->rccount() == 1; }

    T* get() const { return p_; }
        
private:
    T* p_;

};

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
public:
    nonterminal() {}
    nonterminal( const std::string& x ) : name_( x ) {}
    nonterminal( const nonterminal< Token, Traits >& x ) : name_( x.name_ ) {}
    ~nonterminal() {}

    const std::string& name() const { return name_; }

    nonterminal<Token,Traits>& operator=(const nonterminal<Token,Traits>& x)
    {
        name_ = x.name_;
        return *this;
    }

private:
    std::string name_;
        
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
    os << r.name_;
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
    symbol( const symbol< Token, Traits >& x ) : type_( x.type_ ), token_( x.token_ ), name_( x.name_ ) {}
    symbol( const epsilon< Token, Traits >& x ) : type_( type_epsilon ) {}
    symbol( const terminal< Token, Traits>& x ) : type_( type_terminal ), token_( x.token_ ), name_( x.display_ ){}
    symbol( const nonterminal< Token, Traits >& x ) : type_( type_nonterminal ), name_( x.name_ ) {}
    ~symbol(){}

    symbol< Token, Traits >& operator=( const symbol< Token, Traits >& x )
    {
        type_  = x.type_;
        token_ = x.token_;
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
        name_  = x.name_;
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
    std::string name() const        { assert( is_nonterminal() ); return name_; }

private:
    category_type   type_;
    Token           token_;
    std::string     name_;

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

        std::hash< std::string > str_hash;
        switch( s.type_ ) {
            case symbol_type::type_epsilon:      return 0x11111111;
            case symbol_type::type_terminal:     return std::size_t(s.token_);
            case symbol_type::type_nonterminal:  return str_hash(s.name_);
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
        case symbol_type::type_terminal:        return os << r.name_;
        case symbol_type::type_nonterminal:     return os << r.name_;
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
        nonterminal_type        left;
        elements_type           elements;

        rule_imp() { rc_count_ = 0; }
        rule_imp( const nonterminal_type& n ) : left(n) { rc_count_  = 0; }
        rule_imp( const nonterminal_type& n, const elements_type& e )
            : left(n), elements(e) { rc_count_ = 0; }

        int rc_count_;                
        void addref() { rc_count_++; }
        void release() { rc_count_--; if( !rc_count_ ) { delete this; } }
        int rccount() { return rc_count_; }
    };

    typedef intrusive_rc_ptr<rule_imp> imp_ptr;

public:
    rule(){}
    explicit rule( const nonterminal_type& x ) : imp( new rule_imp( x ) ) {}
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
    
    const nonterminal< Token, Traits >&     left() const  { return imp->left; }
    const elements_type&                    right() const { return imp->elements; }

private:
    void enunique()
    {
        if( !imp.unique() ) { imp.reset( new rule_imp( imp->left, imp->elements ) ); }
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

    typedef std::unordered_map<std::string, std::vector<rule_type> >        dictionary_type;
    typedef std::unordered_map<rule_type, int, rule_hash<Token, Traits> >   indices_type;

private:
    struct grammar_imp {
        rule_type       root;
        elements_type   elements;
        dictionary_type dictionary;
        indices_type    indices;

        grammar_imp() { rc_count_ = 0; }
        grammar_imp( const rule_type& x ) : root( x ) { rc_count_ = 0; elements.push_back( x ); }
        grammar_imp( const grammar_imp& x )
        : root(x.root), elements(x.elements), dictionary(x.dictionary), indices(x.indices) {
            rc_count_ = 0;
        }

        void add(const rule_type& x) {
            elements.push_back(x);
            dictionary[x.left().name()].push_back(x);
            indices[x] = elements.size() - 1;
        }

        void addref() { rc_count_++; }
        void release() { rc_count_--; if( !rc_count_ ) { delete this; } }
        int rccount() { return rc_count_; }
        int rc_count_;
    };

    typedef intrusive_rc_ptr< grammar_imp > imp_ptr;

public:
    explicit grammar( const rule<Token,Traits>& r ) : imp( new grammar_imp( r ) ) {}
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
    const indices_type& indices() const { return imp->indices; }

private:
    void enunique()
    {
        if( !imp.unique() ) { imp.reset( new grammar_imp( *imp ) ); }
    }

private:
    imp_ptr imp;

};

} // namespace gr

} // namespace zw

#endif
