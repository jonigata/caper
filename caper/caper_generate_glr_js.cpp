// Copyright (C) 2008 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "caper_ast.hpp"
#include "caper_generate_cpp.hpp"
#include "caper_format.hpp"
#include "caper_stencil.hpp"
#include "caper_finder.hpp"
#include <algorithm>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace {

void make_action_case(
    std::ostream&                           os,
    int                                     indent, 
    const tgt::parsing_table::action_entry& action_entry) {
    // action
    switch (action_entry.type) {
        case zw::gr::action_shift:
            stencil_indent(
                os, indent, R"(
                    { method: 'shift', target: ${dest_index} },
)",
                {"dest_index", action_entry.dest_index}
                );
            break;
        case zw::gr::action_reduce: {
            stencil_indent(
                os, indent, R"(
                    {
                        method: 'reduce',
                        production: {
                            left: Nonterminal.${nonterminal},
                            right: {
                                length: ${right_length}
                            }
                        }
                    },
)",
                {"right_length", action_entry.rule.right().size()},
                {"nonterminal", action_entry.rule.left().name()}
                );
        }
            break;
        case zw::gr::action_accept:
            stencil_indent(
                os, indent, R"(
                    { method: 'accept' },
)"
                );
            break;
        case zw::gr::action_error:
            stencil_indent(
                os, indent, R"(
                    { method: 'error' },
)"
                );
            break;
    }
}

}

void generate_glr_javascript(
    const std::string&                  src_filename,
    std::ostream&                       os,
    const GenerateOptions&              options,
    const std::map<std::string, Type>&,
    const std::map<std::string, Type>&  nonterminal_types,
    const std::vector<std::string>&     tokens,
    const action_map_type&              actions,
    const tgt::parsing_table&           table) {

    // notice / URL
    stencil(
        os, R"(
// This file was automatically generated by Caper.
// (http://jonigata.github.io/caper/caper.html)

var ${namespace_name} = (function() {

    var exports = {};

)",
        {"namespace_name", options.namespace_name}
        
        );

    if (!options.external_token) {
        // token enumeration
        stencil(
            os, R"(
    var Token = {
$${tokens}
        null : null
    };
    exports.Token = Token;

    var getTokenLabel = function(t) {
        var labels = [
$${labels}
            null
        ];
        return labels[t];
    };
    exports.getTokenLabel = getTokenLabel;

)",
            {"tokens", [&](std::ostream& os){
                    int index = 0;
                    for(const auto& token: tokens) {
                        stencil(
                            os, R"(
        ${prefix}${token}: ${index},
)",
                            {"prefix", options.token_prefix},
                            {"token", token},
                            {"index", index}
                            );
                        index++;
                    }
                }},
            {"labels", [&](std::ostream& os){
                    for(const auto& token: tokens) {
                        stencil(
                            os, R"(
            "${token}",
)",
                            {"token", token}
                            );
                    }
                }}
            );

    }

    // nonterminal
    stencil(
        os, R"(
    var Nonterminal = {
)"
        );
    {
        int index = 0;
        for (const auto& nonterminal_type: nonterminal_types) {
            stencil(
                os, R"(
        ${nonterminal_name}: ${index},
)",
                {"nonterminal_name", nonterminal_type.first},
                {"index", index}
                );
            index++;
        }
    }
    stencil(
        os, R"(
        null: null
    };
    var getNonterminalLabel = function(t) {
        var labels = [
$${labels}
            null
        ];
        return labels[t];
    };
    exports.getNonterminalLabel = getNonterminalLabel;

)",
        {"labels", [&](std::ostream& os){
                for (const auto& nonterminal_type: nonterminal_types) {
                    stencil(
                        os, R"(
            "${nonterminal_name}",
)",
                        {"nonterminal_name", nonterminal_type.first}
                        );
                }
            }}
        );
    
    stencil(
        os, R"(
    var Table = [
)"
        );

    // states handler
    for (const auto& state: table.states()) {
        // state header
        stencil(
            os, R"(
        {
            index: ${index},
            state: (function(){
                var a = {};
)",
            {"index", state.no}
            );

        // action table
        for (const auto& pair: state.action_table) {
            const auto& token = pair.first;
            const auto& action = pair.second;

            std::string case_tag =
                "Token." + options.token_prefix + tokens[token];

            // action header 
            stencil(
                os, R"(
                a[${case_tag}] = [
)",
                {"case_tag", case_tag}
                );

            // for each fork
            for(const auto& action_entry: action.entries) {
                make_action_case(
                    os,
                    0,
                    action_entry);
            }
            stencil(
                os, R"(
                    null
                ]; // actions end
)"
                );
        }

        // state footer
        stencil(
            os, R"(
                return a;
            })(), // end state
)"
            );
        
        // gotof header
        stencil(
            os, R"(
            gotof: (function(){
                var a = {};
)"
            );
            
        // gotof dispatcher
        std::stringstream ss;
        for (const auto& pair: state.goto_table) {
            stencil(
                os, R"(
                a[Nonterminal.${nonterminal}] = ${state_index};
)",
                {"nonterminal", pair.first.name()},
                {"state_index", pair.second}
                );
        }

        // gotof footer
        stencil(
            os, R"(
                return a;
            })() // end gotof
)"
            );
        stencil(os, R"(
        }, // end state

)"
            );
    }
    stencil(os, R"(
        null
    ];
    exports.Table = Table;

    exports.firstState = ${first_state};

    return exports;
})();

exports.${namespace_name} = ${namespace_name};
)",
            {"first_state", table.first_state()},
            {"namespace_name", options.namespace_name}
        );
}
