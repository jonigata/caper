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
    if( !t ) { t = getTupleType( std::vector< Type* >() ); }
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

    std::map< Type*, Type* >::const_iterator i = closure_types_.find( func );
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

Type* Type::getStructType( const std::vector< Slot >& slots )
{
	// struct ÇÕíËã`èÍèäÇ™à·Ç¶ÇŒï ÇÃå^Ç…Ç»ÇÈ
	return new Type( slots );
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

int Type::getSlotCount()
{
    assert( tag() == TAG_STRUCT );
	return int( slots_.size() );
}

int Type::getSlotIndex( Symbol* s )
{
    assert( tag() == TAG_STRUCT );

	for( size_t i = 0 ; i < slots_.size() ; i++ ) {
		if( slots_[i].name == s ) {
			return int(i);
		}
	}
	return -1;
}

const Slot& Type::getSlot( int i )
{
	return slots_[i];
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
    case TAG_FUNCTION:
		{
			std::string s = "<fun (";
			Type* p = t->getArgumentType();
			for( int i = 0 ; i < getTupleSize( p ) ; i++ ) {
				s += getDisplay( getElementType( p, i ) );
				if( i < getTupleSize(p) -1 ) {
					s += ",";
				}
			}
			s += "): ";
			p = t->getReturnType();
			for( int i = 0 ; i < getTupleSize( p ) ; i++ ) {
				s += getDisplay( getElementType( p, i ) );
				if( i < getTupleSize(p) -1 ) {
					s += ",";
				}
			}
			return s + ">";
		}
		return "<function>";
    case TAG_CLOSURE: return "<closure>";
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
			return s + "]>";
        }
    case TAG_STRUCT:
        {
            std::string s = "<{";
            for( size_t i = 0 ; i < t->slots_.size() ; i++ ) {
                s += t->slots_[i].name->s + ":" + 
					getDisplay( t->slots_[i].type );
                if( i != t->slots_.size() - 1 ) {
                    s += ",";
                }
            }
			return s + "}>";
        }
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

bool Type::isStruct( Type* t )
{
    return t && t->tag() == Type::TAG_STRUCT;
}

bool Type::isCallable( Type* t )
{
    return isFunction( t ) || isClosure( t );
}

bool Type::isComplete( Type* t )
{
	if( !t ) { return false; }

    switch( t->tag() ) {
    case TAG_FUNCTION:
    case TAG_CLOSURE:
		return
			isComplete( t->getArgumentType() ) &&
			isComplete( t->getReturnType() );
	case TAG_TUPLE:
		{
			int n = getTupleSize( t );
			for( int i = 0 ; i < n ; i++ ) {
				if( !isComplete( getElementType( t, i ) ) ) {
					return false;
				}
			}
			return true;
		}
    default:
		return true;
    }
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

bool Type::match( Type* x, Type* y )
{
	if( !x || !y ) { // Ç«ÇøÇÁÇ©Ç™any
		return true;
	}

	if( x->tag() != y->tag() ) {
		return false;
	}

	if( x->tag() == TAG_TUPLE ) {
		int n = getTupleSize( x );
		if( n != getTupleSize( y ) ) {
			return false;
		}
		for( int i = 0 ; i < n ; i++ ) {
			if( !match( getElementType( x, i ), getElementType( y, i ) ) ) {
				return false;
			}
		}
		return true;
	} else if( x->tag() == TAG_FUNCTION || x->tag() == TAG_CLOSURE ) {
		return
			match( x->getArgumentType(), y->getArgumentType() ) &&
			match( x->getReturnType(), y->getReturnType() );
	} else {
		// íPèÉå^ÇæÇ¡ÇΩÇÁäÆëSàÍívÇ™ïKóv
		return x == y;
	}
}

} // namespace leaf
