#include "capella_generate_cpp.hpp"
#include "capella_dependency.hpp"
#include <iostream>

namespace {

typedef std::map<std::string, std::string> inheritance_type;

template <class S, class K>
inline
bool find_in(const S& set, const K& key) {
    return set.find(key) != set.end();
}

struct Context {
    std::string     class_header;
    std::string     class_footer;
    std::string     module_header;
    std::string     module_footer;
    std::string     basename;
    inheritance_type inheritance;
    typeset_type    types;
    atomset_type    atoms;
    int             tagid;
    bool            cpp11;
};

////////////////////////////////////////////////////////////////
// InheritanceCollector
struct InheritanceCollector : public boost::static_visitor<void> {
    InheritanceCollector(Context& c) : context(c) {}
    Context& context;
        
    template <class T>
    void operator()(const T&) const {}

    template <class T>
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& e: x) {
            boost::apply_visitor(InheritanceCollector(context), e);
        }
    }
        
    void operator()(const Module& x) const {
        apply_to_vector(x.declarations);
    }

    void operator()(const BaseDef& x) const {
        context.basename = x.name.s;
    }

    void operator()(const ClassHeaderDef& x) const {
        context.class_header = x.data.s;
    }

    void operator()(const ClassFooterDef& x) const {
        context.class_footer = x.data.s;
    }

    void operator()(const ModuleHeaderDef& x) const {
        context.module_header = x.data.s;
    }

    void operator()(const ModuleFooterDef& x) const {
        context.module_footer = x.data.s;
    }

    void operator()(const TypeDef& x) const {
        if (x.right.which() == 3) {
            Variant v = boost::get<Variant>(x.right);
            for (const auto& e: v.choises) {
                context.inheritance[e.s] = x.name.s;
            }
        }
    }
};

////////////////////////////////////////////////////////////////
// ForwardDeclarator
struct ForwardDeclarator : public boost::static_visitor<void> {
    ForwardDeclarator(std::ostream& x, const std::string& y)
        : os(x), basename(y){ }
    std::ostream&           os;
    const std::string&      basename;

    template <class T>
    void operator()(const T&) const {}

    template < class T >
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& e: x) {
            boost::apply_visitor(ForwardDeclarator(os, basename), e);
        }
    }
        
    void operator()(const Module& x) const {
        apply_to_vector(x.declarations);
    }

    void operator()(const TypeDef& x) const {
        os << "struct " << x.name.s << ";" << std::endl;
    }
};

////////////////////////////////////////////////////////////////
// TagDeclarator
struct TagDeclarator : public boost::static_visitor<void> {
    TagDeclarator(std::ostream& x, Context& y) : os(x), context(y) {}

    std::ostream&   os;
    Context&        context;
    
    template <class T>
    void operator()(const T&) const {}

    template <class T>
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& e: x) {
            boost::apply_visitor(TagDeclarator(os, context), e);
        }
    }
        
    void operator()(const Module& x) const {
        apply_to_vector(x.declarations);
    }

    void operator()(const TypeDef& x) const {
        std::string tagname = x.name.s;
        for (auto& e: tagname) {
            e = toupper(e);
        }
        if (context.cpp11) {
            os << "    " << tagname << " = " << context.tagid++ << ", \n";
        } else {
            os << "    TAG_" << tagname << " = " << context.tagid++ << ", \n";
        }
    }
};

////////////////////////////////////////////////////////////////
// StructDeclarator
template <class PM>
struct StructDeclarator : public boost::static_visitor<void> {
    StructDeclarator(
        std::ostream&           a,
        Context&                b,
        PM                      c)
        : os(a), context(b), pointer_maker(c) {}

    std::ostream&           os;
    Context&                context;
    PM                      pointer_maker;
    
    template <class T>
    void operator()(const T&) const {}

    template <class T>
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& e: x) {
            boost::apply_visitor(
                StructDeclarator(os, context, pointer_maker), e);
        }
    }
        
    void operator()(const Module& x) const {
        apply_to_vector(x.declarations);
    }

    void operator()(const TypeDef& x) const {
        std::string inh;
        inheritance_type::const_iterator i =
            context.inheritance.find(x.name.s);
        if (i != context.inheritance.end()) {
            inh = " : public " + (*i).second;
        } else if (context.basename != "") {
            inh = " : public " + context.basename;
        }

        std::string tagname = x.name.s;
        for (auto& e: tagname) {
            e = toupper(e);
        }

        switch (x.right.which()) {
            case 1: // Scalor
                {
                    Scalor s = boost::get<Scalor>(x.right);
                    os << "struct " << x.name.s << inh << " {\n";
                    os << context.class_header;
                    os << "public:\n";
                    if (context.cpp11) {
                        os << "    int tag(){ return (int)Tag::" << tagname << "; }\n\n";
                    } else {
                        os << "    int tag(){ return (int)TAG_" << tagname << "; }\n\n";
                    }

                    os << "public:\n";
                    if (find_in(context.atoms, s.type.s)) {
                        os << "    " << s.type.s << " " << s.name.s << ";\n\n";
                    } else if (find_in(context.types, s.type.s)) {
                        os << "    " << pointer_maker(s.type.s)<< " " << s.name.s << ";\n\n";
                    } else {
                        throw undefined_type(s.type.s);
                    }
                    os << "public:\n";
                    os << "    " << x.name.s << "() {}\n";
                    if (context.atoms.find(s.type.s) != context.atoms.end()) {
                        os << "    " << x.name.s << "(const " << s.type.s << "& x)\n"
                           << "        : " << s.name.s << "(x) {}\n";
                    } else {
                        os << "    " << x.name.s << "(" << pointer_maker(s.type.s) << "x)\n"
                           << "        : " << s.name.s << "(x) {}\n";
                    }
                    os << context.class_footer;
                    os << "};\n\n";
                    break;
                }
            case 2: // List
                {
                    List s = boost::get<List>(x.right);
                    os << "struct " << x.name.s << inh << " {\n";
                    os << context.class_header;
                    os << "public:\n";
                    if (context.cpp11) {
                        os << "    int tag(){ return (int)Tag::" << tagname << "; }\n\n";
                    } else {
                        os << "    int tag(){ return (int)TAG_" << tagname << "; }\n\n";
                    }

                    os << "public:\n";
                    if (find_in(context.atoms, s.etype.s)) {
                        os << "    std::vector<" << s.etype.s << "> " << s.name.s << ";\n\n";
                    } else if (find_in(context.types, s.etype.s)) {
                        os << "    std::vector<" << pointer_maker(s.etype.s)<< "> "
                           << s.name.s << ";\n\n";
                    } else {
                        throw undefined_type(s.etype.s);
                    }
                    os << "public:\n";
                    os << "    " << x.name.s << "() {}\n";
                    if (find_in(context.atoms, s.etype.s)) {
                        os << "    " << x.name.s << "(const " << s.etype.s << "& x) { "
                           << s.name.s << ".push_back(x); }\n";
                    } else if (find_in(context.types, s.etype.s)) {
                        os << "    " << x.name.s << "(" << pointer_maker(s.etype.s)<< " x) { "
                           << s.name.s << ".push_back(x); }\n";
                    } else {
                        throw undefined_type(s.etype.s);
                    }
                        
                    os << "    template <class V>\n"
                       << "    " << x.name.s << "(const V& v)\n"
                       << "        : " << s.name.s << "(std::begin(v), std::end(v)) {}\n";
                    
                    os << context.class_footer;
                    os << "};\n\n";
                    break;
                }
            case 3: // Variant
                {
                    Variant s = boost::get<Variant>(x.right);
                    os << "struct " << x.name.s << inh << " {\n";
                    os << context.class_header;
                    os << "public:\n";
                    if (context.cpp11) {
                        os << "    int tag(){ return (int)Tag::" << tagname << "; }\n\n";
                    } else {
                        os << "    int tag(){ return (int)TAG_" << tagname << "; }\n\n";
                    }
                        
                    if (inh == "") {
                        os << "    ~" << x.name.s << "() {}\n";
                    }
                    os << context.class_footer;
                    os << "};\n\n";
                    break;
                }
            case 4: // Tuple
                {
                    Tuple s = boost::get<Tuple>(x.right);
                    os << "struct " << x.name.s << inh << " {\n";
                    os << context.class_header;
                    os << "public:\n";
                    if (context.cpp11) {
                        os << "    int tag(){ return (int)Tag::" << tagname << "; }\n\n";
                    } else {
                        os << "    int tag(){ return (int)TAG_" << tagname << "; }\n\n";
                    }

                    // メンバ宣言
                    std::vector<std::pair<std::string, std::string>> members;                        

                    int n = 0;
                    for (const auto& ti: s.elements) {
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                if (find_in(context.atoms, t.type.s)) {
                                    members.push_back(make_pair(t.type.s, t.name.s));
                                } else if (find_in(context.types, t.type.s)) {
                                    members.push_back(make_pair(pointer_maker(t.type.s), t.name.s));
                                } else {
                                    throw undefined_type(t.type.s);
                                }
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                if (find_in(context.atoms, t.etype.s)) {
                                    members.push_back(
                                        make_pair(
                                            "std::vector<" + t.etype.s + ">",
                                            t.name.s));
                                } else if (find_in(context.types, t.etype.s)) {
                                    members.push_back(
                                        make_pair(
                                            "std::vector<" + pointer_maker(t.etype.s)+ ">",
                                            t.name.s));
                                } else {
                                    throw undefined_type(t.etype.s);
                                }
                                break;
                            }
                        }
                        n++;
                    }

                    int column = 0;
                    for (const auto& e: members) {
                        int c = e.first.length()+ 1;
                        if (column < c) {
                            column = c;
                        }
                    }

                    for (const auto& e: members) {
                        os << "    ";
                        os << e.first;
                        for (size_t j = 0 ; j <column - e.first.length(); j++) {
                            os << ' ';
                        }
                        os << e.second << ";\n";
                    }

                    // コンストラクタ宣言
                    os << "\n";
                    os << "    " << x.name.s << "() {}\n";
                    os << "    " << x.name.s << "(";
                    n = 0;
                    for (const auto& ti: s.elements) {
                        if (n != 0) { os << ", "; }
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                if (find_in(context.atoms, t.type.s)) {
                                    os << "const " << t.type.s << "& a" << n;
                                } else if (find_in(context.types, t.type.s)) {
                                    os << pointer_maker(t.type.s)<< " a" << n;
                                } else {
                                    throw undefined_type(t.type.s);
                                }
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                if (find_in(context.atoms, t.etype.s)) {
                                    os << "const std::vector<" << t.etype.s << ">& a" << n;
                                } else if (find_in(context.types, t.etype.s)) {
                                    os << "const std::vector<" << pointer_maker(t.etype.s)
                                       << ">& a" << n;
                                } else {
                                    throw undefined_type(t.etype.s);
                                }
                                break;
                            }
                        }
                        n++;
                    }
                    os << ")\n"
                       << "        : ";

                    // メンバ初期化宣言
                    n = 0;
                    for (const auto& ti: s.elements) {
                        if (n != 0) { os << ", "; }
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                os << t.name.s << "(a" << n << ")";
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                os << t.name.s << "(a" << n << ")";
                                break;
                            }
                        }
                        n++;
                    }
                    os << "{}\n";
                    os << context.class_footer;
                    os << "};\n\n";
                    break;
                }
        }
    }
};

} // namespace

////////////////////////////////////////////////////////////////
//
template <class T>
void generate_cpp(
    const std::string&      filename,
    std::ostream&           os,
    Dependency&,
    const typeset_type&     types,
    const atomset_type&     atoms,
    const Value&            v,
    T                       pointer_maker,
    bool                    cpp11) {
    std::string headername = filename;
    for (auto& e: headername) {
        if (!isalpha(e)) { e = '_'; } else { e = toupper(e); }
    }
    os << "#ifndef " << headername << "\n";
    os << "#define " << headername << "\n\n";

    Context context;
    context.types = types;
    context.atoms = atoms;
    context.cpp11 = cpp11;
    
    InheritanceCollector ic(context);
    boost::apply_visitor(ic, v.data);

    os << "#include <vector>\n";
    if (cpp11) {
        os << "#include <memory>\n";
    }
    os << "\n";
    
    os << context.module_header;

    os << "////////////////////////////////////////////////////////////////\n"
       << "// forward declarations\n";
    ForwardDeclarator fd(os, context.basename);
    boost::apply_visitor(fd, v.data);
    os << std::endl;

    os << "////////////////////////////////////////////////////////////////\n"
       << "// tag declarations\n";
    if (cpp11) {
        os << "enum class Tag { \n";
    } else {
        os << "enum Tag { \n";
    }
    context.tagid = 1;
    TagDeclarator td(os, context);
    boost::apply_visitor(td, v.data);
    os << " };\n\n";

    os << "////////////////////////////////////////////////////////////////\n"
       << "// struct declarations\n";
    context.tagid = 1;
    StructDeclarator<T> sd(os, context, pointer_maker);
    boost::apply_visitor(sd, v.data);

    os << context.module_footer;

    os << "#endif // " << headername << "\n";
}

struct shared_ptr_maker {
    std::string operator()(const std::string& x) const {
        return "boost::shared_ptr<" + x + ">";
    }
};

struct shared_ptr_cpp11_maker {
    std::string operator()(const std::string& x) const {
        return "std::shared_ptr<" + x + ">";
    }
};

struct normal_ptr_maker {
    std::string operator()(const std::string& x) const {
        return x + "*";
    }
};

////////////////////////////////////////////////////////////////
//
void generate_cpp_normal(
    const std::string&  filename,
    std::ostream&       os,
    Dependency&         dependency,
    const typeset_type& types,
    const atomset_type& atoms,
    const Value&        v) {
    generate_cpp(
        filename, os, dependency, types, atoms, v,
        normal_ptr_maker(), false);
}

////////////////////////////////////////////////////////////////
//
void generate_cpp_shared(
    const std::string&  filename,
    std::ostream&       os,
    Dependency&         dependency,
    const typeset_type& types,
    const atomset_type& atoms,
    const Value&        v) {

    generate_cpp(
        filename, os, dependency, types, atoms, v,
        shared_ptr_maker(), false);
}

////////////////////////////////////////////////////////////////
//
void generate_cpp11_normal(
    const std::string&  filename,
    std::ostream&       os,
    Dependency&         dependency,
    const typeset_type& types,
    const atomset_type& atoms,
    const Value&        v) {
    generate_cpp(
        filename, os, dependency, types, atoms, v,
        normal_ptr_maker(), true);
}

////////////////////////////////////////////////////////////////
//
void generate_cpp11_shared(
    const std::string&  filename,
    std::ostream&       os,
    Dependency&         dependency,
    const typeset_type& types,
    const atomset_type& atoms,
    const Value&        v) {

    generate_cpp(
        filename, os, dependency, types, atoms, v,
        shared_ptr_cpp11_maker(), true);
}

