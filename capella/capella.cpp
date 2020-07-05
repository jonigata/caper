// Copyright (C) 2006 Naoyuki Hirayama.
// All Rights Reserved.

// $Id$

#include "capella_ast.hpp"
#include "capella.hpp"
#include "capella_dependency.hpp"
#include "capella_generate_cpp.hpp"
#include "capella_generate_boost.hpp"
#include "capella_generate_stub.hpp"
#include "capella_generate_dot.hpp"
#include "capella_generate_cs.hpp"
#include <set>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstring>

using namespace capella;

////////////////////////////////////////////////////////////////
// scanner
template <class It>
class scanner {
public:
    typedef int char_type;

public:
    scanner(It b, It e) : b_(b), e_(e), c_(b), unget_count_(0) {
        addr_ = 0;
        reserved_["type"] = token_TypeDef;
        reserved_["base"] = token_BaseDef;
        reserved_["atom"] = token_AtomDef;
        reserved_["class-header"] = token_ClassHeaderDef;
        reserved_["class-footer"] = token_ClassFooterDef;
        reserved_["module-header"] = token_ModuleHeaderDef;
        reserved_["module-footer"] = token_ModuleFooterDef;
        reserved_["namespace"] = token_NamespaceDef;
        lines_.push_back(0);
    }
    ~scanner(){}

    int addr() { return addr_; }

    int lineno(int addr) {
        std::vector<int>::const_iterator i = std::upper_bound(lines_.begin(), lines_.end(), addr);
        assert(i != lines_.begin());
        return int(i - lines_.begin());
    }
    int column(int addr) {
        std::vector<int>::const_iterator i = std::upper_bound(lines_.begin(), lines_.end(), addr);
        assert(i != lines_.begin());
        --i;
        return addr - *i;
    }

    Token get(Value& v) {
        int c;
        do {
            c = sgetc();
        } while (isspace(c));

        int b = addr_ - 1;

        // 記号類
        switch (c) {
            case '(': v = Value(range(b, addr_), Operator('(')); return token_LParen;
            case ')': v = Value(range(b, addr_), Operator(')')); return token_RParen;
            case '|': v = Value(range(b, addr_), Operator('|')); return token_Pipe;
            case '*': v = Value(range(b, addr_), Operator('=')); return token_List;
            case '=': v = Value(range(b, addr_), Operator('=')); return token_Equal;
            case ',': v = Value(range(b, addr_), Operator(':')); return token_Comma;
            case ';': v = Value(range(b, addr_), Operator(';')); return token_Semicolon;
            case EOF: v = Value(range(b, addr_), Operator('$')); return token_eof;
        }

        // 識別子
        if (isalpha(c) || c == '_' || c == ':') {
            std::stringstream ss;
            while (c != EOF && (isalpha(c) || isdigit(c) || c == '-' || c == '_' || c == ':' || c == '.')) {
                if (c == ':') {
                    c = sgetc();
                    if (c == ':') {
                        ss << "::";
                    } else {
                        sungetc(c);
                        break;
                    }
                } else {
                    ss << (char)c;
                }
                c = sgetc();
            }
            sungetc(c);

            reserved_type::const_iterator  i = reserved_.find(ss.str());
            if (i != reserved_.end()) {
                v = Value(range(b, addr_), Reserved(ss.str()));
                return (*i).second;
            } else {
                v = Value(range(b, addr_), Identifier(ss.str()));
                return token_Identifier;
            }
        }

        // 埋め込み
        if (c == '@') {
            c = sgetc();
            if (c != '{') {
                throw unexpected_char(addr_, c);
            }

            bool first_cr = true;

            std::stringstream ss;
            for (;;) {
                c = sgetc();
                if (c == '}') {
                    c = sgetc();
                    if (c == '@') {
                        break;
                    }
                    sungetc(c);
                    ss << '}';
                } else if (c == EOF) {
                    throw mismatch_paren(addr_, c);
                } else if (c == '\n') {
                    if (first_cr) {
                        std::string s = ss.str();
                        size_t i;
                        for (i = 0 ; i < s.length(); i++) {
                            if (!isspace(s[i])) {
                                break;
                            }
                        }
                        if (i == s.length()) {
                            ss.str("");
                        } else {
                            ss << (char)c;
                        }
                        first_cr = false;
                    } else {
                        ss << (char)c;
                    }
                } else {
                    ss << (char)c;
                }
            }
            v = Value(range(b, addr_), BulkText(ss.str()));
            return token_BulkText;
        }

        throw unexpected_char(addr_, c);
    }

private:
    char_type sgetc() {
        int c;
        if (0 < unget_count_) {
            c = unget_[--unget_count_];
        } else if (c_ == e_) {
            c = EOF; 
        } else {
            c = *c_++;
            if (c == '\n') {
                lines_.push_back(addr_+1);
            }
        }
        addr_++;
        return c;
    }

    void sungetc(char_type c) {
        if (c != EOF) {
            addr_--;
            unget_[unget_count_++] = c;
        }
    }

    void push_paren(std::vector<char>& stack, int c) {
        assert(c == '{');
        stack.push_back(c);
    }

    void pop_paren(std::vector<char>& stack, int c) {
        assert(!stack.empty());
        switch (c) {
            case '}':
                if (stack.back() != '{') {
                    throw mismatch_paren(addr_, c);
                }
                break;
            default: assert(0);
        }
        stack.pop_back();
    }

private:
    Range range(int b, int e) { return Range(b, e); }

private:
    It              b_;
    It              e_;
    It              c_;
    int             unget_count_;
    char_type       unget_[2];
    int             addr_;

    typedef std::map<std::string, Token> reserved_type;
    reserved_type reserved_;

    std::vector<int> lines_;
};

////////////////////////////////////////////////////////////////
// SemanticAction
struct SemanticAction {
    void syntax_error(){}
    void stack_overflow(){}

    template <class T>
    void downcast(T& x, Value y) { x = boost::get<T>(y.data); }

    template <class T>
    void upcast(Value& x, const T& y) { x.data = Node(y); }

    template <class T>
    Module MakeModule(const T& x) {
        return Module(x);
    }
    Declaration MakeBaseDef(const Identifier& y) {
        return BaseDef(y);
    }
    template <class T>
    Declaration MakeAtomDef(const T& x){
        return AtomDef(x);
    }
    Atom MakeAtom0(const Identifier& x) {
        Atom z;
        z.name = x;
        return z;
    }
    Atom MakeAtom1(const Identifier& x, const Identifier& y) {
        Atom z;
        z.name = x;
        z.type = y;
        return z;
    }
    Declaration MakeClassHeaderDef(const BulkText& x) {
        return ClassHeaderDef(x);
    }
    Declaration MakeClassFooterDef(const BulkText& x) {
        return ClassFooterDef(x);
    }
    Declaration MakeModuleHeaderDef(const BulkText& x) {
        return ModuleHeaderDef(x);
    }
    Declaration MakeModuleFooterDef(const BulkText& x) {
        return ModuleFooterDef(x);
    }
    Declaration MakeNamespaceDef(const Identifier& x) {
        return NamespaceDef(x);
    }
    Declaration MakeTypeDef(const Identifier& x, const TypeDefRight& y) {
        return TypeDef(x, y);
    }
    TypeDefRight MakeScalor(const Identifier& x, const Identifier& y) {
        return Scalor(x, y);
    }
    TypeDefRight MakeList(const Identifier& x, const Identifier& y) {
        return List(x, y);
    }
    TypeDefRight MakeVariant(const Variant& x) {
        return x;
    }
    Variant MakeVariant0(const Identifier& x) {
        Variant z;
        z.choises.push_back(x);
        return z;
    }
    Variant MakeVariant1(const Variant& x, const Identifier& y) {
        Variant z = x;
        z.choises.push_back(y);
        return z;
    }
    TypeDefRight MakeTuple(const Tuple& x) {
        return x;
    }
    Tuple MakeTuple0(const TupleItem& x, const TupleItem& y) {
        Tuple z;
        z.elements.push_back(x);
        z.elements.push_back(y);
        return z;
    }
    Tuple MakeTuple1(const Tuple& x, const TupleItem& y) {
        Tuple z = x;
        z.elements.push_back(y);
        return z;
    }
    TupleItem MakeTupleItem0(const Identifier& x, const Identifier& y) {
        return Scalor(x, y);
    }
    TupleItem MakeTupleItem1(const Identifier& x, const Identifier& y) {
        return List(x, y);
    }
};

////////////////////////////////////////////////////////////////
// IdentifierCollector
struct IdentifierCollector : public boost::static_visitor<void> {
    IdentifierCollector(typeset_type& t, atomset_type& s)
        : types(t), atoms(s) {}
    typeset_type& types;
    atomset_type& atoms;

    template <class T>
    void operator()(const T&) const {}

    template <class T>
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& y: x) {
            boost::apply_visitor(IdentifierCollector(types, atoms), y);
        }
    }
        
    void operator()(const Module& x) const {
        apply_to_vector(x.declarations);
    }

    void operator()(const TypeDef& x) const {
        types.insert(x.name.s);
    }

    void operator()(const AtomDef& x) const {
        for (const auto& y: x.atoms) {
            atoms.insert(y.name.s);
        }
    }
};

struct commandline_options {
    std::string     infile;
    std::string     outfile;
    std::string     language;
};

void get_commandline_options(
    commandline_options&    cmdopt,
    int                     argc,
    char**                  argv) {
    cmdopt.language = "C++";

    int state = 0;
    for (int index = 1 ; index < argc ; index++) {
        std::string arg = argv[index];
        if (arg[0] == '-') {
            if (arg == "-c++" || arg == "-cpp") {
                cmdopt.language = "C++";
                continue;
            }
            if (arg == "-c++-shared" || arg == "-cpp-shared") {
                cmdopt.language = "C++-shared";
                continue;
            }
            if (arg == "-c++-variant" || arg == "-cpp-variant") {
                cmdopt.language = "C++-variant";
                continue;
            }
            if (arg == "-c++-stub" || arg == "-cpp-stub") {
                cmdopt.language = "C++-stub";
                continue;
            }
            if (arg == "-c++-shared-stub" || arg == "-cpp-shared-stub") {
                cmdopt.language = "C++-shared-stub";
                continue;
            }
            if (arg == "-c++-variant-stub" || arg == "-cpp-variant-stub") {
                cmdopt.language = "c++-variant-stub";
                continue;
            }
            if (arg == "-c++11" || arg == "-cpp11") {
                cmdopt.language = "C++11";
                continue;
            }
            if (arg == "-c++11-shared" || arg == "-cpp11-shared") {
                cmdopt.language = "C++11-shared";
                continue;
            }
            if (arg == "-dot") {
                cmdopt.language = "dot";
                continue;
            }
            if (arg == "-cs") {
                cmdopt.language = "C#";
                continue;
            }
            std::cerr << "unknown option: " << arg << std::endl;
            exit(1);
        }

        switch (state) {
            case 0: cmdopt.infile = arg; state++; break;
            case 1: cmdopt.outfile = arg; state++; break;
            default:
                std::cerr << "too many arguments" << std::endl;
                exit(1);
        }
    }

    if (state < 2) {
        std::cerr << "usage: capella [-c++ | -c++-shared | -boost | -dot] input_filename output_filename\n";
        exit(1);
    }

}

////////////////////////////////////////////////////////////////
// main
int main(int argc, char** argv) {
    commandline_options cmdopt;
    get_commandline_options(cmdopt, argc, argv);

    typedef  void(*generator_type)(
        const std::string&,
        std::ostream&,
        Dependency&,
        const typeset_type& types,
        const atomset_type& atoms,
        const Value&);

    std::map<std::string, generator_type> generators;
    generators["C++"] = generate_cpp_normal;
    generators["C++-shared"] = generate_cpp_shared;
    generators["C++-variant"] = generate_cpp_variant;
    generators["C++-stub"] = generate_stub_cpp_normal;
    generators["C++-shared-stub"] = generate_stub_cpp_shared;
    generators["C++-variant-stub"] = generate_stub_cpp_variant;
    generators["C++11"] = generate_cpp11_normal;
    generators["C++11-shared"] = generate_cpp11_shared;
    generators["dot"] = generate_dot;
    generators["C#"] = generate_cs;

    std::ifstream ifs(cmdopt.infile.c_str());
    if (!ifs) {
        std::cerr << "can't open input file '" << cmdopt.infile << "'" << std::endl;
        exit(1);
    }

    std::ofstream ofs(cmdopt.outfile.c_str());
    if (!ofs) {
        std::cerr << "can't open output file '" << cmdopt.outfile << "'" << std::endl;
        exit(1);
    }

    if (generators.find(cmdopt.language) == generators.end()) {
        std::cerr << "no such language supported: " << cmdopt.language << std::endl;
        exit(1);
    }

    try {
        // スキャナ
        typedef std::istreambuf_iterator<char> is_iterator;
        is_iterator b(ifs);
        is_iterator e;
        scanner<is_iterator> s(b, e);

        SemanticAction sa;
        Parser<Value, SemanticAction> parser(sa);

        Token token;
        for (;;) {
            Value v;
            token = s.get(v);
            if (parser.post(token, v)) { break; }
        }

        if (parser.error()) {
            std::cerr << "syntax error at line: " << s.lineno(s.addr()) << std::endl;
            exit(1);
        }

        Value v;
        if (parser.accept(v)) {
            Dependency dependency;
            make_dependency(dependency, v);

            typeset_type types;
            atomset_type atoms;
            boost::apply_visitor(IdentifierCollector(types, atoms), v.data);

            generators[cmdopt.language](argv[2], ofs, dependency, types, atoms, v);
#if 0

            std::cerr << "atoms: ";
            for (const auto& e: atoms) {
                std::cerr << e << ", ";
            }
            std::cerr << std::endl;

            std::cerr << "accpeted\n";
#endif

        }
    }
    catch(capella_error& e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    return 0;
}

