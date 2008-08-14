#ifndef CAPELLA_DEPENDENCY_HPP
#define CAPELLA_DEPENDENCY_HPP

#include "capella_ast.hpp"
#include <boost/graph/adjacency_list.hpp>

////////////////////////////////////////////////////////////////
// Dependency
struct Dependency {
        // for topological sort
        typedef boost::adjacency_list<>                                 graph_type;
        typedef boost::graph_traits<graph_type>::vertex_descriptor      vertex_type;
        typedef boost::graph_traits<graph_type>::edge_descriptor        edge_type;

        graph_type g;
        std::map< std::string, vertex_type > d;
        std::map< vertex_type, std::string > r;

        vertex_type find_vertex( const std::string& s )
        {
                std::map< std::string, vertex_type >::const_iterator k = d.find( s );
                if( k == d.end() ) {
                        vertex_type v = boost::add_vertex( g );
                        d[s] = v;
                        r[v] = s;
                        return v;
                }
                return (*k).second;
        }

        void add_edge( const std::string& x, const std::string& y ) // link x <= y
        {
                vertex_type xx = find_vertex( x );
                vertex_type yy = find_vertex( y );
                boost::add_edge( xx, yy, g );
        }

};

void make_dependency( Dependency& dependency, const Value& v );

#endif // CAPELLA_DEPENDENCY_HPP
