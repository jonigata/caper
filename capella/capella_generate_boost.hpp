#ifndef CAPELLA_GENERATE_BOOST_HPP
#define CAPELLA_GENERATE_BOOST_HPP

#include "capella_ast.hpp"

struct Dependency;

void generate_cpp_variant(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );

#endif // CAPELLA_GENERATE_BOOST_HPP
