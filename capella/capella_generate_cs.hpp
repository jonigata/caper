#ifndef CAPELLA_GENERATE_CS_HPP
#define CAPELLA_GENERATE_CS_HPP

#include "capella_ast.hpp"

struct Dependency;

void generate_cs(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );

#endif // CAPELLA_GENERATE_CS_HPP
