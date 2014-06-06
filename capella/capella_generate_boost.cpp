#include "capella_generate_boost.hpp"
#include "capella_dependency.hpp"
#include <cctype>
#include <sstream>
#include <boost/graph/topological_sort.hpp>

namespace {

////////////////////////////////////////////////////////////////
// ForwardDeclarator
struct ForwardDeclarator : public boost::static_visitor<void> {
        ForwardDeclarator( std::ostream& x ) : os(x){}
        std::ostream& os;

        template < class T >
        void operator()( const T& ) const {}

        template < class T >
        void apply_to_vector( const std::vector<T>& x ) const
        {
                for( typename std::vector<T>::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
                        boost::apply_visitor( ForwardDeclarator( os ), *i );
                }
        }
        
        void operator()( const Module& x ) const
        {
                apply_to_vector( x.declarations  );
        }

        void operator()( const TypeDef& x ) const
        {
                if( x.right.which() != 3 ) { // Variant
                        os << "struct " << x.name.s << ";" << std::endl;
                }
        }
};

////////////////////////////////////////////////////////////////
// VariantDeclarator
struct VariantDeclarator : public boost::static_visitor<void> {
        VariantDeclarator( std::ostream& x ) : os(x){}
        std::ostream& os;

        template < class T >
        void operator()( const T& ) const {}

        template < class T >
        void apply_to_vector( const std::vector<T>& x ) const
        {
                for( typename std::vector<T>::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
                        boost::apply_visitor( VariantDeclarator( os ), *i );
                }
        }
        
        void operator()( const Module& x ) const
        {
                apply_to_vector( x.declarations  );
        }

        void operator()( const TypeDef& x ) const
        {
                if( x.right.which() == 3 ) { // Variant
                        Variant v( boost::get< Variant >( x.right ) );

                        os << "typedef boost::variant<\n";
                        for( std::vector<Identifier>::const_iterator i = v.choises.begin() ;
                             i != v.choises.end() ; ++i ) {
                                os << "        boost::recursive_wrapper< " << (*i).s << " >,\n";
                        }
                        os << "    > " << x.name.s << ";\n\n";
                }
        }
};

////////////////////////////////////////////////////////////////
// StructDeclarator
struct StructDeclarator : public boost::static_visitor<void> {
        typedef std::map< std::string, std::string > declarations_type;

        StructDeclarator( declarations_type& x ) : declarations(x){}
        declarations_type&      declarations;

        // match
        template < class T >
        void operator()( const T& ) const {}

        template < class T >
        void apply_to_vector( const std::vector<T>& x ) const
        {
                for( typename std::vector<T>::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
                        boost::apply_visitor( StructDeclarator( declarations ), *i );
                }
        }
        
        void operator()( const Module& x ) const
        {
                apply_to_vector( x.declarations  );
        }

        void operator()( const TypeDef& x ) const
        {
                std::stringstream os;
                
                switch( x.right.which() ) {
                case 1: // Scalor
                {
                        Scalor s = boost::get< Scalor >( x.right );
                        os << "struct " << x.name.s << " {\n";
                        os << "    " << s.type.s << " " << s.name.s << ";\n\n";
                        os << "    " << x.name.s << "() {}\n";
                        os << "    " << x.name.s << "(const " << s.type.s << "& x)\n"
                           << "        : " << s.name.s << "(x) {}\n";
                        os << "};\n\n";
                        break;
                }
                case 2: // List
                {
                        List s = boost::get< List >( x.right );
                        os << "struct " << x.name.s << " {\n";
                        os << "    std::vector< " << s.etype.s << " > items;\n\n";
                        os << "    " << x.name.s << "() {}\n";
                        os << "    " << x.name.s << "(const std::vector<" << s.etype.s << ">& x)\n"
                           << "        : " << s.name.s << "(x) {}\n";
                        os << "};\n\n";
                        break;
                }
                case 4: // Tuple
                {
                        Tuple s = boost::get< Tuple >( x.right );
                        os << "struct " << x.name.s << " {\n";
                        int n = 0;
                        for( std::vector< TupleItem >::const_iterator i = s.elements.begin() ;
                             i != s.elements.end() ;
                             ++i ) {
                                const TupleItem& ti = *i;
                                switch( ti.which() ) {
                                case 0:
                                        os << "    " << boost::get<Scalor>( ti ).type.s
                                           << " " << boost::get<Scalor>( ti ).name.s << ";\n";
                                        break;
                                case 1: os << "    " << boost::get<List>( ti ).etype.s
                                           << " " << boost::get<List>( ti ).name.s << ";\n";
                                        break;
                                }
                                n++;
                        }
                        
                        os << "\n";
                        os << "    " << x.name.s << "() {}\n";
                        os << "    " << x.name.s << "(";
                        n = 0;
                        for( std::vector< TupleItem >::const_iterator i = s.elements.begin() ;
                             i != s.elements.end() ;
                             ++i ) {
                                if( n != 0 ) { os << ", "; }
                                const TupleItem& ti = *i;
                                switch( ti.which() ) {
                                case 0:
                                        os << boost::get<Scalor>( ti ).type.s << " a" << n;
                                        break;
                                case 1: os << boost::get<List>( ti ).etype.s << " a" << n;
                                        break;
                                }
                                n++;
                        }
                        os << ")\n"
                           << "        : ";
                        n = 0;
                        for( std::vector< TupleItem >::const_iterator i = s.elements.begin() ;
                             i != s.elements.end() ;
                             ++i ) {
                                if( n != 0 ) { os << ", "; }
                                const TupleItem& ti = *i;
                                switch( ti.which() ) {
                                case 0:
                                        os << boost::get<Scalor>( ti ).name.s << "( a" << n << " )";
                                        break;
                                case 1: os << boost::get<List>( ti ).name.s << "( a" << n << " )";
                                        break;
                                }

                                n++;
                        }
                        os << "{}\n";
                        os << "};\n\n";
                        break;
                }
                }

                declarations[ x.name.s ] = os.str();
        }

                
};

} // namespace

////////////////////////////////////////////////////////////////
//
void generate_cpp_variant(
        const std::string&  filename,
        std::ostream&       os,
        Dependency&         dependency,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&        v )
{
        std::string headername = filename;
        for( std::string::iterator i = headername.begin() ; i != headername.end() ; ++i ) {
                if( !std::isalpha( *i ) ) { *i = '_'; } else { *i = std::toupper( *i ); }
        }
        os << "#ifndef " << headername << "\n";
        os << "#define " << headername << "\n\n";

        os << "#include <vector>\n"
           << "#include <boost/variant.hpp>\n\n";

        os << "////////////////////////////////////////////////////////////////\n"
           << "// forward declarations\n";
        ForwardDeclarator fd( os );
        boost::apply_visitor( fd, v.data );
        os << std::endl;

        os << "////////////////////////////////////////////////////////////////\n"
           << "// variant declarations\n";
        VariantDeclarator vd( os );
        boost::apply_visitor( vd, v.data );
        os << std::endl;

        os << "////////////////////////////////////////////////////////////////\n"
           << "// struct declarations\n";
        StructDeclarator::declarations_type declarations;
        StructDeclarator sd( declarations );
        boost::apply_visitor( sd, v.data );

        std::vector< Dependency::vertex_type > c;
        boost::topological_sort( dependency.g, std::back_inserter( c ) );

        for( std::vector< Dependency::vertex_type >::const_iterator i = c.begin() ; i != c.end() ; ++i ) {
                os << declarations[ dependency.r[*i] ];
        }

        os << "#endif // " << headername << "\n";
}

