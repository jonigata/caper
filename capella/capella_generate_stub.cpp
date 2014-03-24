#include "capella_generate_cpp.hpp"
#include "capella_dependency.hpp"
#include <iostream>

namespace {

////////////////////////////////////////////////////////////////
// StructDeclarator
struct Context {
		std::string		class_header;
		std::string		class_footer;
		std::string		module_header;
		std::string		module_footer;
};

template < class PM >
struct StructDeclarator : public boost::static_visitor<void> {
		StructDeclarator( std::ostream& x, Context& y, PM z )
				: os(x), context(y), pointer_maker( z ) { }
		std::ostream&			os;
		Context&				context;
		PM						pointer_maker;

		template < class T >
		void operator()( const T& ) const {}

		template < class T >
		void apply_to_vector( const std::vector<T>& x ) const
		{
				for( typename std::vector<T>::const_iterator i = x.begin() ; i != x.end() ; ++i ) {
						boost::apply_visitor( StructDeclarator( os, context, pointer_maker ), *i );
				}
		}
		
		void operator()( const Module& x ) const
		{
				apply_to_vector( x.declarations  );
		}

		void operator()( const ClassHeaderDef& x ) const
		{
				context.class_header = x.data.s;
		}

		void operator()( const ClassFooterDef& x ) const
		{
				context.class_footer = x.data.s;
		}

		void operator()( const ModuleHeaderDef& x ) const
		{
				context.module_header = x.data.s;
		}

		void operator()( const ModuleFooterDef& x ) const
		{
				context.module_footer = x.data.s;
		}

		void operator()( const TypeDef& x ) const
		{
				switch( x.right.which() ) {
				case 1: // Scalor
				{
						Scalor s = boost::get< Scalor >( x.right );
						os << "	   " << pointer_maker.ptr( x.name.s ) << " Make" << x.name.s << "( "
						   << pointer_maker.ref( s.type.s ) << " x )\n"
						   << "	   {\n"
						   << "		   return " << pointer_maker.make( x.name.s ) << ";\n"
						   << "	   }\n";
						break;
				}
				case 2: // List
				{
						List s = boost::get< List >( x.right );

						// no arguments
						os << "	   " << pointer_maker.ptr( x.name.s ) << " Make" << x.name.s << "()\n"
						   << "	   {\n"
						   << "		   return " << pointer_maker.make( x.name.s ) << ";\n"
						   << "	   }\n";

						// 1 argument
						os << "	   " << pointer_maker.ptr( x.name.s ) << " Make" << x.name.s << "( ";
						os << pointer_maker.ref( s.etype.s ) << " x";
						os << " )\n"
						   << "	   {\n"
						   << "		   " << pointer_maker.ptr( x.name.s ) << " z = "
						   << pointer_maker.make( x.name.s ) << "();\n"
						   << "		   z" << pointer_maker.mem() << s.name.s << ".push_back( x );\n"
						   << "		   return z;\n"
						   << "	   }\n";
						// 2 arguments
						os << "	   " << pointer_maker.ptr( x.name.s ) << " Make" << x.name.s << "( ";
						os << pointer_maker.ref( s.etype.s ) << " x, " << pointer_maker.ref( s.etype.s ) << " y";
						os << " )\n"
						   << "	   {\n"
						   << "		   " << pointer_maker.ptr( x.name.s ) << " z = "
						   << pointer_maker.make( x.name.s ) << "();\n"
						   << "		   z" << pointer_maker.mem() << s.name.s << ".push_back( x );\n"
						   << "		   z" << pointer_maker.mem() << s.name.s << ".push_back( y );\n"
						   << "		   return z;\n"
						   << "	   }\n";
						// append
						os << "	   " << pointer_maker.ptr( x.name.s ) << " Make" << x.name.s << "( ";
						os << pointer_maker.ref( x.name.s ) << " x, " << pointer_maker.ref( s.etype.s ) << " y";
						os << " )\n"
						   << "	   {\n"
						   << "		   " << pointer_maker.ptr( x.name.s ) << " z = x;\n"
						   << "		   z" << pointer_maker.mem() << s.name.s << ".push_back( y );\n"
						   << "		   return z;\n"
						   << "	   }\n";
						break;
				}
				case 4: // Tuple
				{
						Tuple s = boost::get< Tuple >( x.right );
						os << "	   " << pointer_maker.ptr( x.name.s ) << " Make" << x.name.s << "( ";
						int n =0 ;
						for( std::vector< TupleItem >::const_iterator i = s.elements.begin() ;
							 i != s.elements.end() ;
							 ++i ) {
								if( n != 0 ) { os << ", "; }
								const TupleItem& ti = *i;
								switch( ti.which() ) {
								case 0:
										os << pointer_maker.ptr( boost::get<Scalor>( ti ).type.s ) << " a" << n;
										break;
								case 1:
										os << pointer_maker.ptr( boost::get<List>( ti ).etype.s ) << " a" << n;
										break;
								}
								n++;
						}
						os << " )\n"
						   << "	   {\n";
						os << "		   return " << pointer_maker.make( x.name.s ) << "(";
						n = 0;
						for( std::vector< TupleItem >::const_iterator i = s.elements.begin() ;
							 i != s.elements.end() ;
							 ++i ) {
								if( n != 0 ) { os << ","; }
								os << " a" << n;
								n++;
						}
						os << " );\n";
						os << "	   }\n";
						break;
				}
				}
		}

				
};

} // namespace

////////////////////////////////////////////////////////////////
//
template < class T >
void generate_stub(
		const std::string&		filename,
		std::ostream&			os,
		Dependency&				dependency,
		const Value&			v ,
		T						pointer_maker )
{
		std::string headername = filename;
		for( std::string::iterator i = headername.begin() ; i != headername.end() ; ++i ) {
				if( !isalpha( *i ) ) { *i = '_'; } else { *i = toupper( *i ); }
		}
		os << "#ifndef " << headername << "\n";
		os << "#define " << headername << "\n\n";

		os << "////////////////////////////////////////////////////////////////\n"
		   << "// semantic action stub\n";

		os << "struct SemanticAction {\n";
		os << "	   void syntax_error(){}\n";
		os << "	   void stack_overflow(){}\n\n";

		pointer_maker.cast( os );

		Context context;
		StructDeclarator<T> sd( os, context, pointer_maker );
		boost::apply_visitor( sd, v.data );

		os << "}\n";

		os << "#endif // " << headername << "\n";
}

struct normal_ptr_maker {
		std::string ptr( const std::string& x ) const { return x + "*"; }
		std::string ref( const std::string& x ) const { return x + "*"; }
		std::string make( const std::string& x ) const { return "new " + x; }		 
		std::string init( const std::string& x ) const { return " = " + x; }
		std::string mem() const { return "->"; }

		void cast( std::ostream& os )
		{
				os << "	   template < class T, class U >\n"
				   << "	   void downcast( T*& x, U* y ) { x = static_cast<T*>( y ); }\n"
				   << "	   template < class T, class U >\n"
				   << "	   void upcast( T*& x, U* y ) { x = y; }\n\n";
		}
};

struct shared_ptr_maker {
		std::string ptr( const std::string& x ) const { return "boost::shared_ptr< " + x + " >"; }
		std::string ref( const std::string& x ) const { return "const boost::shared_ptr< " + x + " >&"; }
		std::string make( const std::string& x ) const { return "new " + x; }
		std::string init( const std::string& x ) const { return "( " + x + " )"; }
		std::string mem() const { return "->"; }

		void cast( std::ostream& os )
		{
				os << "	   template < class T, class U >\n"
				   << "	   void downcast( boost::shared_ptr<T>& x, const boost::shared_ptr<U>& y ) { x = boost::static_pointer_cast<T>( y ); }\n"
				   << "	   template < class T, class U >\n"
				   << "	   void upcast( boost::shared_ptr<T>& x, const boost::shared_ptr<U>& y ) { x = y; }\n\n";
		}
};

struct boost_ptr_maker {
		std::string ptr( const std::string& x ) const { return x; }
		std::string ref( const std::string& x ) const { return "const " + x + "&"; }
		std::string make( const std::string& x ) const { return x; }		
		std::string init( const std::string& x ) const { return " = " + x; }
		std::string mem() const { return "."; }

		void cast( std::ostream& os )
		{
				os << "	   template < class T, class U >\n"
				   << "	   void downcast( T& x, const U& y ) { x = boost::get<T>( y ); }\n"
				   << "	   template < class T, class U >\n"
				   << "	   void upcast( T& x, const U& y ) { x = y; }\n\n";
		}
};

////////////////////////////////////////////////////////////////
//
void generate_stub_cpp_normal(
		const std::string&	filename,
		std::ostream&		os,
		Dependency&			dependency,
		const typeset_type& types,
		const atomset_type&,
		const Value&		v )
{
		generate_stub( filename, os, dependency, v, normal_ptr_maker() );
}

////////////////////////////////////////////////////////////////
//
void generate_stub_cpp_shared(
		const std::string&	filename,
		std::ostream&		os,
		Dependency&			dependency,
		const typeset_type& types,
		const atomset_type&,
		const Value&		v )
{
		generate_stub( filename, os, dependency, v, shared_ptr_maker() );
}

////////////////////////////////////////////////////////////////
//
void generate_stub_cpp_variant(
		const std::string&	filename,
		std::ostream&		os,
		Dependency&			dependency,
		const typeset_type& types,
		const atomset_type&,
		const Value&		v )
{
		generate_stub( filename, os, dependency, v, boost_ptr_maker() );
}

