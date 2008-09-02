// 2008/08/16 Naoyuki Hirayama

#include "leaf_type.hpp"
#include <cassert>

namespace leaf {

std::map< Type::FunSig, Type* >			Type::function_types_;
std::map< Type*, Type* >				Type::closure_types_;
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

Type* Type::getClosureType( Type* f )
{
	assert( isFunction( f ) );

	std::map< Type*, Type* >::const_iterator i = closure_types_.find( f );
	if( i != closure_types_.end() ) {
		return (*i).second;
	}
	return closure_types_[f] = new Type( f );
}

Type* Type::getTupleType( const std::vector< Type* >& elems )
{
	std::map< std::vector< Type* >, Type* >::const_iterator i =
		tuple_types_.find( elems );
	if( i != tuple_types_.end() ) {
		return (*i).second;
	}
	
	return tuple_types_[elems] = new Type( elems );
	
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
	assert( tag() == TAG_FUNCTION );
	return funsig_.atypes;
}

const std::vector<Type*>& Type::getElements()
{
	assert( tag() == TAG_TUPLE );
	return elems_;
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
