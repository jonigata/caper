// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

// parser generator 'caper'

#include <cctype>
#include <cstdlib>
#include <cstring>
using std::exit;

#include "fastlalr.hpp"
#include "caper_ast.hpp"
#include "caper_error.hpp"
#include "caper_scanner.hpp"
#include "caper_cpg.hpp"
#include "caper_tgt.hpp"
#include "caper_generate_cpp.hpp"
#include "caper_generate_js.hpp"
#include "caper_generate_csharp.hpp"
#include "caper_generate_d.hpp"
#include "caper_generate_java.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <boost/filesystem/operations.hpp>

struct commandline_options {
	std::string		infile;
	std::string		outfile;
	std::string		language;
	std::string		algorithm;
};

void get_commandline_options(
	commandline_options&	cmdopt,
	int						argc,
	char**					argv )
{
	cmdopt.language = "C++";
	cmdopt.algorithm = "lalr1";
		
	int state = 0;
	for( int index = 1 ; index < argc ; index++ ) {
		if( argv[index][0] == '-' ) {
			if( strcmp( argv[index], "-java" ) == 0 ||
				strcmp( argv[index], "-Java" ) == 0 ) {
				cmdopt.language = "Java";
				continue;
			}
			if( strcmp( argv[index], "-cs" ) == 0 || strcmp( argv[index], "-CS" ) == 0 ||
				strcmp( argv[index], "-Cs" ) == 0 || strcmp( argv[index], "-C#" ) == 0 ||
				strcmp( argv[index], "-CSharp" ) == 0 || strcmp( argv[index], "-csharp#" ) == 0 ||
				strcmp( argv[index], "-c#" ) == 0 ) {
				cmdopt.language = "C#";
				continue;
			}
			if( strcmp( argv[index], "-d" ) == 0 || strcmp( argv[index], "-D" ) == 0 ) {
				cmdopt.language = "D";
				continue;
			}
			if( strcmp( argv[index], "-c++" ) == 0 || strcmp( argv[index], "-C++" ) == 0 ||
				strcmp( argv[index], "-cpp" ) == 0 || strcmp( argv[index], "-CPP" ) == 0 ) {
				cmdopt.language = "C++";
				continue;
			}
			if( strcmp( argv[index], "-js" ) == 0 || strcmp( argv[index], "-JS" ) == 0 ||
				strcmp( argv[index], "-javascript" ) == 0 ||
				strcmp( argv[index], "-JavaScript" ) == 0 ||
				strcmp( argv[index], "-JAVASCRIPT" ) == 0 ) {
				cmdopt.language = "JavaScript";
				continue;
			}
			if( strcmp( argv[index], "-lalr1" ) == 0 ) {
				cmdopt.algorithm = "lalr1";
				continue;
			}
			if( strcmp( argv[index], "-lr1" ) == 0 ) {
				cmdopt.algorithm = "lr1";
				continue;
			}

			std::cerr << "caper: unknown option: " << argv[index] << std::endl;
			exit(1);
		}

		switch( state ) {
		case 0: cmdopt.infile  = argv[index]; state++; break;
		case 1: cmdopt.outfile = argv[index]; state++; break;
		default:
			std::cerr << "caper: too many arguments" << std::endl;
			exit(1);
		}
	}

	if( state < 2 ) {
		std::cerr << "caper: usage: caper [ -c++ | -js | -cs | -java ] input_filename output_filename" << std::endl;
		exit(1);
	}
		
}

int main( int argc, char** argv )
{
	commandline_options cmdopt;
	get_commandline_options( cmdopt, argc, argv );

	typedef void (*generator_type)(
		const std::string&,
		std::ostream&,
		const GenerateOptions&,
		const symbol_map_type&,
		const symbol_map_type&,
		const std::map< size_t, std::string >&,
		const action_map_type&,
		const tgt::parsing_table& );
		
	std::unordered_map< std::string, generator_type > generators;
	generators["Java"]		= generate_java;
	generators["C#"]		= generate_csharp;
	generators["C++"]				= generate_cpp;
	generators["JavaScript"]		= generate_javascript;
	generators["D"]					= generate_d;

	std::ifstream ifs( cmdopt.infile.c_str() );
	if( !ifs ) {
		std::cerr << "caper: can't open input file '" << cmdopt.infile << "'" << std::endl;
		exit(1);
	}

	std::ofstream ofs( cmdopt.outfile.c_str() );
	if( !ofs ) {
		std::cerr << "caper: can't open output file '" << cmdopt.outfile << "'" << std::endl;
		exit(1);
	}

	// cpgスキャナ
	typedef std::istreambuf_iterator<char> is_iterator;
	is_iterator b( ifs );	// 即値にするとVC++が頓珍漢なことを言う
	is_iterator e;
	scanner< is_iterator > s( b, e );

	try {
		// cpgパーサ
		cpg::parser p;
		make_cpg_parser( p );

		// cpgパース
		Token token = token_empty;
		while( token != token_eof ) {
			value_type v;
			token = s.get( v );
			try {
				p.push( token, v );
			}
			catch( zw::gr::syntax_error& ) {
				throw syntax_error( v.range.beg, token );
			}
		}


		// 各種情報の収集
		GenerateOptions options;
		options.token_prefix = "token_";
		options.external_token = false;
		options.namespace_name = "caper_parser";
		options.dont_use_stl = false;

		symbol_map_type terminal_types;
		symbol_map_type nonterminal_types;
		collect_informations(
			options,
			terminal_types,
			nonterminal_types,
			p.accept_value() );

		// 対象文法の構文テーブルの作成
		tgt::parsing_table table;
		std::map< std::string, size_t > token_id_map;
		action_map_type actions;
		make_target_parser(
			table,
			token_id_map,
			actions,
			p.accept_value(),
			terminal_types,
			nonterminal_types,
			cmdopt.algorithm == "lr1" );

		// ターゲットパーサの出力
		std::map< size_t, std::string > reverse_token_id_map;
		for( std::map< std::string, size_t >::const_iterator i = token_id_map.begin() ;
			 i != token_id_map.end();
			 ++i ) {
			reverse_token_id_map[(*i).second] = (*i).first;
		}
		generators[ cmdopt.language ](
			cmdopt.outfile,
			ofs,
			options,
			terminal_types,
			nonterminal_types,
			reverse_token_id_map,
			actions,
			table );

	}
	catch( caper_error& e ) {
		if( e.addr < 0 ) {
			std::cerr << "caper: " << e.what() << std::endl;
		} else {
			std::cerr << "caper: " 
					  << e.what()
					  << ", line: " << s.lineno( e.addr )
					  << ", column: " << s.column( e.addr )
					  << std::endl;
		}

		ofs.close();
		boost::filesystem::remove( cmdopt.outfile );

		return 1;
	}
	catch( std::exception& e ) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
