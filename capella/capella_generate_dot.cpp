// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "capella_generate_dot.hpp"
#include <fstream>
#include <boost/graph/graphviz.hpp>

namespace {

struct label_writer {
        label_writer( Dependency& x ) : d( x ){}
        Dependency& d;

        template < class VertexOrEdge >
        void operator()( std::ostream& out, const VertexOrEdge& v ) const
        {
                out << "[label=\"" << d.r[v] << "\"]";
        }
};

} // namespace 

////////////////////////////////////////////////////////////////
//
void generate_dot(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v )
{
        boost::write_graphviz( os, dependency.g, label_writer( dependency ) );
}

