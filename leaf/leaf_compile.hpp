// 2008/08/13 Naoyuki Hirayama

/*!
	@file	  leaf_compile.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef LEAF_COMPILE_HPP_
#define LEAF_COMPILE_HPP_

#include <iosfwd>
#include "leaf_scanner.hpp"

class heap_cage;

namespace leaf {
struct Node;
}

typedef leaf::Scanner< std::istreambuf_iterator<char> > scanner_type;

leaf::Node* read_from_file(
	const std::string&	infile,
	scanner_type&		s );

void compile(
	const std::string&	infile,
	scanner_type&		s,
	leaf::Node*			n,
	std::ostream&		os );

#endif // LEAF_COMPILE_HPP_
