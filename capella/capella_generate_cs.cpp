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
    std::string     name_space;
    std::string     basename;
    inheritance_type inheritance;
    typeset_type    types;
    atomset_type    atoms;
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

    void operator()(const NamespaceDef& x) const {
        context.name_space = x.name.s;
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
// StructDeclarator
struct StructDeclarator : public boost::static_visitor<void> {
    StructDeclarator(
        std::ostream&           a,
        Context&                b)
        : os(a), context(b) {}

    std::ostream&           os;
    Context&                context;
    
    template <class T>
    void operator()(const T&) const {}

    template <class T>
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& e: x) {
            boost::apply_visitor(
                StructDeclarator(os, context), e);
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
            inh = " : " + (*i).second;
        } else if (context.basename != "") {
            inh = " : " + context.basename;
        }

        switch (x.right.which()) {
            case 1: // Scalor
                {
                    Scalor s = boost::get<Scalor>(x.right);
                    os << "public class " << x.name.s << inh << " {\n";
                    os << context.class_header;

                    if (find_in(context.atoms, s.type.s)) {
                        os << "    public " << s.type.s << " " << s.name.s << " { get; set; }\n\n";
                    } else if (find_in(context.types, s.type.s)) {
                        os << "    public " << s.type.s << " " << s.name.s << " { get; set; }\n\n";
                    } else {
                        throw undefined_type(s.type.s);
                    }
                    os << "    public " << x.name.s << "() {}\n";
                    if (context.atoms.find(s.type.s) != context.atoms.end()) {
                        os << "    public " << x.name.s << "(" << s.type.s << " x) {" << s.name.s << " = x; }\n";
                    } else {
                        os << "    public " << x.name.s << "(" << s.type.s << " x) {" << s.name.s << " = x; }\n";
                    }
                    os << "    public void Deconstruct(out " << s.type.s << " x) {\n";
                    os << "        x = " << s.name.s << ";\n";
                    os << "    }\n";

                    os << context.class_footer;
                    os << "};\n\n";
                    break;
                }
            case 2: // List
                {
                    List s = boost::get<List>(x.right);
                    os << "public class " << x.name.s << inh << " {\n";
                    os << context.class_header;

                    if (find_in(context.atoms, s.etype.s)) {
                        os << "    public List<" << s.etype.s << "> " << s.name.s << " { get; set; }\n\n";
                    } else if (find_in(context.types, s.etype.s)) {
                        os << "    public List<" << s.etype.s<< "> "
                           << s.name.s << " { get; set; }\n\n";
                    } else {
                        throw undefined_type(s.etype.s);
                    }
                    os << "    public " << x.name.s << "() {}\n";
                    if (find_in(context.atoms, s.etype.s)) {
                        os << "    public " << x.name.s << "(" << s.etype.s << " x) { "
                           << s.name.s << ".Add(x); }\n";
                    } else if (find_in(context.types, s.etype.s)) {
                        os << "    public " << x.name.s << "(" << s.etype.s << " x) { "
                           << s.name.s << ".Add(x); }\n";
                    } else {
                        throw undefined_type(s.etype.s);
                    }
                        
                    os << "    public " << x.name.s << "(IEnumerable<" << s.etype.s << "> v) { " << s.name.s << " = v.ToList(); }\n";
                    
                    os << "    public void Deconstruct(out List<" << s.etype.s << "> x) {\n";
                    os << "        x = " << s.name.s << ";\n";
                    os << "    }\n";

                    os << context.class_footer;
                    os << "};\n\n";

                    break;
                }
            case 3: // Variant
                {
                    Variant s = boost::get<Variant>(x.right);
                    os << "public abstract class " << x.name.s << inh << " {\n";
                    os << context.class_header;
                        
                    os << context.class_footer;
                    os << "};\n\n";
                    break;
                }
            case 4: // Tuple
                {
                    Tuple s = boost::get<Tuple>(x.right);
                    os << "public class " << x.name.s << inh << " {\n";
                    os << context.class_header;

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
                                    members.push_back(make_pair(t.type.s, t.name.s));
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
                                            "List<" + t.etype.s + ">",
                                            t.name.s));
                                } else if (find_in(context.types, t.etype.s)) {
                                    members.push_back(
                                        make_pair(
                                            "List<" + t.etype.s+ ">",
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
                        os << "    public ";
                        os << e.first;
                        for (size_t j = 0 ; j <column - e.first.length(); j++) {
                            os << ' ';
                        }
                        os << e.second << ";\n";
                    }

                    // コンストラクタ宣言
                    os << "\n";
                    os << "    public " << x.name.s << "() {}\n";
                    os << "    public " << x.name.s << "(";
                    n = 0;
                    for (const auto& ti: s.elements) {
                        if (n != 0) { os << ", "; }
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                if (find_in(context.atoms, t.type.s)) {
                                    os << t.type.s << " a" << n;
                                } else if (find_in(context.types, t.type.s)) {
                                    os << t.type.s << " a" << n;
                                } else {
                                    throw undefined_type(t.type.s);
                                }
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                if (find_in(context.atoms, t.etype.s)) {
                                    os << "List<" << t.etype.s << "> a" << n;
                                } else if (find_in(context.types, t.etype.s)) {
                                    os << "List<" << t.etype.s << "> a" << n;
                                } else {
                                    throw undefined_type(t.etype.s);
                                }
                                break;
                            }
                        }
                        n++;
                    }
                    os << ") {\n";

                    // メンバ初期化宣言
                    n = 0;
                    for (const auto& ti: s.elements) {
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                os << "        " << t.name.s << " = a" << n << ";\n";
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                os << "        " << t.name.s << " = a" << n << ";\n";
                                break;
                            }
                        }
                        n++;
                    }
                    os << "    }\n";

                    // Deconstruct
                    os << "    public void Deconstruct(";
                    n = 0;
                    for (const auto& ti: s.elements) {
                        if (0 < n) { os << ", "; }
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                os << "out " << t.type.s << " a" << n;
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                os << "out " << t.etype.s << " a" << n;
                                break;
                            }
                        }
                        n++; 
                    }
                    os << ") {\n";
                    n = 0;
                    for (const auto& ti: s.elements) {
                        switch (ti.which()) {
                            case 0: {
                                Scalor t = boost::get<Scalor>(ti);
                                os << "        a" << n << " = " << t.name.s << ";\n";
                                break;
                            }
                            case 1: {
                                List t = boost::get<List>(ti);
                                os << "        a" << n << " = " << t.name.s << ";\n";
                                break;
                            }
                        }
                        n++; 
                    }
                    os << "    }\n";

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
void generate_cs(
    const std::string&      filename,
    std::ostream&           os,
    Dependency&,
    const typeset_type&     types,
    const atomset_type&     atoms,
    const Value&            v) {

    Context context;
    context.types = types;
    context.atoms = atoms;
    
    InheritanceCollector ic(context);
    boost::apply_visitor(ic, v.data);

    os << "using System;\n";
    os << "using System.Collections;\n";
    os << "using System.Collections.Generic;\n";
    os << "using System.Linq;\n";
    os << "\n";
    
    if (context.name_space != "") {
        os << "namespace " << context.name_space << " {\n\n";
    }

    os << context.module_header;

    os << "////////////////////////////////////////////////////////////////\n"
       << "// class declarations\n";
    StructDeclarator sd(os, context);
    boost::apply_visitor(sd, v.data);

    os << context.module_footer;

    if (context.name_space != "") {
        os << "} // namespace " << context.name_space << "\n";
    }
}
