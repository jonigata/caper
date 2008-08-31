// 2008/08/15 Naoyuki Hirayama

/*!
    @file     leaf_type.hpp
    @brief    <ŠT—v>

    <à–¾>
*/

#ifndef LEAF_TYPE_HPP_
#define LEAF_TYPE_HPP_

#include <vector>
#include <string>
#include <map>

namespace leaf {

class Type {
public:
    enum Tag {
        TAG_BOOL,
        TAG_CHAR,
        TAG_SHORT,
        TAG_INT,
        TAG_LONG,
        TAG_TUPLE,
        TAG_FUNCTION,
    };

    
public:
    static Type* getVoidType();
    static Type* getBoolType();
    static Type* getCharType();
    static Type* getShortType();
    static Type* getIntType();
    static Type* getLongType();
    static Type* getFunctionType( Type* rtypes, Type* atypes );
    static Type* getTupleType( const std::vector< Type* >& elems );

    static std::string getDisplay( Type* );
    static bool isFunction( Type* );
    static bool isComplete( Type* );
    static Type* normalize( Type* );

protected:
    struct FunSig {
        Type* rtypes;
        Type* atypes;

        bool operator<( const FunSig& x ) const
        {
            if( rtypes < x.rtypes ) {
                return true;
            } else if( x.rtypes > rtypes ) {
                return false;
            } else if( atypes < x.atypes ) {
                return true;
            } else if( x.atypes < atypes ) {
                return false;
            }
            return true;
        }
    };

public:
    Tag tag() { return tag_; }

    Type* getReturnType();
    Type* getArgumentType();
    const std::vector<Type*>& getElements();

protected:
    Type( Tag t ) : tag_(t) {}
    Type( const FunSig& s ) : tag_(TAG_FUNCTION), funsig_(s) {}
    Type( const std::vector< Type* >& s ) : tag_(TAG_TUPLE), elems_(s) {}

private:
    Tag     tag_;
    FunSig  funsig_;
    std::vector< Type* > elems_;

private:
    static std::map< FunSig, Type* > function_types_;
    static std::map< std::vector< Type* >, Type* > tuple_types_;

};

} // namespace leaf

#endif // LEAF_TYPE_HPPh_
