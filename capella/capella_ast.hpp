// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#ifndef CAPELLA_AST_HPP
#define CAPELLA_AST_HPP

#include <iostream>
#include <vector>
#include <set>
#include <boost/variant.hpp>
#include <sstream>

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
// BulkText
struct BulkText {
        std::string     s;
        
        BulkText(){}
        BulkText( const std::string& as ) : s( as ) {}
};

////////////////////////////////////////////////////////////////
// Reserved
struct Reserved {
        std::string     s;
        
        Reserved(){}
        Reserved( const std::string& as ) : s( as ) {}
};

////////////////////////////////////////////////////////////////
// AST nodes
struct Module;
struct BaseDef;
struct AtomDef;
struct TypeDef;
struct ClassHeaderDef;
struct ClassFooterDef;
struct ModuleHeaderDef;
struct ModuleFooterDef;
struct NamespaceDef;
struct Scalor;
struct List;
struct Variant;
struct Tuple;

typedef boost::variant<
        boost::recursive_wrapper< BaseDef >,
        boost::recursive_wrapper< AtomDef >,
        boost::recursive_wrapper< TypeDef >,
        boost::recursive_wrapper< ClassHeaderDef >,
        boost::recursive_wrapper< ClassFooterDef >,
        boost::recursive_wrapper< ModuleHeaderDef >,
        boost::recursive_wrapper< ModuleFooterDef >,
        boost::recursive_wrapper< NamespaceDef > >
        Declaration;

typedef boost::variant<
        Nil,
        boost::recursive_wrapper< Scalor >,
        boost::recursive_wrapper< List >,
        boost::recursive_wrapper< Variant >,
        boost::recursive_wrapper< Tuple > >
        TypeDefRight;

typedef boost::variant<
        boost::recursive_wrapper< Scalor >,
        boost::recursive_wrapper< List > >
        TupleItem;

struct Scalor {
        Identifier type;
        Identifier name;

        Scalor(){}
        Scalor( const Identifier& x, const Identifier& y ) : type( x ), name( y ) {}
};

struct Tuple {
        std::vector< TupleItem > elements;

        Tuple(){}
        Tuple( const std::vector< TupleItem >& x ) : elements( x ) {}
};

struct Variant {
        std::vector< Identifier > choises;

        Variant(){}
        Variant( const std::vector< Identifier >& x ) : choises( x ) {}
};

struct List {
        Identifier etype;
        Identifier name;

        List(){}
        List( const Identifier& x, const Identifier& y ) : etype( x ), name( y ) {}
};

struct ClassHeaderDef {
        BulkText data;

        ClassHeaderDef(){}
        ClassHeaderDef( const BulkText& x ) : data( x ) {}
};

struct ClassFooterDef {
        BulkText data;

        ClassFooterDef(){}
        ClassFooterDef( const BulkText& x ) : data( x ) {}
};

struct ModuleHeaderDef {
        BulkText data;

        ModuleHeaderDef(){}
        ModuleHeaderDef( const BulkText& x ) : data( x ) {}
};

struct ModuleFooterDef {
        BulkText data;

        ModuleFooterDef(){}
        ModuleFooterDef( const BulkText& x ) : data( x ) {}
};

struct NamespaceDef {
        Identifier name;

        NamespaceDef(){}
        NamespaceDef( const Identifier& x ) : name( x ) {}
};

struct TypeDef {
        Identifier      name;
        TypeDefRight    right;

        TypeDef(){}
        TypeDef( const Identifier& x, const TypeDefRight& y ) : name( x ), right( y ) {}
};

struct Atom {
        Identifier      name;
        Identifier      type;

        Atom(){}
        Atom( const Identifier& name, const Identifier& type ){}
};

struct AtomDef {
    std::vector<Atom>   atoms;

    AtomDef() {}
    template <class T>
    AtomDef(const T& x) {
        for (const auto& y: x) {
            atoms.push_back(y);
        }
    }
};

struct BaseDef {
    Identifier name;

    BaseDef() {}
    BaseDef(const Identifier& x) : name(x) {}
};

struct Module {
    std::vector<Declaration> declarations;

    Module() {}
    template <class T>
    Module(const T& x) {
        for (const auto& y: x) {
            declarations.push_back(y);
        }
    }
};

////////////////////////////////////////////////////////////////
// value_type
typedef boost::variant<
        Nil,
        Operator,
        Identifier,
        BulkText,
        Reserved,
        Module,
        Declaration,
        AtomDef,
        Atom,
        TypeDef,
        TypeDefRight,
        Scalor,
        List,
        Variant,
        Tuple,
        TupleItem > Node;

struct Value {
        typedef Node data_type;

        Range           range;
        data_type       data;

        Value() {}

        template < class T >
        Value( const Range& r, const T& d ) : range( r ), data( d ) {}
};

////////////////////////////////////////////////////////////////
// dictionary
typedef std::set< std::string > typeset_type;
typedef std::set< std::string > atomset_type;

////////////////////////////////////////////////////////////////
// errors
class capella_error : public std::exception {
public:
        capella_error( int a ) : addr( a ) {}
        
        int             addr;
};

class unexpected_char : public capella_error {
public:
        unexpected_char( int a, int c ) : capella_error(a), ch(c)
        {
                std::stringstream ss;
                ss << "unexpected char: '" << char(ch) << ",";
                message = ss.str();
        }
        ~unexpected_char() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        int             ch;
};
class mismatch_paren : public capella_error {
public:
        mismatch_paren( int a, int c ) : capella_error(a), ch(c)
        {
                std::stringstream ss;
                ss << "mismatch paren: '" << char(ch) << "'";
                message = ss.str();
        }
        ~mismatch_paren() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        int             ch;
};

class undefined_type : public capella_error {
public:
        undefined_type( const std::string& n ) : capella_error(-1), name(n)
        {
                std::stringstream ss;
                ss << "undefined type: \"" << name << "\"";
                message = ss.str();
        }
        ~undefined_type() throw () {}
        const char* what() const throw () { return message.c_str(); }

        std::string     message;
        std::string     name;
};


#endif // CAPELLA_AST_HPP
