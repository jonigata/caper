#ifndef CAPER_CPG_HPP
#define CAPER_CPG_HPP

#include "caper_ast.hpp"

////////////////////////////////////////////////////////////////
// make_cpg_parser
void make_cpg_parser(cpg::parser& p);

////////////////////////////////////////////////////////////////
// collect_informations
void collect_informations(
    GenerateOptions&    options,
    symbol_map_type&    terminal_types,
    symbol_map_type&    nonterminal_types,
    const value_type&   ast);

#endif // CAPER_CPG_HPP
