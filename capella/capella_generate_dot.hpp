#ifndef CAPELLA_GENERATE_DOT_HPP
#define CAPELLA_GENERATE_DOT_HPP

#include "capella_dependency.hpp"

void generate_dot(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );

#endif // CAPELLA_GENERATE_DOT_HPP
