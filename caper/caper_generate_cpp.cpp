// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "caper_ast.hpp"
#include "caper_generate_cpp.hpp"
#include <algorithm>

void generate_cpp(
    const std::string&                      src_filename,
    std::ostream&                           os,
    const GenerateOptions&                  options,
    const symbol_map_type&                  terminal_types,
    const symbol_map_type&                  nonterminal_types,
    const std::map< size_t, std::string >&  token_id_map,
    const action_map_type&                  actions,
    const tgt::parsing_table&               table )
{
#ifdef _WINDOWS
    char basename[_MAX_PATH];
    char extension[_MAX_PATH];
    _splitpath( src_filename.c_str(), NULL, NULL, basename, extension );
    std::string filename = std::string(basename) + extension;
#else
    std::string filename = src_filename;
#endif
        
    std::string headername = filename;
    for( std::string::iterator i = headername.begin() ; i != headername.end() ; ++i ) {
        if( !isalpha( *i ) && !isdigit( *i ) ) { *i = '_'; } else { *i = toupper( *i ); }
    }
        

    // once header
    os << "#ifndef " << headername << "_\n"
       << "#define " << headername << "_\n\n"
        ;
    // include
    os << "#include <cstdlib>\n";
    os << "#include <cassert>\n";
    if( !options.dont_use_stl ) {
        os << "#include <vector>\n";
    }
    os << "\n";

    // namespace header
    os << "namespace " << options.namespace_name << " {\n\n";

    if( !options.external_token ) {
        // token enumeration
        os << "enum Token {\n";
        for( size_t i = 0 ; i < token_id_map.size() ; i++ ) {
            os << "    " << options.token_prefix << (*token_id_map.find( i )).second << ",\n";
        }
        os << "};\n\n";
    }

    // stack class header

    if( !options.dont_use_stl ) {
        // STL version
        os << "template < class T, int StackSize >\n"
           << "class Stack {\n"
           << "public:\n"
           << "        Stack(){ gap_ = 0; }\n"
           << "        ~Stack(){}\n"
           << "        \n"
           << "        void reset_tmp()\n"
           << "        {\n"
           << "                gap_ = stack_.size();\n"
           << "                tmp_.clear();\n"
           << "        }\n"
           << "\n"
           << "        void commit_tmp()\n"
           << "        {\n"
           << "                stack_.reserve( gap_ + tmp_.size() );                      // may throw\n"
           << "                stack_.erase( stack_.begin() + gap_, stack_.end() );       // expect not to throw\n"
           << "                stack_.insert( stack_.end(), tmp_.begin(), tmp_.end() );   // expect not to throw\n"
           << "        }\n"
           << "        bool push( const T& f )\n"
           << "        {\n"
           << "                if( StackSize != 0 && StackSize <= stack_.size() + tmp_.size() ) {\n"
           << "                    return false;\n"
           << "                }\n"
           << "                tmp_.push_back( f );\n"
           << "                return true;\n"
           << "        }\n"
           << "\n"
           << "        void pop( size_t n )\n"
           << "        {\n"
           << "                if( tmp_.size() < n ) {\n"
           << "                        n -= tmp_.size();\n"
           << "                        tmp_.clear();\n"
           << "                        gap_ -= n;\n"
           << "                } else {\n"
           << "                        tmp_.erase( tmp_.end() - n, tmp_.end() );\n"
           << "                }\n"
           << "        }\n"
           << "\n"
           << "        const T& top()\n"
           << "        {\n"
           << "                if( !tmp_.empty() ) {\n"
           << "                        return tmp_.back();\n"
           << "                } else {\n"
           << "                        return stack_[ gap_ - 1 ];\n"
           << "                }\n"
           << "        }\n"
           << "\n"
           << "        const T& get_arg( size_t base, size_t index )\n"
           << "        {\n"
           << "                size_t n = tmp_.size();\n"
           << "                if( base - index <= n ) {\n"
           << "                        return tmp_[ n - ( base - index ) ];\n"
           << "                } else {\n"
           << "                        return stack_[ gap_ - ( base - n ) + index ];\n"
           << "                }\n"
           << "        }\n"
           << "\n"
           << "        void clear()\n"
           << "        {\n"
           << "                stack_.clear();\n"
           << "        }\n"
           << "\n"
           << "private:\n"
           << "        std::vector< T > stack_;\n"
           << "        std::vector< T > tmp_;\n"
           << "        size_t gap_;\n"
           << "\n"
           << "};\n\n"
            ;
    }else {
        // bulkmemory version
        os << "template < class T, int StackSize >\n"
           << "class Stack {\n"
           << "public:\n"
           << "        Stack(){ top_ = 0; gap_ = 0; tmp_ = 0; }\n"
           << "        ~Stack(){}\n"
           << "        \n"
           << "        void reset_tmp()\n"
           << "        {\n"
           << "                for( size_t i = 0 ; i < tmp_ ; i++ ) {\n"
           << "                        at( StackSize - 1 - i ).~T(); // explicit destructor\n"
           << "                }\n"
           << "                tmp_ = 0;\n"
           << "                gap_ = top_;\n"
           << "        }\n"
           << "\n"
           << "        void commit_tmp()\n"
           << "        {\n"
           << "                for( size_t i = 0 ; i < tmp_ ; i++ ) {\n"
           << "                        if( gap_ + i < top_ ) {\n"
           << "                                at( gap_ + i ) = at( StackSize - 1 - i );\n"
           << "                        } else {\n"
           << "                                // placement new copy constructor\n"
           << "                                new ( &at( gap_ + i ) ) \n"
           << "                                    T( at( StackSize - 1 - i ) );\n"
           << "                        }\n"
           << "                        at( StackSize - 1 - i).~T(); // explicit destructor\n"
           << "                }\n"
           << "                if( gap_ + tmp_ < top_ ) {\n"
           << "                        for( int i = 0 ; i < int( top_ - gap_ - tmp_ ) ; i++ ) {\n"
           << "                                at( top_ - 1 - i ).~T(); // explicit destructor\n"
           << "                        }\n"
           << "                }\n"
           << "\n"
           << "                top_ = gap_ = gap_ + tmp_;\n"
           << "                tmp_ = 0;\n"
           << "        }\n"
           << "        \n"
           << "        bool push( const T& f )\n"
           << "        {\n"
           << "                if( StackSize <= top_ + tmp_ ) { return false; }\n"
           << "                // placement new copy constructor\n"
           << "                new( &at( StackSize - 1 - tmp_++ ) ) T( f );\n"
           << "                return true;\n"
           << "        }\n"
           << "\n"
           << "        void pop( size_t n )\n"
           << "        {\n"
           << "                size_t m = n; if( m > tmp_ ) m = tmp_;\n"
           << "\n"
           << "                for( size_t i = 0 ; i < m ; i++ ) {\n"
           << "                        at( StackSize - tmp_ + i ).~T(); // explicit destructor\n"
           << "                }\n"
           << "\n"
           << "                tmp_ -= m;\n"
           << "                gap_ -= n - m;\n"
           << "        }\n"
           << "\n"
           << "        const T& top()\n"
           << "        {\n"
           << "                if( 0 < tmp_ ) {\n"
           << "                        return at( StackSize - 1 - (tmp_-1) );\n"
           << "                } else {\n"
           << "                        return at( gap_ - 1 );\n"
           << "                }\n"
           << "        }\n"
           << "\n"
           << "        const T& get_arg( size_t base, size_t index )\n"
           << "        {\n"
           << "                if( base - index <= tmp_ ) {\n"
           << "                        return at( StackSize-1-( tmp_ - ( base - index ) ) );\n"
           << "                } else {\n"
           << "                        return at( gap_ - ( base - tmp_ ) + index );\n"
           << "                }\n"
           << "        }\n"
           << "\n"
           << "        void clear()\n"
           << "        {\n"
           << "                reset_tmp();\n"
           << "                for( size_t i = 0 ; i < top_ ; i++ ) {\n"
           << "                        at( i ).~T(); // explicit destructor\n"
           << "                }\n"
           << "                top_ = gap_ = tmp_ = 0;\n"
           << "        }\n"
           << "\n"
           << "private:\n"
           << "        T& at( size_t n )\n"
           << "        {\n"
           << "                return *(T*)(stack_ + (n * sizeof(T) ));\n"
           << "        }\n"
           << "\n"
           << "private:\n"
           << "        char stack_[ StackSize * sizeof(T) ];\n"
           << "        size_t top_;\n"
           << "        size_t gap_;\n"
           << "        size_t tmp_;\n"
           << "\n"
           << "};\n\n"
            ;
    }

    // parser class header
    std::string default_stacksize = "0";
    if( options.dont_use_stl ) {
        default_stacksize = "1024";
    }

    std::string template_parameters = "class Value, class SemanticAction, int StackSize = " + default_stacksize;
    if( options.external_token ) {
        template_parameters = "class Token, " + template_parameters;
    }
        
    os << "template < " << template_parameters << " >\n";
    os <<  "class Parser {\n";

    // public interface
    os << "public:\n"
       << "    typedef Token token_type;\n"
       << "    typedef Value value_type;\n\n"
       << "public:\n"
       << "    Parser( SemanticAction& sa ) : sa_( sa ) { reset(); }\n\n"
       << "    void reset()\n"
       << "    {\n"
       << "        error_ = false;\n"
       << "        accepted_ = false;\n"
       << "        clear_stack();\n"
       << "        reset_tmp_stack();\n"
       << "        if( push_stack( "
       << "&Parser::state_" << table.first_state() << ", "
       << "&Parser::gotof_" << table.first_state() << ", "
       << "value_type() ) ) {\n"
       << "            commit_tmp_stack();\n"
       << "        } else {\n"
       << "            sa_.stack_overflow();\n"
       << "            error_ = true;\n"
       << "        }\n"
       << "    }\n\n"
       << "    bool post( token_type token, const value_type& value )\n"
       << "    {\n"
       << "        assert( !error_ );\n"
       << "        reset_tmp_stack();\n"
       << "        while( (this->*(stack_top()->state) )( token, value ) ); // may throw\n"
       << "        if( !error_ ) {\n"
       << "            commit_tmp_stack();\n"
       << "        }\n"
       << "        return accepted_;\n"
       << "    }\n\n"
       << "    bool accept( value_type& v )\n"
       << "    {\n"
       << "        assert( accepted_ );\n"
       << "        if( error_ ) { return false; }\n"
       << "        v = accepted_value_;\n"
       << "        return true;\n"
       << "    }\n\n"
       << "    bool error() { return error_; }\n\n"
        ;

    // implementation
    os << "private:\n";
    if( options.external_token ) {
        os << "    typedef Parser< Token, Value, SemanticAction, StackSize > self_type;\n";
    } else {
        os << "    typedef Parser< Value, SemanticAction, StackSize > self_type;\n";
    }
    os << "    typedef bool ( self_type::*state_type )( token_type, const value_type& );\n"
       << "    typedef bool ( self_type::*gotof_type )( int, const value_type& );\n\n"
       << "    bool            accepted_;\n"
       << "    bool            error_;\n"
       << "    value_type      accepted_value_;\n\n"
       << "    SemanticAction& sa_;\n\n"
       << "    struct stack_frame {\n"
       << "        state_type state;\n"
       << "        gotof_type gotof;\n"
       << "        value_type value;\n\n"
       << "        stack_frame( state_type s, gotof_type g, const value_type& v )\n"
       << "            : state( s ), gotof( g ), value( v ) {}\n"
       << "    };\n\n"
        ;

    // stack operation
    os << "    Stack< stack_frame, StackSize > stack_;\n"
       << "    bool push_stack( state_type s, gotof_type g, const value_type& v )\n"
       << "    {\n"
       << "        bool f = stack_.push( stack_frame( s, g, v ) );\n"
       << "        assert( !error_ );\n"
       << "        if( !f ) {\n"
       << "            error_ = true;\n"
       << "            sa_.stack_overflow();\n"
       << "        }\n"
       << "        return f;\n"
       << "    }\n\n"
       << "    void pop_stack( size_t n )\n"
       << "    {\n"
       << "        stack_.pop( n );\n"
       << "    }\n\n"
       << "    const stack_frame* stack_top()\n"
       << "    {\n"
       << "        return &stack_.top();\n"
       << "    }\n\n"
       << "    const value_type& get_arg( size_t base, size_t index )\n"
       << "    {\n"
       << "        return stack_.get_arg( base, index ).value;\n"
       << "    }\n\n"
       << "    void clear_stack()\n"
       << "    {\n"
       << "        stack_.clear();\n"
       << "    }\n\n"
       << "    void reset_tmp_stack()\n"
       << "    {\n"
       << "        stack_.reset_tmp();\n"
       << "    }\n\n"
       << "    void commit_tmp_stack()\n"
       << "    {\n"
       << "        stack_.commit_tmp();\n"
       << "    }\n\n"
        ;

    // states handler
    for( tgt::parsing_table::states_type::const_iterator i = table.states().begin();
         i != table.states().end() ;
         ++i ) {
        const tgt::parsing_table::state& s = *i;

        // gotof header
        os << "    bool gotof_" << s.no << "( int nonterminal_index, const value_type& v )\n"
           << "    {\n";

        // gotof dispatcher
        std::stringstream ss;
        ss << "        switch( nonterminal_index ) {\n";
        bool output_switch = false;
        std::set<size_t> generated;
        for( tgt::parsing_table::rules_type::const_iterator j = table.rules().begin() ;
             j != table.rules().end() ;
             ++j ) {

            size_t nonterminal_index = std::distance(
                nonterminal_types.begin(),
                nonterminal_types.find( (*j).left().name() ) );
            if( generated.find( nonterminal_index ) != generated.end() ) {
                continue;
            }

            tgt::parsing_table::state::goto_table_type::const_iterator k =
                (*i).goto_table.find( (*j).left() );

            if( k != (*i).goto_table.end() ) {


                ss << "        case " << nonterminal_index
                   << ": return push_stack( &Parser::state_" << (*k).second
                   << ", &Parser::gotof_" << (*k).second
                   << ", v );\n";
                output_switch = true;
                generated.insert( nonterminal_index );
            }
        }
        ss << "        default: assert(0); return false;\n"; 
        ss << "        }\n";
        if( output_switch ) {
            os << ss.str();
        } else {
            os << "        assert(0);\n"
               << "        return true;\n";
        }

        // gotof footer
        os << "    }\n\n";

        // state header
        os << "    bool state_" << s.no << "( token_type token, const value_type& value )\n";
        os << "    {\n";

        // dispatcher header
        os << "        switch( token ) {\n";

        // action table
        for( tgt::parsing_table::state::action_table_type::const_iterator j = s.action_table.begin();
             j != s.action_table.end() ;
             ++j ) {
            // action header 
            os << "        case " << options.token_prefix
               << (*token_id_map.find( (*j).first )).second << ":\n";

            // action
            const tgt::parsing_table::action* a = &(*j).second;
            switch( a->type ) {
            case zw::gr::action_shift:
                os << "            // shift\n"
                   << "            push_stack( "
                   << "&Parser::state_" << a->dest_index << ", "
                   << "&Parser::gotof_" << a->dest_index << ", "
                   << "value );\n"
                   << "            return false;\n";
                break;
            case zw::gr::action_reduce:
                os << "            // reduce\n";
                {
                    size_t base = table.rules()[ a->rule_index ].right().size();
                                        
                    const tgt::parsing_table::rule_type& rule = table.rules()[a->rule_index];
                    action_map_type::const_iterator k = actions.find( rule );

                    size_t nonterminal_index = std::distance(
                        nonterminal_types.begin(),
                        nonterminal_types.find( rule.left().name() ) );

                    if( k != actions.end() ) {
                        const semantic_action& sa = (*k).second;

                        os << "            {\n";
                        // automatic argument conversion
                        for( size_t l = 0 ; l < sa.args.size() ; l++ ) {
                            const semantic_action_argument& arg =
                                (*sa.args.find( l )).second;
                            os << "                " << arg.type << " arg" << l << "; "
                               << "sa_.downcast( arg" << l << ", get_arg(" << base
                               << ", " << arg.src_index << ") );\n";
                        }

                        // semantic action
                        os << "                "
                           << (*nonterminal_types.find( rule.left().name() )).second
                           << " r = sa_." << sa.name << "( ";
                        bool first = true;
                        for( size_t l = 0 ; l < sa.args.size() ; l++ ) {
                            if( first ) { first = false; } else { os << ", "; }
                            os << "arg" << l;
                        }
                        os << " );\n";

                        // automatic return value conversion
                        os << "                value_type v; sa_.upcast( v, r );\n";
                        os << "                pop_stack( "
                           << base
                           << " );\n";
                        os << "                return (this->*(stack_top()->gotof))( "
                           << nonterminal_index << ", v );\n";
                        os << "            }\n";
                    } else {
                        os << "            // run_semantic_action();\n";
                        os << "            pop_stack( "
                           << base
                           << " );\n";
                        os << "            return (this->*(stack_top()->gotof))( "
                           << nonterminal_index << ", value_type() );\n";
                    }
                }
                break;
            case zw::gr::action_accept:
                os << "            // accept\n"
                   << "            // run_semantic_action();\n"
                   << "            accepted_ = true;\n"
                   << "            accepted_value_  = get_arg( 1, 0 );\n" // implicit root
                   << "            return false;\n";
                break;
            case zw::gr::action_error:
                os << "            sa_.syntax_error();\n";
                os << "            error_ = true;\n"; 
                os << "            return false;\n";
                break;
            }

            // action footer
        }

        // dispatcher footer
        os << "        default:\n"
           << "            sa_.syntax_error();\n"
           << "            error_ = true;\n"
           << "            return false;\n";
        os << "        }\n";

        // state footer
        os << "    }\n\n";
    }

    // parser class footer
    os << "};\n\n";

    // namespace footer
    os << "} // namespace " << options.namespace_name << "\n\n";

    // once footer
    os << "#endif // #ifndef " << headername << "_\n";
}
