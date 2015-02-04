#ifndef CAPELLA_GENERATE_CPP_HPP
#define CAPELLA_GENERATE_CPP_HPP

#include "capella_ast.hpp"

struct Dependency;

void generate_cpp_normal(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );
void generate_cpp_shared(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );

void generate_cpp11_normal(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );
void generate_cpp11_shared(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );

#endif // CAPELLA_GENERATE_CPP_HPP
