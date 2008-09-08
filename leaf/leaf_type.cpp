// 2008/08/16 Naoyuki Hirayama

#include "leaf_type.hpp"
#include "leaf_error.hpp"
#include <cassert>

namespace leaf {

std::map< Type::FunSig, Type* >         Type::function_types_;
std::map< Type*, Type* >                Type::closure_types_;
std::map< std::vector< Type* >, Type* > Type::tuple_types_;

Type* Type::getVoidType()
{
    static Type* t = NULL;
    if( !t ) {
        t = getTupleType( std::vector< Type* >() );
    }
    return t;
}

Type* Type::getBoolType()
{
    static Type* t = NULL;
    if( !t ) { t = new Type( TAG_BOOL ); }
    return t;
}

Type* Type::getCharType()
{
    static Type* t = NULL;
    if( !t ) { t = new Type( TAG_CHAR ); }
    return t;
}

Type* Type::getShortType()
{
    static Type* t = NULL;
    if( !t ) { t = new Type( TAG_SHORT ); }
    return t;
}

Type* Type::getIntType()
{
    static Type* t = NULL;
    if( !t ) { t = new Type( TAG_INT ); }
    return t;
}

Type* Type::getLongType()
{
    static Type* t = NULL;
    if( !t ) { t = new Type( TAG_LONG ); }
    return t;
}

Type* Type::getFunctionType( Type* rtypes, Type* atypes )
{
    Type::FunSig s;
    s.rtypes = rtypes;
    s.atypes = atypes;

    std::map< FunSig, Type* >::const_iterator i = function_types_.find( s );
    if( i != function_types_.end() ) {
        return (*i).second;
    }
    
    return function_types_[s] = new Type( s );
}

Type* Type::getClosureType( Type* func )
{
    assert( isFunction( func ) );

    std::map< Type*, Type* >::const_iterator i =
        closure_types_.find( func );
    if( i != closure_types_.end() ) {
        return (*i).second;
    }
    return closure_types_[func] = new Type( func );
}

Type* Type::getTupleType( const std::vector< Type* >& elems )
{
    if( elems.size() == 1 ) {
        return elems[0];
    } else {
        std::map< std::vector< Type* >, Type* >::const_iterator i =
            tuple_types_.find( elems );
        if( i != tuple_types_.end() ) {
            return (*i).second;
        }
    
        return tuple_types_[elems] = new Type( elems );
    }   
}

Type* Type::getElementType( Type* t, int i )
{
    if( !t ) {
        return NULL;
    } else if( t->tag() == TAG_TUPLE ) {
        return t->getElement(i);
    } else {
        return t;
    }
}

Type* Type::getReturnType()
{
    switch( tag() ) {
    case TAG_FUNCTION:
        return funsig_.rtypes;
    case TAG_CLOSURE:
        return rawfunc_->funsig_.rtypes;
    default:
        assert(0);
    }
}

Type* Type::getArgumentType()
{
    switch( tag() ) {
    case TAG_FUNCTION:
        return funsig_.atypes;
    case TAG_CLOSURE:
        return rawfunc_->funsig_.atypes;
    default:
        assert(0);
    }
}

Type* Type::getElement( int index )
{
    if( tag() == TAG_TUPLE ) { 
        return elems_[index];
    } else {
        assert( index == 0 );
        return this;
    }
}

Type* Type::getRawFunc()
{
    assert( tag() == TAG_CLOSURE );
    return rawfunc_;
}

std::string Type::getDisplay( Type* t )
{
    if( !t ) {
        return "<*>";
    }
    switch( t->tag() ) {
    case TAG_BOOL: return "<bool>";
    case TAG_CHAR: return "<char>";
    case TAG_SHORT: return "<short>";
    case TAG_INT: return "<int>";
    case TAG_LONG: return "<long>";
    case TAG_TUPLE:
        if( t->elems_.empty() ) {
            return "<void>";
        } else if( t->elems_.size() == 1 ) {
            return getDisplay( t->elems_[0] );
        } else {
            std::string s = "<[";
            for( size_t i = 0 ; i < t->elems_.size() ; i++ ) {
                s += getDisplay( t->elems_[i] );
                if( i != t->elems_.size() - 1 ) {
                    s += ",";
                }
            }
        }
    case TAG_FUNCTION: return "<function>";
    case TAG_CLOSURE: return "<closure>";
    default:
        return "<UNKNOWN TYPE>";
    };
}

bool Type::isFunction( Type* t )
{
    return t && t->tag() == Type::TAG_FUNCTION;
}

bool Type::isClosure( Type* t )
{
    return t && t->tag() == Type::TAG_CLOSURE;
}

bool Type::isCallable( Type* t )
{
    return isFunction( t ) || isClosure( t );
}

bool Type::isComplete( Type* )
{
    assert(0);
    return false;
}

int Type::getTupleSize( Type* t )
{
    assert(t);
    if( t->tag() == TAG_TUPLE ) {
        return int( t->elems_.size() );
    } else {
        return 1;
    }
}

Type* Type::unify( Type* x, Type* y )
{
    if( x && !y ) { return x; }
    if( !x && y ) { return y; }
    if( !x && !y ) { return NULL; }

    int xn = getTupleSize( x );
    int yn = getTupleSize( y );
    if( xn != yn ) {
        throw type_mismatch(
            Addr(),
            Type::getDisplay( x ),
            Type::getDisplay( y ) );
    }

    if( xn == 1 ) {
        if( x != y ) {
            throw type_mismatch(
                Addr(),
                Type::getDisplay( x ),
                Type::getDisplay( y ) );
        }
        return x;
    }

    std::vector< Type* > v;
    for( int i = 0 ; i < xn ; i++ ) {
        Type* xe = getElementType( x, i );
        Type* ye = getElementType( y, i );
        v.push_back( unify( xe, ye ) );
    }
    return getTupleType( v );
}

Type* Type::normalize( Type* t )
{
    if( !t || t->tag() != TAG_TUPLE ) { return t; }
    if( t->elems_.empty() ) { return NULL; }
    if( t->elems_.size() == 1 ) {
        return normalize( t->elems_[0] );
    }
    return t;
}

} // namespace leaf
