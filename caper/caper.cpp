// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

// parser generator 'caper'

#include <cctype>
#include <cstdlib>
#include <cstring>
using std::exit;

#include "fastlalr.hpp"
#include "caper_error.hpp"
#include "caper_scanner.hpp"
#include "caper_cpg.hpp"
#include "caper_tgt.hpp"
#include "caper_generate_cpp.hpp"
#include "caper_generate_js.hpp"
#include "caper_generate_csharp.hpp"
#include "caper_generate_d.hpp"
#include "caper_generate_java.hpp"
#include "caper_generate_boo.hpp"
#include "caper_generate_ruby.hpp"
#include "caper_generate_php.hpp"
#include "caper_generate_haxe.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/filesystem/operations.hpp>

struct commandline_options {
    std::string infile;
    std::string outfile;
    std::string language;
    std::string algorithm;
    bool        debug_parser;
};

void get_commandline_options(
    commandline_options&    cmdopt,
    int                     argc,
    const char**            argv) {
    cmdopt.language = "C++";
    cmdopt.algorithm = "lalr1";
    cmdopt.debug_parser = false;

    int state = 0;
    for (int index = 1 ; index < argc ; index++) {
        std::string arg = argv[index];

        if (arg[0] == '-') {
            if (arg == "-java" || arg == "-Java") {
                cmdopt.language = "Java";
                continue;
            }
            if (arg == "-cs" || arg == "-CS" ||
                arg == "-Cs" || arg == "-C#" ||
                arg == "-CSharp" || arg == "-csharp#" ||
                arg == "-c#") {
                cmdopt.language = "C#";
                continue;
            }
            if (arg == "-d" || arg == "-D") {
                cmdopt.language = "D";
                continue;
            }
            if (arg == "-c++" || arg == "-C++" ||
                arg == "-cpp" || arg == "-CPP") {
                cmdopt.language = "C++";
                continue;
            }
            if (arg == "-js" || arg == "-JS" ||
                arg == "-javascript" ||
                arg == "-JavaScript" ||
                arg == "-JAVASCRIPT") {
                cmdopt.language = "JavaScript";
                continue;
            }
            if (arg == "-boo" || arg == "-BOO") {
                cmdopt.language = "Boo";
                continue;
            }
            if (arg == "-rb" || arg == "-RB" ||
                arg == "-ruby" || arg == "-Ruby" || arg == "-RUBY") {
                cmdopt.language = "Ruby";
                continue;
            }
            if (arg == "-php" || arg == "-PHP") {
                cmdopt.language = "PHP";
                continue;
            }
            if (arg == "-haxe" || arg == "-HAXE" || arg == "-Haxe" ||
                arg == "-haXe" || arg == "-hx" || arg == "-HX") {
                cmdopt.language = "Haxe";
                continue;
            }
            if (arg == "-lalr1") {
                cmdopt.algorithm = "lalr1";
                continue;
            }
            if (arg == "--debug") {
                cmdopt.debug_parser = true;
                continue;
            }
            
/*
            if (arg == "-lr1") == 0) {
                cmdopt.algorithm = "lr1";
                continue;
            }
*/

            std::cerr << "caper: unknown option: " << argv[index] << std::endl;
            exit(1);
        }

        switch (state) {
            case 0: cmdopt.infile = arg; state++; break;
            case 1: cmdopt.outfile = arg; state++; break;
            default:
                std::cerr << "caper: too many arguments" << std::endl;
                exit(1);
        }
    }

    if (state < 2) {
        std::cerr << "caper: usage: caper [-c++ | -js | -cs | -d | -java | -boo | -ruby | -php | -haxe] input_filename output_filename" << std::endl;
        exit(1);
    }

}

int main(int argc, const char** argv) {
    commandline_options cmdopt;
    get_commandline_options(cmdopt, argc, argv);

    typedef void(*generator_type)(
        const std::string&,
        std::ostream&,
        const GenerateOptions&,
        const std::map<std::string, Type>&,
        const std::map<std::string, Type>&,
        const std::vector<std::string>&,
        const action_map_type&,
        const tgt::parsing_table&);

    std::unordered_map<std::string, generator_type> generators;
    generators["C++"]           = generate_cpp;
    generators["JavaScript"]    = generate_javascript;
    generators["C#"]            = generate_csharp;
    generators["D"]             = generate_d;
    generators["Java"]          = generate_java;
    generators["Boo"]           = generate_boo;
    generators["Ruby"]          = generate_ruby;
    generators["PHP"]           = generate_php;
    generators["Haxe"]          = generate_haxe;

    std::ifstream ifs(cmdopt.infile.c_str());
    if (!ifs) {
        std::cerr << "caper: can't open input file '" << cmdopt.infile << "'" << std::endl;
        exit(1);
    }

    std::ofstream ofs(cmdopt.outfile.c_str());
    if (!ofs) {
        std::cerr << "caper: can't open output file '" << cmdopt.outfile << "'" << std::endl;
        exit(1);
    }

    // cpgスキャナ
    typedef std::istreambuf_iterator<char> is_iterator;
    is_iterator b(ifs);
    is_iterator e;
    scanner<is_iterator> s(b, e);

    try {
        // cpgパーサ
        cpg::parser p;
        make_cpg_parser(p);

        // cpgパース
        Token token = token_empty;
        while (token != token_eof) {
            value_type v;
            token = s.get(v);
            try {
                p.push(token, v);
            }
            catch(zw::gr::syntax_error&) {
                throw syntax_error(v.range.beg, token);
            }
        }


        // 各種情報の収集
        GenerateOptions options;
        options.debug_parser = cmdopt.debug_parser;

        std::map<std::string, Type> terminal_types;
        std::map<std::string, Type> nonterminal_types;
        collect_informations(
            options,
            terminal_types,
            nonterminal_types,
            p.accept_value());

        // 対象文法の構文テーブルの作成
        tgt::parsing_table table;
        std::map<std::string, size_t> token_id_map;
        action_map_type actions;
        make_target_parser(
            table,
            token_id_map,
            actions,
            p.accept_value(),
            terminal_types,
            nonterminal_types);

        // ターゲットパーサの出力
        std::vector<std::string> tokens(token_id_map.size());
        for (const auto& x: token_id_map) {
            tokens[x.second] = x.first;
        }
        generators[cmdopt.language](
            cmdopt.outfile,
            ofs,
            options,
            terminal_types,
            nonterminal_types,
            tokens,
            actions,
            table);

    }
    catch(caper_error& e) {
        if (e.addr <0) {
            std::cerr << "caper: " << e.what() << std::endl;
        } else {
            std::cerr << "caper: " 
                      << e.what()
                      << ", line: " << s.lineno(e.addr)
                      << ", column: " << s.column(e.addr)
                      << std::endl;
        }

        ofs.close();
        boost::filesystem::remove(cmdopt.outfile);

        return 1;
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
