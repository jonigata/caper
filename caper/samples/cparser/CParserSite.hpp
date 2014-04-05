// Written by Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
// This file is public domain software.

#ifndef PARSERSITE_HPP_
#define PARSERSITE_HPP_

#include <iostream>     // std::cerr
#include <string>       // std::string
#include <map>      // std::map

#include "Location.hpp"     // cparser::Location
#include "Type.hpp"         // cparser::TypeCell
#include "CParserAST.hpp"   // cparser::Node

namespace cparser
{
    //
    // NameScope
    //
    struct NameScope
    {
        typedef std::map<std::string, TypeCell> map_type;

    public:
        NameScope() : m_next(NULL)
        {
        }

        NameScope(const NameScope& namescope) : m_next(NULL)
        {
            m_map1 = namescope.m_map1;
            m_map2 = namescope.m_map2;
        }

        void operator=(const NameScope& namescope)
        {
            m_map1 = namescope.m_map1;
            m_map2 = namescope.m_map2;
        }

    public:
        NameScope *m_next;
        map_type   m_map1;  // standard name mapping
        map_type   m_map2;  // tag name mapping
    };

    //
    // ParserSite
    //
    struct ParserSite
    {
        typedef NameScope                            name_scope_type;
        typedef NameScope::map_type                  map_type;
        typedef NameScope::map_type::iterator        map_iterator_type;
        typedef NameScope::map_type::const_iterator  map_const_iterator_type;

    public:
        ParserSite() : m_num_errors(0), m_num_warnings(0),
                       m_name_scope_stack(NULL)
        {
        }

        ~ParserSite()
        {
            name_scope_type *next, *scope = m_name_scope_stack;
            while (scope)
            {
                next = scope->m_next;
                delete scope;
                scope = next;
            }
        }

        bool compile(TransUnit& tu);

        void syntax_error()
        {
        }

        void stack_overflow()
        {
        }

        void downcast(int& x, const shared_ptr<Node>& y)
        {
        }

        void upcast(shared_ptr<Node>& x, const int& y)
        {
            x = shared_ptr<Node>(new Node);
        }

        template <typename T>
        void downcast(shared_ptr<T>& x, const shared_ptr<Node>& y)
        {
            x = dynamic_pointer_cast<T, Node>(y);
        }

        template <typename T>
        void upcast(shared_ptr<Node>& x, const shared_ptr<T>& y)
        {
            x = static_pointer_cast<Node, T>(y);
        }

        //
        // semantic actions
        //
        shared_ptr<TransUnit> DoTransUnit1(
            shared_ptr<TransUnit>& tu, shared_ptr<ExtDecl>& ed)
        {
            if (ed)
                tu.get()->push_back(ed->m_decl);
            return tu;
        }

        shared_ptr<TransUnit> DoTransUnit2(shared_ptr<ExtDecl>& ed)
        {
            TransUnit *tu = new TransUnit;
            if (ed)
                tu->push_back(ed->m_decl);
            return shared_ptr<TransUnit>(tu);
        }

        shared_ptr<ExtDecl> DoExtDecl1(shared_ptr<Decl>& decl)
        {
            ExtDecl *ed = new ExtDecl;
            ed->m_decl = decl;
            return shared_ptr<ExtDecl>(ed);
        }

        shared_ptr<ExtDecl> DoExtDecl2(shared_ptr<Decl>& decl)
        {
            ExtDecl *ed = new ExtDecl;
            ed->m_decl = decl;
            return shared_ptr<ExtDecl>(ed);
        }

        shared_ptr<ExtDecl> DoExtDecl3()
        {
            return shared_ptr<ExtDecl>();
        }

        shared_ptr<Decl> DoFuncDef1(shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d,
                                    shared_ptr<DeclList>& dl, shared_ptr<CompStmt>& cs)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::FUNCTION;
            decl->m_decl_specs = ds;

            DeclorList *declor_list = new DeclorList;
            declor_list->push_back(d);
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            decl->m_decl_list = dl;
            decl->m_comp_stmt = cs;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoFuncDef2(
            shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d, shared_ptr<CompStmt>& cs)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::FUNCTION;
            decl->m_decl_specs = ds;

            DeclorList *declor_list = new DeclorList;
            declor_list->push_back(d);
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            decl->m_comp_stmt = cs;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoFuncDef3(
            shared_ptr<Declor>& d, shared_ptr<DeclList>& dl, shared_ptr<CompStmt>& cs)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::FUNCTION;

            DeclorList *declor_list = new DeclorList;
            declor_list->push_back(d);
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            decl->m_decl_list = dl;
            decl->m_comp_stmt = cs;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoFuncDef4(shared_ptr<Declor>& d, shared_ptr<CompStmt>& cs)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::FUNCTION;

            DeclorList *declor_list = new DeclorList;
            declor_list->push_back(d);
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            decl->m_comp_stmt = cs;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<DeclList> DoDeclList1(
            shared_ptr<DeclList>& dl, shared_ptr<Decl>& d)
        {
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclList> DoDeclList2(shared_ptr<Decl>& d)
        {
            DeclList *dl = new DeclList;
            dl->push_back(d);
            return shared_ptr<DeclList>(dl);
        }

        shared_ptr<Decl> DoDecl1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<DeclorList>& dl)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::TYPEDEF;
            decl->m_decl_specs = ds;
            decl->m_declor_list = dl;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl2(
            shared_ptr<DeclSpecs>& ds, shared_ptr<DeclorList>& dl)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::DECLORLIST;
            decl->m_decl_specs = ds;
            decl->m_declor_list = dl;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl3(shared_ptr<DeclSpecs>& ds)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::SINGLE;
            decl->m_decl_specs = ds;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl4(shared_ptr<StaticAssertDecl>& sad)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::STATIC_ASSERT;
            decl->m_static_assert_decl = sad;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl5(shared_ptr<AsmSpec>& as)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::ASMSPEC;
            decl->m_asm_spec = as;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl6(shared_ptr<AsmBlock>& ab)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::ASMBLOCK;
            decl->m_asm_block = ab;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs1(
            shared_ptr<StorClsSpec>& scs, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::STORCLSSPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_stor_cls_spec = scs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs2(shared_ptr<StorClsSpec>& scs)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::STORCLSSPEC;
            decl_specs->m_stor_cls_spec = scs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs3(
            shared_ptr<FuncSpec>& fs, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::FUNCSPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_func_spec = fs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs4(shared_ptr<FuncSpec>& fs)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::STORCLSSPEC;
            decl_specs->m_func_spec = fs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs5(
            shared_ptr<TypeSpec>& ts, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs6(shared_ptr<TypeSpec>& ts)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs7(
            shared_ptr<TypeQual>& tq, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs8(shared_ptr<TypeQual>& tq)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs9(
            shared_ptr<AlignSpec>& as, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::ALIGNSPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_align_spec = as;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs10(shared_ptr<AlignSpec>& as)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::ALIGNSPEC;
            decl_specs->m_align_spec = as;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs1(
            shared_ptr<TypeSpec>& ts, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            decl_specs->m_decl_specs = ds;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs2(
            shared_ptr<TypeQual>& tq, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            decl_specs->m_decl_specs = ds;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs3(shared_ptr<TypeSpec>& ts)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs4(shared_ptr<TypeQual>& tq)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec1()
        {
            StorClsSpec *scs = new StorClsSpec;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec2()
        {
            StorClsSpec *scs = new StorClsSpec;
            scs->m_flag = TF_EXTERN;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec3()
        {
            StorClsSpec *scs = new StorClsSpec;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec4()
        {
            StorClsSpec *scs = new StorClsSpec;
            scs->m_flag = TF_STATIC;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec5()
        {
            StorClsSpec *scs = new StorClsSpec;
            scs->m_flag = TF_THREADLOCAL;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<TypeSpec> DoTypeSpec1()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_VOID;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec2()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_CHAR;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec3()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_SHORT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec4()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_INT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec5()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_INT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec6()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_LONGLONG;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec7()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_LONG;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec8()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_FLOAT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec9()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_DOUBLE;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec10()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_SIGNED;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec11()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNSIGNED;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec12()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_INT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec13()
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = 0;   // TODO: __w64
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec14(shared_ptr<TokenValue>& info)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ALIAS;
            ts->m_name = info->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec15(shared_ptr<TypeSpec>& struct_spec)
        {
            return struct_spec;
        }

        shared_ptr<TypeSpec> DoTypeSpec16(shared_ptr<TypeSpec>& union_spec)
        {
            return union_spec;
        }

        shared_ptr<TypeSpec> DoTypeSpec17(shared_ptr<TypeSpec>& enum_spec)
        {
            return enum_spec;
        }

        shared_ptr<TypeSpec> DoTypeSpec18(shared_ptr<AtomicTypeSpec>& ats)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ATOMIC;
            ts->m_atomic_type_spec = ats;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec19()
        {
            // TODO: complex
            assert(0);
            return shared_ptr<TypeSpec>();
        }

        shared_ptr<TypeSpec> DoTypeSpec20()
        {
            // TODO: imaginary
            assert(0);
            return shared_ptr<TypeSpec>();
        }
        
        shared_ptr<AtomicTypeSpec> DoAtomicTypeSpec1(shared_ptr<TypeName>& tn)
        {
            AtomicTypeSpec *ats = new AtomicTypeSpec;
            ats->m_type_name = tn;
            return shared_ptr<AtomicTypeSpec>(ats);
        }

        shared_ptr<TypeQual> DoTypeQual1()
        {
            TypeQual *tq = new TypeQual;
            tq->m_flag = TF_CONST;
            return shared_ptr<TypeQual>(tq);
        }

        shared_ptr<TypeQual> DoTypeQual2()
        {
            TypeQual *tq = new TypeQual;
            tq->m_flag = 0;    // restrict is ignored
            return shared_ptr<TypeQual>(tq);
        }

        shared_ptr<TypeQual> DoTypeQual3()
        {
            TypeQual *tq = new TypeQual;
            tq->m_flag = TF_VOLATILE;
            return shared_ptr<TypeQual>(tq);
        }

        shared_ptr<TypeSpec> DoStructSpec1(
            shared_ptr<TokenValue>& tag, shared_ptr<DeclList>& decl_list)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_STRUCT;
            ts->m_name = tag->m_text;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoStructSpec2(shared_ptr<DeclList>& decl_list)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_STRUCT;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoStructSpec3(shared_ptr<TokenValue>& tag)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_STRUCT;
            ts->m_name = tag->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoUnionSpec1(
            shared_ptr<TokenValue>& tag, shared_ptr<DeclList>& decl_list)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNION;
            ts->m_name = tag->m_text;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoUnionSpec2(shared_ptr<DeclList>& decl_list)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNION;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoUnionSpec3(shared_ptr<TokenValue>& tag)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNION;
            ts->m_name = tag->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<DeclList> DoStructDeclList1(
            shared_ptr<DeclList>& dl, shared_ptr<Decl>& d)
        {
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclList> DoStructDeclList2(shared_ptr<Decl>& d)
        {
            DeclList *dl = new DeclList;
            dl->push_back(d);
            return shared_ptr<DeclList>(dl);
        }

        shared_ptr<DeclorList> DoTypedefDeclorList1(
            shared_ptr<DeclorList>& dl, shared_ptr<Declor>& d)
        {
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclorList> DoTypedefDeclorList2(shared_ptr<Declor>& d)
        {
            DeclorList *dl = new DeclorList;
            dl->push_back(d);
            return shared_ptr<DeclorList>(dl);
        }

        shared_ptr<DeclorList> DoInitDeclorList1(
            shared_ptr<DeclorList>& dl, shared_ptr<Declor>& d)
        {
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclorList> DoInitDeclorList2(shared_ptr<Declor>& d)
        {
            DeclorList *dl = new DeclorList;
            dl->push_back(d);
            return shared_ptr<DeclorList>(dl);
        }

        shared_ptr<Declor> DoInitDeclor1(
            shared_ptr<Declor>& d, shared_ptr<Initer>& i)
        {
            d.get()->m_initer = i;
            return d;
        }

        shared_ptr<Declor> DoInitDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Decl> DoStructDecl1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<DeclorList>& dl)
        {
            Decl *d = new Decl;
            d->m_decl_type = Decl::DECLORLIST;
            d->m_decl_specs = ds;
            d->m_declor_list = dl;
            return shared_ptr<Decl>(d);
        }

        shared_ptr<Decl> DoStructDecl2(shared_ptr<DeclSpecs>& ds)
        {
            Decl *d = new Decl;
            d->m_decl_type = Decl::SINGLE;
            d->m_decl_specs = ds;
            return shared_ptr<Decl>(d);
        }

        shared_ptr<Decl> DoStructDecl3(shared_ptr<StaticAssertDecl>& sad)
        {
            Decl *d = new Decl;
            d->m_decl_type = Decl::STATIC_ASSERT;
            d->m_static_assert_decl = sad;
            return shared_ptr<Decl>(d);
        }

        shared_ptr<DeclSpecs> DoSpecQualList1(
            shared_ptr<TypeSpec>& ts, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoSpecQualList2(shared_ptr<TypeSpec>& ts)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoSpecQualList3(
            shared_ptr<TypeQual>& tq, shared_ptr<DeclSpecs>& ds)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoSpecQualList4(shared_ptr<TypeQual>& tq)
        {
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclorList> DoStructDeclorList1(
            shared_ptr<DeclorList>& dl, shared_ptr<Declor>& d)
        {
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclorList> DoStructDeclorList2(shared_ptr<Declor>& d)
        {
            DeclorList *dl = new DeclorList;
            dl->push_back(d);
            return shared_ptr<DeclorList>(dl);
        }

        shared_ptr<Declor> DoStructDeclor1(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            d.get()->m_declor_type = Declor::BITS;
            d.get()->m_const_expr = ce;
            return d;
        }

        shared_ptr<Declor> DoStructDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoStructDeclor3(shared_ptr<CondExpr>& ce)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::BITS;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<TypeSpec> DoEnumSpec1(
            shared_ptr<TokenValue>& tag, shared_ptr<EnumorList>& el)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_name = tag->m_text;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec2(
            shared_ptr<TokenValue>& tag, shared_ptr<EnumorList>& el)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_name = tag->m_text;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec3(shared_ptr<EnumorList>& el)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec4(shared_ptr<EnumorList>& el)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec5(shared_ptr<TokenValue>& tag)
        {
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_name = tag->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<EnumorList> DoEnumorList1(
            shared_ptr<EnumorList>& el, shared_ptr<Enumor>& e)
        {
            el.get()->push_back(e);
            return el;
        }

        shared_ptr<EnumorList> DoEnumorList2(shared_ptr<Enumor>& e)
        {
            EnumorList *el = new EnumorList;
            el->push_back(e);
            return shared_ptr<EnumorList>(el);
        }

        shared_ptr<Enumor> DoEnumor1(
            shared_ptr<TokenValue>& info, shared_ptr<CondExpr>& ce)
        {
            Enumor *e = new Enumor;
            e->m_name = info->m_text;
            e->m_const_expr = ce;
            return shared_ptr<Enumor>(e);
        }

        shared_ptr<Enumor> DoEnumor2(shared_ptr<TokenValue>& info)
        {
            Enumor *e = new Enumor;
            e->m_name = info->m_text;
            return shared_ptr<Enumor>(e);
        }

        shared_ptr<Declor> DoTypedefDeclor1(
            shared_ptr<Pointers>& ptrs, shared_ptr<Declor>& d)
        {
            d.get()->m_pointers = ptrs;
            return d;
        }

        shared_ptr<Declor> DoTypedefDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoDeclor1(
            shared_ptr<Pointers>& ptrs, shared_ptr<Declor>& d)
        {
            d.get()->m_pointers = ptrs;
            return d;
        }

        shared_ptr<Declor> DoDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoTypedefDirDeclor1(shared_ptr<TokenValue>& token)
        {
            Declor *d = new Declor;
            d->m_declor_type = Declor::TYPEDEF_TAG;
            d->m_name = token->m_text;
            return shared_ptr<Declor>(d);
        }

        shared_ptr<Declor> DoTypedefDirDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoTypedefDirDeclor3(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDirDeclor4(shared_ptr<Declor>& d)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDirDeclor5(
            shared_ptr<Declor>& d, shared_ptr<ParamList>& pl)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDirDeclor6(
            shared_ptr<Declor>& d, shared_ptr<IdentList>& il)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_ident_list = il;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDirDeclor7(shared_ptr<Declor>& d)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor1(shared_ptr<TokenValue>& token)
        {
            Declor *d = new Declor;
            d->m_declor_type = Declor::IDENTIFIER;
            d->m_name = token->m_text;
            return shared_ptr<Declor>(d);
        }

        shared_ptr<Declor> DoDirDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoDirDeclor3(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor4(shared_ptr<Declor>& d)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor5(
            shared_ptr<Declor>& d, shared_ptr<ParamList>& pl)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor6(
            shared_ptr<Declor>& d, shared_ptr<IdentList>& il)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_ident_list = il;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor7(shared_ptr<Declor>& d)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Pointers> DoPtr1(
            shared_ptr<AstCom>& ac, shared_ptr<Pointers>& p)
        {
            p.get()->push_back(ac);
            return p;
        }

        shared_ptr<Pointers> DoPtr2(shared_ptr<AstCom>& ac)
        {
            Pointers *p = new Pointers;
            p->push_back(ac);
            return shared_ptr<Pointers>(p);
        }

        shared_ptr<Pointers> DoPtr3(
            shared_ptr<AstCom>& ac, shared_ptr<Pointers>& p)
        {
            p.get()->push_back(ac);
            return p;
        }

        shared_ptr<Pointers> DoPtr4(shared_ptr<AstCom>& ac)
        {
            Pointers *p = new Pointers;
            p->push_back(ac);
            return shared_ptr<Pointers>(p);
        }

        shared_ptr<AstCom> DoAstCom1(shared_ptr<TokenValue>& asterisk)
        {
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom2(shared_ptr<TokenValue>& asterisk)
        {
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom3(shared_ptr<TokenValue>& asterisk)
        {
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom4(shared_ptr<TokenValue>& asterisk)
        {
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom5(shared_ptr<TokenValue>& asterisk)
        {
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom6(shared_ptr<TokenValue>& asterisk)
        {
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<TypeQualList> DoTypeQualList1(
            shared_ptr<TypeQualList>& tql, shared_ptr<TypeQual>& tq)
        {
            tql.get()->push_back(tq);
            return tql;
        }

        shared_ptr<TypeQualList> DoTypeQualList2(shared_ptr<TypeQual>& tq)
        {
            TypeQualList *tql = new TypeQualList;
            tql->push_back(tq);
            return shared_ptr<TypeQualList>(tql);
        }

        shared_ptr<ParamList> DoParamTypeList1(shared_ptr<ParamList>& pl)
        {
            pl.get()->m_ellipsis = true;
            return pl;
        }

        shared_ptr<ParamList> DoParamTypeList2(shared_ptr<ParamList>& pl)
        {
            return pl;
        }

        shared_ptr<ParamList> DoParamList1(
            shared_ptr<ParamList>& pl, shared_ptr<Decl>& d)
        {
            pl.get()->push_back(d);
            return pl;
        }

        shared_ptr<ParamList> DoParamList2(shared_ptr<Decl>& d)
        {
            ParamList *pl = new ParamList;
            pl->push_back(d);
            return shared_ptr<ParamList>(pl);
        }

        shared_ptr<Decl> DoParamDecl1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::PARAM;
            decl->m_decl_specs = ds;

            DeclorList *declor_list = new DeclorList;
            declor_list->push_back(d);
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoParamDecl2(
            shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::PARAM;
            decl->m_decl_specs = ds;

            DeclorList *declor_list = new DeclorList;
            declor_list->push_back(d);
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoParamDecl3(shared_ptr<DeclSpecs>& ds)
        {
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::PARAM;
            decl->m_decl_specs = ds;

            DeclorList *declor_list = new DeclorList;
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            return shared_ptr<Decl>(decl);
        }

        shared_ptr<IdentList> DoIdentList1(
            shared_ptr<IdentList>& il, shared_ptr<TokenValue>& id)
        {
            il.get()->push_back(id->m_text);
            return il;
        }

        shared_ptr<IdentList> DoIdentList2(shared_ptr<TokenValue>& id)
        {
            IdentList *il = new IdentList;
            il->push_back(id->m_text);
            return shared_ptr<IdentList>(il);
        }

        shared_ptr<Initer> DoIniter1(shared_ptr<AssignExpr>& ae)
        {
            Initer *i = new Initer;
            i->m_initer_type = Initer::SIMPLE;
            i->m_assign_expr = ae;
            return shared_ptr<Initer>(i);
        }

        shared_ptr<Initer> DoIniter2(shared_ptr<IniterList>& il)
        {
            Initer *i = new Initer;
            i->m_initer_type = Initer::COMPLEX;
            i->m_initer_list = il;
            return shared_ptr<Initer>(i);
        }

        shared_ptr<Initer> DoIniter3(shared_ptr<IniterList>& il)
        {
            Initer *i = new Initer;
            i->m_initer_type = Initer::COMPLEX;
            i->m_initer_list = il;
            return shared_ptr<Initer>(i);
        }

        shared_ptr<IniterList> DoIniterList1(
            shared_ptr<IniterList>& il, shared_ptr<Initer>& i)
        {
            il.get()->push_back(i);
            return il;
        }

        shared_ptr<IniterList> DoIniterList2(shared_ptr<Initer>& i)
        {
            IniterList *il = new IniterList;
            il->push_back(i);
            return shared_ptr<IniterList>(il);
        }

        shared_ptr<TypeName> DoTypeName1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d)
        {
            TypeName *tn = new TypeName;
            tn->m_decl_specs = ds;
            tn->m_declor = d;
            return shared_ptr<TypeName>(tn);
        }

        shared_ptr<TypeName> DoTypeName2(shared_ptr<DeclSpecs>& ds)
        {
            TypeName *tn = new TypeName;
            tn->m_decl_specs = ds;
            return shared_ptr<TypeName>(tn);
        }

        shared_ptr<Declor> DoAbsDeclor1(
            shared_ptr<Pointers>& ptrs, shared_ptr<Declor>& d)
        {
            d.get()->m_pointers = ptrs;
            return d;
        }

        shared_ptr<Declor> DoAbsDeclor2(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoAbsDeclor3(shared_ptr<Pointers>& ptrs)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::POINTERS;
            declor->m_pointers = ptrs;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor1(shared_ptr<Declor>& d)
        {
            return d;
        }

        shared_ptr<Declor> DoDirAbsDeclor2()
        {
            Declor *d = new Declor;
            d->m_declor_type = Declor::ARRAY;
            return shared_ptr<Declor>(d);
        }

        shared_ptr<Declor> DoDirAbsDeclor3(shared_ptr<Declor>& d)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor4(shared_ptr<CondExpr>& ce)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor5(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor6()
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor7(shared_ptr<Declor>& d)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor8(shared_ptr<ParamList>& pl)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor9(
            shared_ptr<Declor>& d, shared_ptr<ParamList>& pl)
        {
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<StmtList> DoStmtList1(
            shared_ptr<StmtList>& sl, shared_ptr<Stmt>& s)
        {
            sl.get()->push_back(s);
            return sl;
        }

        shared_ptr<StmtList> DoStmtList2(shared_ptr<Stmt>& s)
        {
            StmtList *sl = new StmtList;
            sl->push_back(s);
            return shared_ptr<StmtList>(sl);
        }

        shared_ptr<Stmt> DoStmt1(shared_ptr<LabeledStmt>& ls)
        {
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::LABELED;
            s->m_labeled_stmt = ls;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt2(shared_ptr<ExprStmt>& es)
        {
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::EXPR;
            s->m_expr_stmt = es;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt3(shared_ptr<CompStmt>& cs)
        {
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::COMP;
            s->m_comp_stmt = cs;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt4(shared_ptr<SelStmt>& ss)
        {
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::SEL;
            s->m_sel_stmt = ss;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt5(shared_ptr<IterStmt>& is)
        {
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::ITER;
            s->m_iter_stmt = is;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt6(shared_ptr<JumpStmt>& js)
        {
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::JUMP;
            s->m_jump_stmt = js;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<LabeledStmt> DoLabeledStmt1(
            shared_ptr<TokenValue>& id, shared_ptr<Stmt>& s)
        {
            LabeledStmt *ls = new LabeledStmt;
            ls->m_labeled_type = LabeledStmt::LABEL;
            ls->m_label = id->m_text.c_str();
            ls->m_stmt = s;
            return shared_ptr<LabeledStmt>(ls);
        }

        shared_ptr<LabeledStmt> DoLabeledStmt2(
            shared_ptr<CondExpr>& ce, shared_ptr<Stmt>& s)
        {
            LabeledStmt *ls = new LabeledStmt;
            ls->m_labeled_type = LabeledStmt::CASE;
            ls->m_const_expr = ce;
            ls->m_stmt = s;
            return shared_ptr<LabeledStmt>(ls);
        }

        shared_ptr<LabeledStmt> DoLabeledStmt3(shared_ptr<Stmt>& s)
        {
            LabeledStmt *ls = new LabeledStmt;
            ls->m_labeled_type = LabeledStmt::DEFAULT;
            ls->m_stmt = s;
            return shared_ptr<LabeledStmt>(ls);
        }

        shared_ptr<ExprStmt> DoExprStmt1()
        {
            return shared_ptr<ExprStmt>(new ExprStmt);
        }

        shared_ptr<ExprStmt> DoExprStmt2(shared_ptr<Expr>& e)
        {
            ExprStmt *es = new ExprStmt;
            es->m_expr = e;
            return shared_ptr<ExprStmt>(es);
        }

        shared_ptr<CompStmt> DoCompStmt1()
        {
            return shared_ptr<CompStmt>(new CompStmt);
        }

        shared_ptr<CompStmt> DoCompStmt2(shared_ptr<DeclList>& dl)
        {
            CompStmt *cs = new CompStmt;
            cs->m_decl_list = dl;
            return shared_ptr<CompStmt>(cs);
        }

        shared_ptr<CompStmt> DoCompStmt3(shared_ptr<StmtList>& sl)
        {
            CompStmt *cs = new CompStmt;
            cs->m_stmt_list = sl;
            return shared_ptr<CompStmt>(cs);
        }

        shared_ptr<CompStmt> DoCompStmt4(
            shared_ptr<DeclList>& dl, shared_ptr<StmtList>& sl)
        {
            CompStmt *cs = new CompStmt;
            cs->m_decl_list = dl;
            cs->m_stmt_list = sl;
            return shared_ptr<CompStmt>(cs);
        }

        shared_ptr<SelStmt> DoSelStmt1(
            shared_ptr<Expr>& e, shared_ptr<Stmt>& then_s, shared_ptr<Stmt>& else_s)
        {
            SelStmt *ss = new SelStmt;
            ss->m_sel_type = SelStmt::IF_THEN_ELSE;
            ss->m_expr = e;
            ss->m_then = then_s;
            ss->m_else = else_s;
            return shared_ptr<SelStmt>(ss);
        }

        shared_ptr<SelStmt> DoSelStmt2(
            shared_ptr<Expr>& e, shared_ptr<Stmt>& then_s)
        {
            SelStmt *ss = new SelStmt;
            ss->m_sel_type = SelStmt::IF_THEN;
            ss->m_expr = e;
            ss->m_then = then_s;
            return shared_ptr<SelStmt>(ss);
        }

        shared_ptr<SelStmt> DoSelStmt3(
            shared_ptr<Expr>& e, shared_ptr<Stmt>& then_s)
        {
            SelStmt *ss = new SelStmt;
            ss->m_sel_type = SelStmt::SWITCH;
            ss->m_expr = e;
            ss->m_then = then_s;
            return shared_ptr<SelStmt>(ss);
        }

        shared_ptr<IterStmt> DoIterStmt1(shared_ptr<Expr>& e, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::WHILE;
            is->m_expr1 = e;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt2(shared_ptr<Stmt>& s, shared_ptr<Expr>& e)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::DO_WHILE;
            is->m_expr1 = e;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt3(shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt4(shared_ptr<Expr>& e3, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr3 = e3;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt5(shared_ptr<Expr>& e2, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr2 = e2;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt6(
            shared_ptr<Expr>& e2, shared_ptr<Expr>& e3, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e2;
            is->m_expr2 = e3;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt7(shared_ptr<Expr>& e1, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e1;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt8(
            shared_ptr<Expr>& e1, shared_ptr<Expr>& e3, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e1;
            is->m_expr3 = e3;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt9(
            shared_ptr<Expr>& e1, shared_ptr<Expr>& e2, shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e1;
            is->m_expr2 = e2;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt10(
            shared_ptr<Expr>& e1, shared_ptr<Expr>& e2, shared_ptr<Expr>& e3,
            shared_ptr<Stmt>& s)
        {
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e1;
            is->m_expr2 = e2;
            is->m_expr2 = e3;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<JumpStmt> DoJumpStmt1(shared_ptr<TokenValue>& id)
        {
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::GOTO;
            js->m_label = id->m_text;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt2()
        {
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::CONTINUE;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt3()
        {
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::BREAK;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt4()
        {
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::RETURN_VOID;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt5(shared_ptr<Expr>& e)
        {
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::RETURN_EXPR;
            js->m_expr = e;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<Expr> DoExpr1(shared_ptr<AssignExpr>& ae)
        {
            Expr *e = new Expr;
            e->push_back(ae);
            return shared_ptr<Expr>(e);
        }

        shared_ptr<Expr> DoExpr2(
            shared_ptr<Expr>& e, shared_ptr<AssignExpr>& ae)
        {
            e.get()->push_back(ae);
            return e;
        }

        shared_ptr<AssignExpr> DoAssignExpr1(shared_ptr<CondExpr>& ce)
        {
            AssignExpr *ae = new AssignExpr;
            ae->m_assign_type = AssignExpr::COND;
            ae->m_cond_expr = ce;
            return shared_ptr<AssignExpr>(ae);
        }

        shared_ptr<AssignExpr> DoAssignExpr2(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::SINGLE;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr3(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::MUL;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr4(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::DIV;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr5(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::MOD;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr6(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::ADD;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr7(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::SUB;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr8(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::L_SHIFT;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr9(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::R_SHIFT;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr10(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::AND;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr11(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::XOR;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr12(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::OR;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<CondExpr> DoCondExpr1(
            shared_ptr<LogOrExpr>& loe)
        {
            CondExpr *ce = new CondExpr;
            ce->m_cond_type = CondExpr::SINGLE;
            ce->m_log_or_expr = loe;
            return shared_ptr<CondExpr>(ce);
        }

        shared_ptr<CondExpr> DoCondExpr2(
            shared_ptr<LogOrExpr>& loe,
            shared_ptr<Expr>& e,
            shared_ptr<CondExpr>& ce)
        {
            CondExpr *newce = new CondExpr;
            newce->m_cond_type = CondExpr::QUESTION;
            newce->m_log_or_expr = loe;
            newce->m_expr = e;
            newce->m_cond_expr = ce;
            return shared_ptr<CondExpr>(newce);
        }

        shared_ptr<CondExpr> DoConstExpr1(shared_ptr<CondExpr>& ce)
        {
            return ce;
        }

        shared_ptr<LogOrExpr> DoLogOrExpr1(shared_ptr<LogAndExpr>& lae)
        {
            LogOrExpr *loe = new LogOrExpr;
            loe->push_back(lae);
            return shared_ptr<LogOrExpr>(loe);
        }

        shared_ptr<LogOrExpr> DoLogOrExpr2(
            shared_ptr<LogOrExpr>& loe, shared_ptr<LogAndExpr>& lae)
        {
            loe.get()->push_back(lae);
            return loe;
        }

        shared_ptr<LogAndExpr> DoLogAnd1(shared_ptr<InclOrExpr>& ioe)
        {
            LogAndExpr *lae = new LogAndExpr;
            lae->push_back(ioe);
            return shared_ptr<LogAndExpr>(lae);
        }

        shared_ptr<LogAndExpr> DoLogAnd2(
            shared_ptr<LogAndExpr>& lae, shared_ptr<InclOrExpr>& ioe)
        {
            lae.get()->push_back(ioe);
            return lae;
        }

        shared_ptr<InclOrExpr> DoInclOrExpr1(shared_ptr<ExclOrExpr>& eoe)
        {
            InclOrExpr *ioe = new InclOrExpr;
            ioe->push_back(eoe);
            return shared_ptr<InclOrExpr>(ioe);
        }

        shared_ptr<InclOrExpr> DoInclOrExpr2(
            shared_ptr<InclOrExpr>& ioe,
            shared_ptr<ExclOrExpr>& eoe)
        {
            ioe.get()->push_back(eoe);
            return ioe;
        }

        shared_ptr<ExclOrExpr> DoExclOr1(shared_ptr<AndExpr>& ae)
        {
            ExclOrExpr *eoe = new ExclOrExpr;
            eoe->push_back(ae);
            return shared_ptr<ExclOrExpr>(eoe);
        }

        shared_ptr<ExclOrExpr> DoExclOr2(
            shared_ptr<ExclOrExpr>& eoe, shared_ptr<AndExpr>& ae)
        {
            eoe.get()->push_back(ae);
            return eoe;
        }

        shared_ptr<AndExpr> DoAndExpr1(shared_ptr<EqualExpr>& ee)
        {
            AndExpr *ae = new AndExpr;
            ae->push_back(ee);
            return shared_ptr<AndExpr>(ae);
        }

        shared_ptr<AndExpr> DoAndExpr2(
            shared_ptr<AndExpr>& ae,
            shared_ptr<EqualExpr>& ee)
        {
            ae.get()->push_back(ee);
            return ae;
        }

        shared_ptr<EqualExpr> DoEqualExpr1(shared_ptr<RelExpr>& re)
        {
            EqualExpr *ee = new EqualExpr;
            ee->m_equal_type = EqualExpr::SINGLE;
            ee->m_rel_expr = re;
            return shared_ptr<EqualExpr>(ee);
        }

        shared_ptr<EqualExpr> DoEqualExpr2(
            shared_ptr<EqualExpr>& ee,
            shared_ptr<RelExpr>& re)
        {
            EqualExpr *newee = new EqualExpr;
            newee->m_equal_type = EqualExpr::EQUAL;
            newee->m_equal_expr = ee;
            newee->m_rel_expr = re;
            return shared_ptr<EqualExpr>(ee);
        }

        shared_ptr<EqualExpr> DoEqualExpr3(
            shared_ptr<EqualExpr>& ee,
            shared_ptr<RelExpr>& re)
        {
            EqualExpr *newee = new EqualExpr;
            newee->m_equal_type = EqualExpr::NE;
            newee->m_equal_expr = ee;
            newee->m_rel_expr = re;
            return shared_ptr<EqualExpr>(ee);
        }

        shared_ptr<RelExpr> DoRelExpr1(shared_ptr<ShiftExpr>& se)
        {
            RelExpr *re = new RelExpr;
            re->m_rel_type = RelExpr::SINGLE;
            re->m_shift_expr = se;
            return shared_ptr<RelExpr>(re);
        }

        shared_ptr<RelExpr> DoRelExpr2(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::LT;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<RelExpr> DoRelExpr3(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::GT;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<RelExpr> DoRelExpr4(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::LE;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<RelExpr> DoRelExpr5(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::GE;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<ShiftExpr> DoShiftExpr1(shared_ptr<AddExpr>& ae)
        {
            ShiftExpr *se = new ShiftExpr;
            se->m_shift_type = ShiftExpr::SINGLE;
            se->m_add_expr = ae;
            return shared_ptr<ShiftExpr>(se);
        }

        shared_ptr<ShiftExpr> DoShiftExpr2(
            shared_ptr<ShiftExpr>& se, shared_ptr<AddExpr>& ae)
        {
            ShiftExpr *newse = new ShiftExpr;
            newse->m_shift_type = ShiftExpr::L_SHIFT;
            newse->m_shift_expr = se;
            newse->m_add_expr = ae;
            return shared_ptr<ShiftExpr>(newse);
        }

        shared_ptr<ShiftExpr> DoShiftExpr3(
            shared_ptr<ShiftExpr>& se, shared_ptr<AddExpr>& ae)
        {
            ShiftExpr *newse = new ShiftExpr;
            newse->m_shift_type = ShiftExpr::R_SHIFT;
            newse->m_shift_expr = se;
            newse->m_add_expr = ae;
            return shared_ptr<ShiftExpr>(newse);
        }

        shared_ptr<AddExpr> DoAddExpr1(shared_ptr<MulExpr>& me)
        {
            AddExpr *ae = new AddExpr;
            ae->m_add_type = AddExpr::SINGLE;
            ae->m_mul_expr = me;
            return shared_ptr<AddExpr>(ae);
        }

        shared_ptr<AddExpr> DoAddExpr2(
            shared_ptr<AddExpr>& ae, shared_ptr<MulExpr>& me)
        {
            AddExpr *newae = new AddExpr;
            newae->m_add_type = AddExpr::PLUS;
            newae->m_add_expr = ae;
            newae->m_mul_expr = me;
            return shared_ptr<AddExpr>(newae);
        }

        shared_ptr<AddExpr> DoAddExpr3(
            shared_ptr<AddExpr>& ae, shared_ptr<MulExpr>& me)
        {
            AddExpr *newae = new AddExpr;
            newae->m_add_type = AddExpr::MINUS;
            newae->m_add_expr = ae;
            newae->m_mul_expr = me;
            return shared_ptr<AddExpr>(newae);
        }

        shared_ptr<MulExpr> DoMulExpr1(shared_ptr<CastExpr>& ce)
        {
            MulExpr *me = new MulExpr;
            me->m_mul_type = MulExpr::SINGLE;
            me->m_cast_expr = ce;
            return shared_ptr<MulExpr>(me);
        }

        shared_ptr<MulExpr> DoMulExpr2(
            shared_ptr<MulExpr>& me, shared_ptr<CastExpr>& ce)
        {
            MulExpr *newme = new MulExpr;
            newme->m_mul_type = MulExpr::ASTERISK;
            newme->m_mul_expr = me;
            newme->m_cast_expr = ce;
            return shared_ptr<MulExpr>(newme);
        }

        shared_ptr<MulExpr> DoMulExpr3(
            shared_ptr<MulExpr>& me, shared_ptr<CastExpr>& ce)
        {
            MulExpr *newme = new MulExpr;
            newme->m_mul_type = MulExpr::SLASH;
            newme->m_mul_expr = me;
            newme->m_cast_expr = ce;
            return shared_ptr<MulExpr>(newme);
        }

        shared_ptr<MulExpr> DoMulExpr4(
            shared_ptr<MulExpr>& me, shared_ptr<CastExpr>& ce)
        {
            MulExpr *newme = new MulExpr;
            newme->m_mul_type = MulExpr::PERCENT;
            newme->m_mul_expr = me;
            newme->m_cast_expr = ce;
            return shared_ptr<MulExpr>(newme);
        }

        shared_ptr<CastExpr> DoCastExpr1(shared_ptr<UnaryExpr>& ue)
        {
            CastExpr *ce = new CastExpr;
            ce->m_cast_type = CastExpr::UNARY;
            ce->m_unary_expr = ue;
            return shared_ptr<CastExpr>(ce);
        }

        shared_ptr<CastExpr> DoCastExpr2(
            shared_ptr<TypeName>& tn, shared_ptr<IniterList>& il)
        {
            CastExpr *ce = new CastExpr;
            ce->m_cast_type = CastExpr::INITERLIST;
            ce->m_type_name = tn;
            ce->m_initer_list = il;
            return shared_ptr<CastExpr>(ce);
        }

        shared_ptr<CastExpr> DoCastExpr3(
            shared_ptr<TypeName>& tn, shared_ptr<IniterList>& il)
        {
            CastExpr *ce = new CastExpr;
            ce->m_cast_type = CastExpr::INITERLIST;
            ce->m_type_name = tn;
            ce->m_initer_list = il;
            return shared_ptr<CastExpr>(ce);
        }

        shared_ptr<CastExpr> DoCastExpr4(
            shared_ptr<TypeName>& tn, shared_ptr<CastExpr>& ce)
        {
            CastExpr *newce = new CastExpr;
            newce->m_cast_type = CastExpr::CAST;
            newce->m_type_name = tn;
            newce->m_cast_expr = ce;
            return shared_ptr<CastExpr>(newce);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr1(shared_ptr<PostfixExpr>& pe)
        {
            UnaryExpr *ue = new UnaryExpr;
            ue->m_unary_type = UnaryExpr::SINGLE;
            ue->m_postfix_expr = pe;
            return shared_ptr<UnaryExpr>(ue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr2(shared_ptr<UnaryExpr>& ue)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::INC;
            newue->m_unary_expr = ue;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr3(shared_ptr<UnaryExpr>& ue)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::DEC;
            newue->m_unary_expr = ue;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr4(shared_ptr<CastExpr>& ce)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::AND;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr5(shared_ptr<CastExpr>& ce)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::ASTERISK;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr6(shared_ptr<CastExpr>& ce)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::PLUS;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr7(shared_ptr<CastExpr>& ce)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::MINUS;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr8(shared_ptr<CastExpr>& ce)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::BITWISE_NOT;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr9(shared_ptr<CastExpr>& ce)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::NOT;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr10(shared_ptr<UnaryExpr>& ue)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::SIZEOF1;
            newue->m_unary_expr = ue;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr11(shared_ptr<TypeName>& tn)
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::SIZEOF2;
            newue->m_type_name = tn;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr12()
        {
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::ALIGNOF;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr1(shared_ptr<PrimExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::SINGLE;
            newpe->m_prim_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr2(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::ARRAYITEM;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr3(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::FUNCCALL1;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr4(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::FUNCCALL2;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr5(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::DOT;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr6(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::ARROW;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr7(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::INC;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr8(shared_ptr<PostfixExpr>& pe)
        {
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::DEC;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr1(shared_ptr<TokenValue>& token)
        {
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::IDENTIFIER;
            newpe->m_text = token->m_text;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr2(shared_ptr<TokenValue>& token)
        {
            PrimExpr *newpe = new PrimExpr;
            newpe->m_flags = token->m_flags;
            if (token->m_flags & (TF_FLOAT | TF_DOUBLE))
                newpe->m_prim_type = PrimExpr::F_CONSTANT;
            else
                newpe->m_prim_type = PrimExpr::I_CONSTANT;
            newpe->m_text = token->m_text;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr3(shared_ptr<TokenValue>& token)
        {
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::STRING;
            newpe->m_text = token->m_text;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr4(shared_ptr<Expr>& e)
        {
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::PAREN;
            newpe->m_expr = e;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr5()
        {
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::SELECTION;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<GeneSel> DoGeneSel1(
            shared_ptr<AssignExpr>& ae, shared_ptr<GeneAssocList>& gal)
        {
            GeneSel *gs = new GeneSel;
            gs->m_assign_expr = ae;
            gs->m_gene_assoc_list = gal;
			return shared_ptr<GeneSel>(gs);
        }

        shared_ptr<GeneAssocList> DoGeneAssocList1(
            shared_ptr<GeneAssocList>& gal, shared_ptr<GeneAssoc>& ga)
        {
            gal.get()->push_back(ga);
            return gal;
        }

        shared_ptr<GeneAssocList> DoGeneAssocList2(shared_ptr<GeneAssoc>& ga)
        {
            GeneAssocList *gal = new GeneAssocList;
            gal->push_back(ga);
            return shared_ptr<GeneAssocList>(gal);
        }

        shared_ptr<GeneAssoc> DoGeneAssoc1(
            shared_ptr<TypeName>& tn, shared_ptr<AssignExpr>& ae)
        {
            GeneAssoc *ga = new GeneAssoc;
            ga->m_gene_assoc_type = GeneAssoc::NONDEFAULT;
            ga->m_type_name = tn;
            ga->m_assign_expr = ae;
            return shared_ptr<GeneAssoc>(ga);
        }

        shared_ptr<GeneAssoc> DoGeneAssoc2(shared_ptr<AssignExpr>& ae)
        {
            GeneAssoc *ga = new GeneAssoc;
            ga->m_gene_assoc_type = GeneAssoc::DEFAULT;
            ga->m_assign_expr = ae;
            return shared_ptr<GeneAssoc>(ga);
        }

        shared_ptr<ArgExprList> DoArgExprList1(
            shared_ptr<ArgExprList>& ael, shared_ptr<AssignExpr>& ae)
        {
            ael.get()->push_back(ae);
            return ael;
        }

        shared_ptr<ArgExprList> DoArgExprList2(shared_ptr<AssignExpr>& ae)
        {
            ArgExprList *ael = new ArgExprList;
            ael->push_back(ae);
            return shared_ptr<ArgExprList>(ael);
        }

        shared_ptr<FuncSpec> DoFuncSpec1()
        {
            FuncSpec *fs = new FuncSpec;
            fs->m_flag = TF_INLINE;
            return shared_ptr<FuncSpec>(fs);
        }

        shared_ptr<FuncSpec> DoFuncSpec2()
        {
            FuncSpec *fs = new FuncSpec;
            fs->m_flag = TF_FORCEINLINE;
            return shared_ptr<FuncSpec>(fs);
        }

        shared_ptr<FuncSpec> DoFuncSpec3()
        {
            FuncSpec *fs = new FuncSpec;
            fs->m_flag = TF_NORETURN;
            return shared_ptr<FuncSpec>(fs);
        }

        shared_ptr<AlignSpec> DoAlignSpec1(shared_ptr<TypeName>& tn)
        {
            AlignSpec *as = new AlignSpec;
            as->m_align_spec_type = AlignSpec::TYPENAME;
            as->m_type_name = tn;
            return shared_ptr<AlignSpec>(as);
        }

        shared_ptr<AlignSpec> DoAlignSpec2(shared_ptr<CondExpr>& ce)
        {
            AlignSpec *as = new AlignSpec;
            as->m_align_spec_type = AlignSpec::CONSTEXPR;
            as->m_const_expr = ce;
            return shared_ptr<AlignSpec>(as);
        }

        shared_ptr<StaticAssertDecl> DoStaticAssertDecl1(
            shared_ptr<CondExpr>& ce, shared_ptr<TokenValue>& str)
        {
            StaticAssertDecl *sad = new StaticAssertDecl;
            sad->m_const_expr = ce;
            sad->m_str = str->m_text;
            return shared_ptr<StaticAssertDecl>(sad);
        }

        shared_ptr<AsmSpec> DoAsmSpec1(
            shared_ptr<TypeQualList>& tql, shared_ptr<AsmOperands>& aos)
        {
            AsmSpec *as = new AsmSpec;
            as->m_type_qual_list = tql;
            as->m_asm_operands = aos;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmSpec> DoAsmSpec2(shared_ptr<TypeQualList>& tql)
        {
            AsmSpec *as = new AsmSpec;
            as->m_type_qual_list = tql;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmSpec> DoAsmSpec3(shared_ptr<AsmOperands>& aos)
        {
            AsmSpec *as = new AsmSpec;
            as->m_asm_operands = aos;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmSpec> DoAsmSpec4()
        {
            AsmSpec *as = new AsmSpec;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmBlock> DoAsmBlock1(shared_ptr<AsmOperands>& aos)
        {
            AsmBlock *ab = new AsmBlock;
            ab->m_asm_operands = aos;
            return shared_ptr<AsmBlock>(ab);
        }

        shared_ptr<AsmOperands> DoAsmOperands1(
            shared_ptr<AsmOperands>& aos, shared_ptr<AsmOperand>& ao)
        {
            aos.get()->push_back(ao);
            return aos;
        }

        shared_ptr<AsmOperands> DoAsmOperands2(shared_ptr<AsmOperand>& ao)
        {
            AsmOperands *aos = new AsmOperands;
            aos->push_back(ao);
            return shared_ptr<AsmOperands>(aos);
        }

        shared_ptr<AsmOperand> DoAsmOperand1(shared_ptr<TokenValue>& value)
        {
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::STRING;
            ao->m_text = value->m_text;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand2(shared_ptr<TokenValue>& value)
        {
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::IDENTIFIER;
            ao->m_text = value->m_text;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand3()
        {
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::COMMA;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand4()
        {
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::COLON;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand5(shared_ptr<Expr>& e)
        {
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::PAREN;
            ao->m_expr = e;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand6(shared_ptr<Expr>& e)
        {
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::BRACKET;
            ao->m_expr = e;
            return shared_ptr<AsmOperand>(ao);
        }

    public:
              Location& location()       { return m_loc; }
        const Location& location() const { return m_loc; }

        void namescope_push(NameScope *scope)
        {
            assert(scope);
            scope->m_next = m_name_scope_stack;
            m_name_scope_stack = scope;
        }

        NameScope *namescope_pop()
        {
            NameScope *scope = m_name_scope_stack;
            if (scope)
                m_name_scope_stack = scope->m_next;
            return scope;
        }

        TypeCell *find_name(const char *name)
        {
            NameScope *scope = m_name_scope_stack;
            while (scope)
            {
                map_iterator_type it = scope->m_map1.find(name);
                if (it != scope->m_map1.end())
                {
                    return &(it->second);
                }
                scope = scope->m_next;
            }
            return NULL;
        }

        const TypeCell *find_name(const char *name) const
        {
            const NameScope *scope = m_name_scope_stack;
            while (scope)
            {
                map_const_iterator_type it = scope->m_map1.find(name);
                if (it != scope->m_map1.end())
                {
                    return &(it->second);
                }
                scope = scope->m_next;
            }
            return NULL;
        }

        TypeCell *find_tag_name(const char *name)
        {
            NameScope *scope = m_name_scope_stack;
            while (scope)
            {
                map_iterator_type it = scope->m_map2.find(name);
                if (it != scope->m_map2.end())
                {
                    return &(it->second);
                }
                scope = scope->m_next;
            }
            return NULL;
        }

        const TypeCell *find_tag_name(const char *name) const
        {
            const NameScope *scope = m_name_scope_stack;
            while (scope)
            {
                map_const_iterator_type it = scope->m_map2.find(name);
                if (it != scope->m_map2.end())
                {
                    return &(it->second);
                }
                scope = scope->m_next;
            }
            return NULL;
        }

        //
        // errors and warnings
        //
        void message(const std::string& str)
        {
            std::cerr << location() << ": " << str << std::endl;
        }

        void unsupported_escape_sequence()
        {
            message("WARNING: unsupported escape sequence");
            add_warning();
        }

        void unexpected_character(char c)
        {
            std::string str("ERROR: unexpected character '");
            str += c;
            str += '\'';
            message(str);
            add_error();
        }

        void unexpected_eof()
        {
            message("ERROR: unexpected end of file");
            add_error();
        }

        void not_supported_yet(const std::string& str)
        {
            message(std::string("ERROR: ") + str + " is not supported yet");
            add_error();
        }

        int get_errors() const   { return m_num_errors; }
        int get_warnings() const { return m_num_warnings; }
        void add_error()         { m_num_errors++; }
        void add_warning()       { m_num_warnings++; }

        void clear_errors()
        {
            m_num_errors = m_num_warnings = 0;
        }

    protected:
        Location                        m_loc;
        int                             m_num_errors;
        int                             m_num_warnings;
        name_scope_type *               m_name_scope_stack;
    };
} // namespace cparser

inline bool cparser::ParserSite::compile(TransUnit& tu)
{
    // do nothing
    return 0;
}

#endif  // ndef PARSERSITE_HPP_
