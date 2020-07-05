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
#include <unordered_set>
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
    epsilon(const epsilon<Token, Traits>&) {}

    epsilon<Token, Traits>& operator=(const epsilon<Token, Traits>&) {
        return *this;
    }
    
private:
    friend class symbol<Token, Traits>;
};

template <class Token,class Traits>
std::ostream& operator<<(std::ostream& os, const epsilon<Token, Traits>&) {
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

template <class Token, class Traits>
bool operator ==(const terminal<Token, Traits>& x,
                 const terminal<Token, Traits>& y);

template <class Token, class Traits>
std::ostream& operator<<(std::ostream&, const terminal<Token, Traits>&);

template <class Token,class Traits >
class terminal {
public:
    terminal(): token_(Traits::eof()) {}
    terminal(const std::string& d, const Token& t) : display_(d), token_(t) {}
    terminal(const terminal<Token, Traits>& x)
        : display_(x.display_), token_(x.token_) {}
    
    terminal<Token, Traits>& operator=(const terminal<Token, Traits>& x) {
        display_ = x.display_;
        token_ = x.token_;
        return *this;
    }
    
    Token token() const { return token_; }

    int cmp(const terminal<Token, Traits>& y) const {
        return token_ - y.token_;
    }

private:
    std::string     display_;
    Token           token_;

    friend bool operator== <>(const terminal<Token, Traits>& x,
                              const terminal<Token, Traits>& y);

    friend std::ostream& operator<< <>(
        std::ostream& os, const terminal<Token, Traits>& r);
        
    friend class symbol<Token, Traits>;

public:
    struct hash {
        size_t operator()(const terminal< Token, Traits >& s) const {
            return size_t(s.token_);
        }
    };
    
};

template <class Token, class Traits>
bool operator==(const terminal<Token, Traits>& x,
                const terminal<Token, Traits>& y) {
    return x.token_ == y.token_;
}

template <class Token,class Traits>
std::ostream& operator<<(std::ostream& os, const terminal< Token, Traits >& r) {
    os << r.display_;
    return os;
}

template <class Token, class Traits>
class terminal_set : public std::unordered_set<terminal<Token, Traits>, typename terminal<Token, Traits>::hash> {
};

/*============================================================================
 *
 * class nonterminal
 *
 * 非終端記号
 *
 *==========================================================================*/

template <class Token, class Traits> class symbol;

template <class Token, class Traits> class nonterminal;

template <class Token, class Traits>
bool operator ==(const nonterminal<Token, Traits>& x,
                 const nonterminal<Token, Traits>& y);

template <class Token, class Traits>
bool operator<(const nonterminal<Token, Traits>& x,
               const nonterminal<Token, Traits>& y);

template <class Token, class Traits>
std::ostream& operator<<(
    std::ostream& os, const nonterminal<Token, Traits>& r);

template <class Token, class Traits>
class nonterminal {
private:
    static const std::string* intern(const std::string& s) {
        static std::set<std::string> env;
        return &(*(env.insert(s).first));
    }

public:
    nonterminal() {}
    explicit nonterminal(const std::string& x) : name_(intern(x)) {}
    explicit nonterminal(const std::string* n) : name_(n) {}
    nonterminal(const nonterminal<Token, Traits>& x) : name_(x.name_) {}

    const std::string& name() const { return *name_; }
    const std::string* identity() const { return name_; }

    nonterminal<Token,Traits>& operator=(const nonterminal<Token, Traits>& x) {
        name_ = x.name_;
        return *this;
    }

    int cmp(const nonterminal<Token, Traits>& y) const {
        return name_ - y.name_;
    }

private:
    const std::string* name_;
        
    friend bool operator== <>(const nonterminal<Token, Traits>& x,
                              const nonterminal<Token, Traits>& y);
    friend bool operator< <>(const nonterminal<Token, Traits>& x,
                             const nonterminal<Token, Traits>& y);

    friend std::ostream& operator<< <>(
        std::ostream&, const nonterminal<Token, Traits>& y);

    friend class symbol< Token, Traits >;

public:
    struct hash {
        size_t
        operator()(const nonterminal<Token, Traits>& s) const {
            return reinterpret_cast<size_t>(s.name_);
        }
    };

};

template <class Token, class Traits>
bool operator<(const nonterminal<Token, Traits>& x,
               const nonterminal<Token, Traits>& y) {
    return x.cmp(y) < 0;
}

template <class Token, class Traits>
bool operator ==(const nonterminal<Token, Traits>& x,
                 const nonterminal<Token, Traits>& y) {
    return x.cmp(y) == 0;
}

template <class Token, class Traits>
std::ostream& operator<<(
    std::ostream& os, const nonterminal<Token, Traits>& r) {
    os << r.name();
    return os;
}

template <class Token, class Traits>
class nonterminal_set :
        public std::unordered_set<nonterminal<Token, Traits>,
                                  typename nonterminal<Token, Traits>::hash> {
    
};

/*============================================================================
 *
 * class symbol
 *
 * terminal | nonterminal | epsilon
 *
 *==========================================================================*/

template <class Token,class Traits > class rule;

template <class Token, class Traits>
bool operator==(const symbol<Token, Traits>& x,
                const symbol<Token, Traits>& y);

template <class Token, class Traits>
bool operator<(const symbol<Token, Traits>& x,
               const symbol<Token, Traits>& y);

template <class Token, class Traits>
std::ostream& operator<<(std::ostream& os, const symbol<Token, Traits>& r);

template <class Token, class Traits>
class symbol {
private:
    enum category_type {
        type_epsilon,
        type_terminal,
        type_nonterminal,
    };

public:
    symbol() : type_( type_epsilon ) {}
    symbol(const symbol<Token, Traits>& x)
        : type_(x.type_),
          token_(x.token_), display_(x.display_), name_(x.name_) {}
    symbol(const epsilon<Token, Traits>&) : type_(type_epsilon) {}
    symbol(const terminal<Token, Traits>& x)
        : type_(type_terminal), token_(x.token_), display_(x.display_) {}
    symbol(const nonterminal<Token, Traits>& x)
        : type_(type_nonterminal), name_(x.name_) {}

    symbol<Token, Traits>& operator=(const symbol<Token, Traits>& x) {
        type_ = x.type_;
        token_ = x.token_;
        display_ = x.display_;
        name_ = x.name_;
        return *this;
    }
    symbol<Token, Traits>& operator=(const epsilon<Token, Traits>&) {
        type_ = type_epsilon;
        return *this;
    }
    symbol<Token, Traits>& operator=(const terminal<Token, Traits>& x) {
        type_ = type_terminal;
        token_ = x.token_;
        display_ = x.display_;
        return *this;
    }
    symbol<Token, Traits>& operator=(const nonterminal<Token, Traits>& x) {
        type_ = type_nonterminal;
        name_ = x.name_;
        return *this;
    }
        
    bool is_epsilon() const         { return type_ == type_epsilon; }
    bool is_terminal() const        { return type_ == type_terminal; }
    bool is_nonterminal() const     { return type_ == type_nonterminal; }
    terminal<Token, Traits> as_terminal() const {
        assert(is_terminal());
        return terminal<Token, Traits>(display_, token_);
    }
    nonterminal<Token, Traits> as_nonterminal() const {
        assert(is_nonterminal());
        return nonterminal<Token, Traits>(name_);
    }
    Token token() const {
        assert(is_terminal());
        return token_;
    }
    const std::string& display() const {
        assert(is_terminal());
        return display_;
    }
    const std::string& name() const {
        assert(is_nonterminal());
        return *name_;
    }
    const std::string* identity() const {
        assert(is_nonterminal());
        return name_;
    }

    int cmp(const symbol<Token, Traits>& y) const {
        typedef symbol<Token, Traits> symbol_type;

        if (type_ != y.type_) { return type_ - y.type_; }
        switch (type_) {
            case symbol_type::type_epsilon:      return 0;
            case symbol_type::type_terminal:     return token_ - y.token_;
            case symbol_type::type_nonterminal:  return name_ - y.name_;
            default: assert(0);     return 0;
        }
    }

private:
    category_type       type_;
    Token               token_;
    std::string         display_;
    const std::string*  name_;

    friend class rule<Token, Traits>;
    friend bool operator== <>(const symbol<Token, Traits>& x,
                              const symbol<Token, Traits>& y);
    friend bool operator< <>(const symbol<Token, Traits>& x,
                             const symbol<Token, Traits>& y);
    friend std::ostream& operator<< <>(
        std::ostream& os, const symbol<Token, Traits>& r);

public:
    struct hash {
        size_t operator()(const symbol<Token, Traits>& s) const {
            switch (s.type_) {
                case type_epsilon:      return 0x11111111;
                case type_terminal:     return size_t(s.token_);
                case type_nonterminal:
                    return reinterpret_cast<size_t>(s.name_);
                default: assert(0);     return false;
            }
        }
    };

};

template <class Token, class Traits> inline
bool operator<(const symbol<Token, Traits>& x, const symbol<Token, Traits>& y) {
    return x.cmp(y) < 0;
}

template < class Token, class Traits > inline
bool operator==(const symbol<Token, Traits>& x,
                const symbol<Token, Traits>& y) {
    return x.cmp(y) == 0;
}

template <class Token, class Traits>
std::ostream& operator<<(std::ostream& os, const symbol<Token, Traits>& r) {
    typedef symbol<Token, Traits> symbol_type;

    switch (r.type_) {
        case symbol_type::type_epsilon:         return os << " { e }";
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

template <class Token, class Traits> class grammar;

template <class Token, class Traits> class rule;

template <class Token, class Traits>
bool operator ==(const rule<Token, Traits>& x,
                 const rule<Token, Traits>& y);


template <class Token, class Traits>
bool operator<(const rule<Token, Traits>& x,
               const rule<Token, Traits>& y);

template <class Token, class Traits>
class rule {
public:
    typedef nonterminal<Token, Traits>          nonterminal_type;
    typedef std::vector<symbol<Token, Traits>>  elements_type;

private:
    struct rule_imp {
        nonterminal_type    left;
        elements_type       elements;
        size_t              id = size_t(-1);

        rule_imp() {}
        rule_imp(const nonterminal_type& n) : left(n) {}
        rule_imp(const rule_imp& r)
        : left(r.left), elements(r.elements), id(r.id) {}
    };

    typedef std::shared_ptr<rule_imp> imp_ptr;

public:
    rule(){}
    explicit rule(const nonterminal_type& x)
    : imp(std::make_shared<rule_imp>(x)) {}
    rule(const rule<Token, Traits>& x) : imp(x.imp) {}

    rule<Token, Traits>& operator=(const rule<Token, Traits>& x) {
        imp = x.imp;
        return *this;
    }

    rule<Token, Traits>& operator<<(const symbol<Token, Traits>& s) {
        enunique();
        imp->elements.push_back(s);
        return *this;
    }
    
    void stamp(size_t id) {
        enunique();
        imp->id = id;
    }

    size_t id() const { return imp->id; }

    const nonterminal< Token, Traits >& left() const  {
        return imp->left;
    }
    const elements_type&                right() const {
        return imp->elements;
    }

private:
    void enunique() {
        if (!imp.unique()) { imp = std::make_shared<rule_imp>(*imp); }
    }

private:
    imp_ptr imp;
    
    friend bool operator== <>(const rule<Token, Traits>& x,
                              const rule<Token, Traits>& y);
    friend bool operator< <>(const rule<Token, Traits>& x,
                             const rule<Token, Traits>& y);

public:
    struct hash {
        size_t operator()(const rule<Token, Traits>& s) const {
            // large prime numbers
            const int p1 = 73856093;
            const int p2 = 19349663;
            //const int p3 = 83492791;
            
            typename elements_type::value_type::hash h2;
            int n = 0;
            for (const auto& x: s.right()) {
                n += h2(x);
            }
            
            typename nonterminal_type::hash h1;
            return h1(s.left()) * p1 + n * p2;
        }
    };

};

template <class Token,class Traits> inline
bool operator ==(const rule<Token, Traits>& x, const rule<Token, Traits>& y) {
    if (x.imp == y.imp) { return true; }
    if (!(x.left() == y.left())) { return false; }
    return x.right() == y.right();
}

template <class Token,class Traits> inline
bool operator<(const rule<Token, Traits>& x, const rule<Token, Traits>& y) {
    if (x.imp == y.imp) { return false; }
    if (x.left() == y.left()) {
        return x.right()<y.right();
    } else {
        return x.left()<y.left();
    }
}

template <class Token,class Traits>
std::ostream& operator<<(std::ostream& os, const rule<Token, Traits>& r) {
    os << r.left()<< " ::= ";
    for (const auto& x: r.right()) {
        os << x << " ";
    }
    return os;
}

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
    typedef zw::gr::rule<Token, Traits>             rule_type;
    typedef typename std::vector<rule_type>         elements_type;
    typedef typename elements_type::iterator        iterator;
    typedef typename elements_type::const_iterator  const_iterator;
    typedef typename elements_type::reference       reference;
    typedef typename elements_type::const_reference const_reference;

    typedef std::unordered_map<const std::string*, std::vector<rule_type>>
        dictionary_type;

private:
    struct grammar_imp {
        elements_type   elements;   // 0 = root
        dictionary_type dictionary;

        grammar_imp() {} 
        grammar_imp(const rule_type& x)
            : elements { x } {}
        grammar_imp(const grammar_imp& x)
            : elements(x.elements), dictionary(x.dictionary) {}

        void add(const rule_type& x) {
            rule_type y(x);
            y.stamp(elements.size());
            elements.push_back(y);
            dictionary[x.left().identity()].push_back(y);
        }
    };

    typedef std::shared_ptr<grammar_imp> imp_ptr;

public:
    grammar()
        : imp(std::make_shared<grammar_imp>()){}
    explicit grammar(const rule<Token, Traits>& r)
        : imp(std::make_shared<grammar_imp>(r)){}
    grammar(const grammar& x) : imp(x.imp) {}

    grammar& operator=(const grammar& x) {
        imp = x.imp;
        return *this;
    }

    grammar& operator<<(const rule_type& r) {
        enunique();
        assert(!exists(r));
        imp->add(r);
        return *this;
    }

    const_iterator begin()const { return imp->elements.begin(); }
    const_iterator end()  const { return imp->elements.end(); }
    size_t size() const { return imp->elements.size(); }
    const rule_type& at(size_t n) const { return imp->elements[n]; }

    rule_type root_rule() const { return imp->elements[0]; }

    const dictionary_type& dictionary() const { return imp->dictionary; }

    bool exists(const rule_type& rule) const {
        return
            std::find(imp->elements.begin(),
                      imp->elements.end(), rule) !=
            imp->elements.end();
    }

private:
    void enunique() {
        if (!imp.unique()) { imp = std::make_shared<grammar_imp>(*imp); }
    }

private:
    imp_ptr imp;

};

} // namespace gr

} // namespace zw

#endif
