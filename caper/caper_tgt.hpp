#ifndef CAPER_TGT_HPP
#define CAPER_TGT_HPP

#include "caper_ast.hpp"

void make_target_parser(
    tgt::parsing_table&             table,
    std::map<std::string, size_t>&  token_id_map,
    action_map_type&                actions,
    const value_type&               ast,
    const symbol_map_type&          terminal_types,
    const symbol_map_type&          nonterminal_types,
    bool                            algorithm_lr1);

#endif // CAPER_TGT_HPP
