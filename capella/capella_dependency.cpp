#include "capella_dependency.hpp"

////////////////////////////////////////////////////////////////
// DependencyMaker
struct DependencyMaker : public boost::static_visitor<void> {
        DependencyMaker( Dependency& y ) : dependency(y){}
        Dependency& dependency;

        // match
        template < class T >
        void operator()( const T& ) const {}

        template < class T >
        void apply_to_vector( const std::vector<T>& x ) const
        {
                for( typename std::vector<T>::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
                        boost::apply_visitor( DependencyMaker( dependency ), *i );
                }
        }
        
        void operator()( const Module& x ) const
        {
                apply_to_vector( x.declarations.elements  );
        }

        void operator()( const TypeDef& x ) const
        {
                switch( x.right.which() ) {
                case 1: // Scalor
                {
                        Scalor s = boost::get< Scalor >( x.right );
                        dependency.add_edge( x.name.s, s.type.s );
                        break;
                }
                case 2: // List
                {
                        List s = boost::get< List >( x.right );
                        dependency.add_edge( x.name.s, s.etype.s );
                        break;
                }
                case 3: // Variant
                {
                        Variant v = boost::get< Variant >( x.right );
                        for( std::vector<Identifier>::const_iterator i = v.choises.begin() ;
                             i != v.choises.end() ; ++i ) {
                                dependency.add_edge( x.name.s, (*i).s );
                        }
                        break;
                }
                case 4: // Tuple
                {
                        Tuple s = boost::get< Tuple >( x.right );
                        for( std::vector< TupleItem >::const_iterator i = s.elements.begin() ;
                             i != s.elements.end() ;
                             ++i ) {
                                const TupleItem& ti = *i;
                                switch( ti.which() ) {
                                case 0: dependency.add_edge( x.name.s, boost::get<Scalor>( ti ).type.s ); break;
                                case 1: dependency.add_edge( x.name.s, boost::get<List>( ti ).etype.s ); break;
                                }
                        }
                        break;
                }
                }
        }
};

void make_dependency( Dependency& dependency, const Value& v )
{
        DependencyMaker dm( dependency );
        boost::apply_visitor( dm, v.data );
}
