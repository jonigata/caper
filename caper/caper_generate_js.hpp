#ifndef CAPER_GENERATE_JS_HPP
#define CAPER_GENERATE_JS_HPP

#include "caper_ast.hpp"

void generate_javascript(
        const std::string&                      filename,
        std::ostream&                           os,
        const GenerateOptions&                  options,
        const symbol_map_type&                  terminal_types,
        const symbol_map_type&                  nonterminal_types,
        const std::map< size_t, std::string >&  token_id_map,
        const action_map_type&                  actions,
        const tgt::parsing_table&               table );

#endif // CAPER_GENERATE_JS_HPP
