#ifndef CALC1_AST_HPP
#define CALC1_AST_HPP

#include <boost/variant.hpp>
#include <iostream>

struct Add;
struct Sub;
struct Mul;
struct Div;

template < class OpTag > struct BinOpTerm;
template < class OpTag > struct BinOpExpr;

typedef boost::variant<
        int,
        boost::recursive_wrapper< BinOpTerm<Mul> >,
        boost::recursive_wrapper< BinOpTerm<Div> > >
        Term;
typedef boost::variant<
        Term,
        boost::recursive_wrapper< BinOpExpr<Add> >,
        boost::recursive_wrapper< BinOpExpr<Sub> > >
        Expr;

template < class OpTag >
struct BinOpTerm {
        Term    lhs;
        Term    rhs;
        BinOpTerm( const Term& x, const Term& y ) : lhs( x ), rhs( y ) {}
};

template < class OpTag >
struct BinOpExpr {
        Expr    lhs;
        Expr    rhs;
        BinOpExpr( const Expr& x, const Expr& y ) : lhs( x ), rhs( y ) {}
};

template <class T>
std::ostream& operator<<( std::ostream& os, const BinOpTerm<T>& x )
{
        os << x.lhs << " ? " << x.rhs;
        return os;
}
template <>
std::ostream& operator<< <Mul>( std::ostream& os, const BinOpTerm<Mul>& x )
{
        os << x.lhs << " * " << x.rhs;
        return os;
}
template <>
std::ostream& operator<< <Div>( std::ostream& os, const BinOpTerm<Div>& x )
{
        os << x.lhs << " / " << x.rhs;
        return os;
}


template <class T>
std::ostream& operator<<( std::ostream& os, const BinOpExpr<T>& x )
{
        os << x.lhs << " ? " << x.rhs;
        return os;
}
template <>
std::ostream& operator<< <Add>( std::ostream& os, const BinOpExpr<Add>& x )
{
        os << x.lhs << " + " << x.rhs;
        return os;
}
template <>
std::ostream& operator<< <Sub>( std::ostream& os, const BinOpExpr<Sub>& x )
{
        os << x.lhs << " - " << x.rhs;
        return os;
}

#endif // CALC1_AST_HPP
