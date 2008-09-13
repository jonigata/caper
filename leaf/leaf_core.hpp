// 2008/09/14 Naoyuki Hirayama

/*!
    @file     leaf_core.hpp
    @brief    <ŠT—v>

    <à–¾>
*/

#ifndef LEAF_CORE_HPP_
#define LEAF_CORE_HPP_

namespace leaf {

////////////////////////////////////////////////////////////////
// Symbol
struct Symbol {
    std::string s;
    Symbol(){}
    Symbol( const std::string& as ) : s(as) {}
};

}

#endif // LEAF_CORE_HPP_
