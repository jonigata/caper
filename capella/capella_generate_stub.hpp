#ifndef CAPELLA_GENERATE_STUB_HPP
#define CAPELLA_GENERATE_STUB_HPP

#include "capella_ast.hpp"

struct Dependency;

void generate_stub_cpp_normal(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );
void generate_stub_cpp_shared(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );
void generate_stub_cpp_variant(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v );

#endif // CAPELLA_GENERATE_STUB_HPP
