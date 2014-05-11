#ifndef PARSERSITE_H_
#define PARSERSITE_H_

//#define DEEPDEBUG

namespace cparser
{
    //
    // ParserSite
    //
    struct ParserSite
    {
    public:
        ParserSite() : m_num_errors(0), m_num_warnings(0)
        {
        }

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
            #ifdef DEEPDEBUG
                std::printf("DoTransUnit1\n");
            #endif
            if (ed)
                tu.get()->push_back(ed->m_decl);
            return tu;
        }

        shared_ptr<TransUnit> DoTransUnit2(shared_ptr<ExtDecl>& ed)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTransUnit2\n");
            #endif
            TransUnit *tu = new TransUnit;
            if (ed)
                tu->push_back(ed->m_decl);
            return shared_ptr<TransUnit>(tu);
        }

        shared_ptr<ExtDecl> DoExtDecl1(shared_ptr<Decl>& decl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExtDecl1\n");
            #endif
            ExtDecl *ed = new ExtDecl;
            ed->m_decl = decl;
            return shared_ptr<ExtDecl>(ed);
        }

        shared_ptr<ExtDecl> DoExtDecl2(shared_ptr<Decl>& decl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExtDecl2\n");
            #endif
            ExtDecl *ed = new ExtDecl;
            ed->m_decl = decl;
            return shared_ptr<ExtDecl>(ed);
        }

        shared_ptr<ExtDecl> DoExtDecl3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoExtDecl3\n");
            #endif
            return shared_ptr<ExtDecl>();
        }

        shared_ptr<Decl> DoFuncDef1(shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d,
                                    shared_ptr<DeclList>& dl, shared_ptr<CompStmt>& cs)
        {
            #ifdef DEEPDEBUG
                std::printf("DoFuncDef1\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoFuncDef2\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoFuncDef3\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoFuncDef4\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoDeclList1\n");
            #endif
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclList> DoDeclList2(shared_ptr<Decl>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclList2\n");
            #endif
            DeclList *dl = new DeclList;
            dl->push_back(d);
            return shared_ptr<DeclList>(dl);
        }

        shared_ptr<Decl> DoDecl1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<DeclorList>& dl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDecl1\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::TYPEDEF;
            decl->m_decl_specs = ds;
            decl->m_declor_list = dl;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl2(
            shared_ptr<DeclSpecs>& ds, shared_ptr<DeclorList>& dl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDecl2\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::DECLORLIST;
            decl->m_decl_specs = ds;
            decl->m_declor_list = dl;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl3(shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDecl3\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::SINGLE;
            decl->m_decl_specs = ds;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl4(shared_ptr<StaticAssertDecl>& sad)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDecl4\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::STATIC_ASSERT;
            decl->m_static_assert_decl = sad;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl5(shared_ptr<AsmSpec>& as)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDecl5\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::ASMSPEC;
            decl->m_asm_spec = as;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<Decl> DoDecl6(shared_ptr<AsmBlock>& ab)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDecl6\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::ASMBLOCK;
            decl->m_asm_block = ab;
            return shared_ptr<Decl>(decl);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs1(
            shared_ptr<StorClsSpec>& scs, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs1\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::STORCLSSPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_stor_cls_spec = scs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs2(shared_ptr<StorClsSpec>& scs)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs2\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::STORCLSSPEC;
            decl_specs->m_stor_cls_spec = scs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs3(
            shared_ptr<FuncSpec>& fs, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs3\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::FUNCSPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_func_spec = fs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs4(shared_ptr<FuncSpec>& fs)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs4\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::FUNCSPEC;
            decl_specs->m_func_spec = fs;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs5(
            shared_ptr<TypeSpec>& ts, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs5\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs6(shared_ptr<TypeSpec>& ts)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs6\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs7(
            shared_ptr<TypeQual>& tq, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs7\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs8(shared_ptr<TypeQual>& tq)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs8\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs9(
            shared_ptr<AlignSpec>& as, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs9\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::ALIGNSPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_align_spec = as;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoDeclSpecs10(shared_ptr<AlignSpec>& as)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclSpecs10\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::ALIGNSPEC;
            decl_specs->m_align_spec = as;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs1(
            shared_ptr<TypeSpec>& ts, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclSpecs1\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            decl_specs->m_decl_specs = ds;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs2(
            shared_ptr<TypeQual>& tq, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclSpecs2\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            decl_specs->m_decl_specs = ds;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs3(shared_ptr<TypeSpec>& ts)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclSpecs3\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoTypedefDeclSpecs4(shared_ptr<TypeQual>& tq)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclSpecs4\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec1()
        {
            #ifdef DEEPDEBUG
                std::printf("DoStorClsSpec1\n");
            #endif
            StorClsSpec *scs = new StorClsSpec;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec2()
        {
            #ifdef DEEPDEBUG
                std::printf("DoStorClsSpec2\n");
            #endif
            StorClsSpec *scs = new StorClsSpec;
            scs->m_flag = TF_EXTERN;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoStorClsSpec3\n");
            #endif
            StorClsSpec *scs = new StorClsSpec;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec4()
        {
            #ifdef DEEPDEBUG
                std::printf("DoStorClsSpec4\n");
            #endif
            StorClsSpec *scs = new StorClsSpec;
            scs->m_flag = TF_STATIC;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<StorClsSpec> DoStorClsSpec5()
        {
            #ifdef DEEPDEBUG
                std::printf("DoStorClsSpec5\n");
            #endif
            StorClsSpec *scs = new StorClsSpec;
            scs->m_flag = TF_THREADLOCAL;
            return shared_ptr<StorClsSpec>(scs);
        }

        shared_ptr<TypeSpec> DoTypeSpec1()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec1\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_VOID;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec2()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec2\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_CHAR;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec3\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_SHORT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec4()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec4\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_INT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec5()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec5\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_INT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec6()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec6\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_LONGLONG;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec7()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec7\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_LONG;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec8()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec8\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_FLOAT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec9()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec9\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_DOUBLE;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec10()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec10\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_SIGNED;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec11()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec11\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNSIGNED;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec12()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec12\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_INT;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec13()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec13\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = 0;   // TODO: __w64
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec14(shared_ptr<TokenValue>& info)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec14\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ALIAS;
            ts->m_name = info->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec15(shared_ptr<TypeSpec>& struct_spec)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec15\n");
            #endif
            return struct_spec;
        }

        shared_ptr<TypeSpec> DoTypeSpec16(shared_ptr<TypeSpec>& union_spec)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec16\n");
            #endif
            return union_spec;
        }

        shared_ptr<TypeSpec> DoTypeSpec17(shared_ptr<TypeSpec>& enum_spec)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec17\n");
            #endif
            return enum_spec;
        }

        shared_ptr<TypeSpec> DoTypeSpec18(shared_ptr<AtomicTypeSpec>& ats)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec18\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ATOMIC;
            ts->m_atomic_type_spec = ats;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoTypeSpec19()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec19\n");
            #endif
            // TODO: complex
            assert(0);
            return shared_ptr<TypeSpec>();
        }

        shared_ptr<TypeSpec> DoTypeSpec20()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec20\n");
            #endif
            // TODO: imaginary
            assert(0);
            return shared_ptr<TypeSpec>();
        }

        shared_ptr<TypeSpec> DoTypeSpec21()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeSpec21\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_XSIGNED;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<AtomicTypeSpec> DoAtomicTypeSpec1(shared_ptr<TypeName>& tn)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAtomicTypeSpec1\n");
            #endif
            AtomicTypeSpec *ats = new AtomicTypeSpec;
            ats->m_type_name = tn;
            return shared_ptr<AtomicTypeSpec>(ats);
        }

        shared_ptr<TypeQual> DoTypeQual1()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeQual1\n");
            #endif
            TypeQual *tq = new TypeQual;
            tq->m_flag = TF_CONST;
            return shared_ptr<TypeQual>(tq);
        }

        shared_ptr<TypeQual> DoTypeQual2()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeQual2\n");
            #endif
            TypeQual *tq = new TypeQual;
            tq->m_flag = 0;    // restrict is ignored
            return shared_ptr<TypeQual>(tq);
        }

        shared_ptr<TypeQual> DoTypeQual3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeQual3\n");
            #endif
            TypeQual *tq = new TypeQual;
            tq->m_flag = 0;     // volatile is ignored
            return shared_ptr<TypeQual>(tq);
        }

        shared_ptr<TypeSpec> DoStructSpec1(
            shared_ptr<TokenValue>& tag, shared_ptr<DeclList>& decl_list)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructSpec1\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_STRUCT;
            ts->m_pack = tag->m_pack;
            ts->m_name = tag->m_text;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoStructSpec2(
            shared_ptr<TokenValue>& tag, shared_ptr<DeclList>& decl_list)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructSpec2\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_STRUCT;
            ts->m_pack = tag->m_pack;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoStructSpec3(shared_ptr<TokenValue>& tag)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructSpec3\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_STRUCT;
            ts->m_name = tag->m_text;
            ts->m_pack = tag->m_pack;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoUnionSpec1(
            shared_ptr<TokenValue>& tag, shared_ptr<DeclList>& decl_list)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnionSpec1\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNION;
            ts->m_name = tag->m_text;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoUnionSpec2(shared_ptr<DeclList>& decl_list)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnionSpec2\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNION;
            ts->m_decl_list = decl_list;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoUnionSpec3(shared_ptr<TokenValue>& tag)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnionSpec3\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_UNION;
            ts->m_name = tag->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<DeclList> DoStructDeclList1(
            shared_ptr<DeclList>& dl, shared_ptr<Decl>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclList1\n");
            #endif
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclList> DoStructDeclList2(shared_ptr<Decl>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclList2\n");
            #endif
            DeclList *dl = new DeclList;
            dl->push_back(d);
            return shared_ptr<DeclList>(dl);
        }

        shared_ptr<DeclorList> DoTypedefDeclorList1(
            shared_ptr<DeclorList>& dl, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclorList1\n");
            #endif
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclorList> DoTypedefDeclorList2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclorList2\n");
            #endif
            DeclorList *dl = new DeclorList;
            dl->push_back(d);
            return shared_ptr<DeclorList>(dl);
        }

        shared_ptr<DeclorList> DoInitDeclorList1(
            shared_ptr<DeclorList>& dl, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoInitDeclorList1\n");
            #endif
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclorList> DoInitDeclorList2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoInitDeclorList2\n");
            #endif
            DeclorList *dl = new DeclorList;
            dl->push_back(d);
            return shared_ptr<DeclorList>(dl);
        }

        shared_ptr<Declor> DoInitDeclor1(
            shared_ptr<Declor>& d, shared_ptr<Initer>& i)
        {
            #ifdef DEEPDEBUG
                std::printf("DoInitDeclor1\n");
            #endif
            d.get()->m_initer = i;
            return d;
        }

        shared_ptr<Declor> DoInitDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoInitDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Decl> DoStructDecl1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<DeclorList>& dl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDecl1\n");
            #endif
            Decl *d = new Decl;
            d->m_decl_type = Decl::DECLORLIST;
            d->m_decl_specs = ds;
            d->m_declor_list = dl;
            return shared_ptr<Decl>(d);
        }

        shared_ptr<Decl> DoStructDecl2(shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDecl2\n");
            #endif
            Decl *d = new Decl;
            d->m_decl_type = Decl::SINGLE;
            d->m_decl_specs = ds;
            return shared_ptr<Decl>(d);
        }

        shared_ptr<Decl> DoStructDecl3(shared_ptr<StaticAssertDecl>& sad)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDecl3\n");
            #endif
            Decl *d = new Decl;
            d->m_decl_type = Decl::STATIC_ASSERT;
            d->m_static_assert_decl = sad;
            return shared_ptr<Decl>(d);
        }

        shared_ptr<DeclSpecs> DoSpecQualList1(
            shared_ptr<TypeSpec>& ts, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoSpecQualList1\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoSpecQualList2(shared_ptr<TypeSpec>& ts)
        {
            #ifdef DEEPDEBUG
                std::printf("DoSpecQualList2\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPESPEC;
            decl_specs->m_type_spec = ts;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoSpecQualList3(
            shared_ptr<TypeQual>& tq, shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoSpecQualList3\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_decl_specs = ds;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclSpecs> DoSpecQualList4(shared_ptr<TypeQual>& tq)
        {
            #ifdef DEEPDEBUG
                std::printf("DoSpecQualList4\n");
            #endif
            DeclSpecs *decl_specs = new DeclSpecs;
            decl_specs->m_spec_type = DeclSpecs::TYPEQUAL;
            decl_specs->m_type_qual = tq;
            return shared_ptr<DeclSpecs>(decl_specs);
        }

        shared_ptr<DeclorList> DoStructDeclorList1(
            shared_ptr<DeclorList>& dl, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclorList1\n");
            #endif
            dl.get()->push_back(d);
            return dl;
        }

        shared_ptr<DeclorList> DoStructDeclorList2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclorList2\n");
            #endif
            DeclorList *dl = new DeclorList;
            dl->push_back(d);
            return shared_ptr<DeclorList>(dl);
        }

        shared_ptr<Declor> DoStructDeclor1(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclor1\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::BITS;
            declor->m_const_expr = ce;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoStructDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoStructDeclor3(shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStructDeclor3\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::BITS;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<TypeSpec> DoEnumSpec1(
            shared_ptr<TokenValue>& tag, shared_ptr<EnumorList>& el)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumSpec1\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_name = tag->m_text;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec2(
            shared_ptr<TokenValue>& tag, shared_ptr<EnumorList>& el)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumSpec2\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_name = tag->m_text;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec3(shared_ptr<EnumorList>& el)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumSpec3\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec4(shared_ptr<EnumorList>& el)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumSpec4\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_enumor_list = el;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<TypeSpec> DoEnumSpec5(shared_ptr<TokenValue>& tag)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumSpec5\n");
            #endif
            TypeSpec *ts = new TypeSpec;
            ts->m_flag = TF_ENUM;
            ts->m_name = tag->m_text;
            return shared_ptr<TypeSpec>(ts);
        }

        shared_ptr<EnumorList> DoEnumorList1(
            shared_ptr<EnumorList>& el, shared_ptr<Enumor>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumorList1\n");
            #endif
            el.get()->push_back(e);
            return el;
        }

        shared_ptr<EnumorList> DoEnumorList2(shared_ptr<Enumor>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumorList2\n");
            #endif
            EnumorList *el = new EnumorList;
            el->push_back(e);
            return shared_ptr<EnumorList>(el);
        }

        shared_ptr<Enumor> DoEnumor1(
            shared_ptr<TokenValue>& info, shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumor1\n");
            #endif
            Enumor *e = new Enumor;
            e->m_name = info->m_text;
            e->m_const_expr = ce;
            return shared_ptr<Enumor>(e);
        }

        shared_ptr<Enumor> DoEnumor2(shared_ptr<TokenValue>& info)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEnumor2\n");
            #endif
            Enumor *e = new Enumor;
            e->m_name = info->m_text;
            return shared_ptr<Enumor>(e);
        }

        shared_ptr<Declor> DoTypedefDeclor1(
            shared_ptr<Pointers>& ptrs, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclor1\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::POINTERS;
            declor->m_declor = d;
            declor->m_pointers = ptrs;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoDeclor1(
            shared_ptr<Pointers>& ptrs, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclor1\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::POINTERS;
            declor->m_declor = d;
            declor->m_pointers = ptrs;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoTypedefDirDeclor1(shared_ptr<TokenValue>& token)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDirDeclor1\n");
            #endif
            Declor *d = new Declor;
            d->m_declor_type = Declor::TYPEDEF_TAG;
            d->m_name = token->m_text;
            return shared_ptr<Declor>(d);
        }

        shared_ptr<Declor> DoTypedefDirDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDirDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoTypedefDirDeclor3(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDirDeclor3\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDirDeclor4(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDirDeclor4\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoTypedefDirDeclor5(
            shared_ptr<Declor>& d, shared_ptr<ParamList>& pl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDirDeclor5\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        #if 0
            shared_ptr<Declor> DoTypedefDirDeclor6(
                shared_ptr<Declor>& d, shared_ptr<IdentList>& il)
            {
                #ifdef DEEPDEBUG
                    std::printf("DoTypedefDirDeclor6\n");
                #endif
                Declor *declor = new Declor;
                declor->m_declor_type = Declor::FUNCTION;
                declor->m_declor = d;
                declor->m_ident_list = il;
                return shared_ptr<Declor>(declor);
            }
        #endif

        shared_ptr<Declor> DoTypedefDirDeclor7(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypedefDirDeclor7\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor1(shared_ptr<TokenValue>& token)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirDeclor1\n");
            #endif
            Declor *d = new Declor;
            d->m_declor_type = Declor::IDENTIFIER;
            d->m_name = token->m_text;
            d->m_flags = token->m_flags;
            return shared_ptr<Declor>(d);
        }

        shared_ptr<Declor> DoDirDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoDirDeclor3(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirDeclor3\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor4(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirDeclor4\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirDeclor5(
            shared_ptr<Declor>& d, shared_ptr<ParamList>& pl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirDeclor5\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        #if 0
            shared_ptr<Declor> DoDirDeclor6(
                shared_ptr<Declor>& d, shared_ptr<IdentList>& il)
            {
                #ifdef DEEPDEBUG
                    std::printf("DoDirDeclor6\n");
                #endif
                Declor *declor = new Declor;
                declor->m_declor_type = Declor::FUNCTION;
                declor->m_declor = d;
                declor->m_ident_list = il;
                return shared_ptr<Declor>(declor);
            }
        #endif

        shared_ptr<Declor> DoDirDeclor7(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirDeclor7\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Pointers> DoPtr1(
            shared_ptr<AstCom>& ac, shared_ptr<TypeQualList> tql,
            shared_ptr<Pointers>& p)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPtr1\n");
            #endif
            ac.get()->m_flags |= tql->m_flags;
            p.get()->push_back(ac);
            return p;
        }

        shared_ptr<Pointers> DoPtr2(
            shared_ptr<AstCom>& ac, shared_ptr<TypeQualList> tql)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPtr2\n");
            #endif
            Pointers *p = new Pointers;
            ac.get()->m_flags |= tql->m_flags;
            p->push_back(ac);
            return shared_ptr<Pointers>(p);
        }

        shared_ptr<Pointers> DoPtr3(
            shared_ptr<AstCom>& ac, shared_ptr<Pointers>& p)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPtr3\n");
            #endif
            p.get()->push_back(ac);
            return p;
        }

        shared_ptr<Pointers> DoPtr4(shared_ptr<AstCom>& ac)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPtr4\n");
            #endif
            Pointers *p = new Pointers;
            p->push_back(ac);
            return shared_ptr<Pointers>(p);
        }

        shared_ptr<AstCom> DoAstCom1(shared_ptr<TokenValue>& asterisk)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAstCom1\n");
            #endif
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom2(shared_ptr<TokenValue>& asterisk)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAstCom2\n");
            #endif
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom3(shared_ptr<TokenValue>& asterisk)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAstCom3\n");
            #endif
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom4(shared_ptr<TokenValue>& asterisk)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAstCom4\n");
            #endif
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom5(shared_ptr<TokenValue>& asterisk)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAstCom5\n");
            #endif
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<AstCom> DoAstCom6(shared_ptr<TokenValue>& asterisk)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAstCom6\n");
            #endif
            AstCom *ac = new AstCom;
            ac->m_flags = asterisk->m_flags;
            return shared_ptr<AstCom>(ac);
        }

        shared_ptr<TypeQualList> DoTypeQualList1(
            shared_ptr<TypeQualList>& tql, shared_ptr<TypeQual>& tq)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeQualList1\n");
            #endif
            tql.get()->m_flags |= tq->m_flag;
            return tql;
        }

        shared_ptr<TypeQualList> DoTypeQualList2(shared_ptr<TypeQual>& tq)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeQualList2\n");
            #endif
            TypeQualList *tql = new TypeQualList;
            tql->m_flags |= tq->m_flag;
            return shared_ptr<TypeQualList>(tql);
        }

        shared_ptr<ParamList> DoParamTypeList1(shared_ptr<ParamList>& pl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoParamTypeList1\n");
            #endif
            pl.get()->m_ellipsis = true;
            return pl;
        }

        shared_ptr<ParamList> DoParamTypeList2(shared_ptr<ParamList>& pl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoParamTypeList2\n");
            #endif
            return pl;
        }

        shared_ptr<ParamList> DoParamList1(
            shared_ptr<ParamList>& pl, shared_ptr<Decl>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoParamList1\n");
            #endif
            pl.get()->push_back(d);
            return pl;
        }

        shared_ptr<ParamList> DoParamList2(shared_ptr<Decl>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoParamList2\n");
            #endif
            ParamList *pl = new ParamList;
            pl->push_back(d);
            return shared_ptr<ParamList>(pl);
        }

        shared_ptr<Decl> DoParamDecl1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoParamDecl1\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoParamDecl2\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoParamDecl3\n");
            #endif
            Decl *decl = new Decl;
            decl->m_decl_type = Decl::PARAM;
            decl->m_decl_specs = ds;

            DeclorList *declor_list = new DeclorList;
            decl->m_declor_list = shared_ptr<DeclorList>(declor_list);

            return shared_ptr<Decl>(decl);
        }

        #if 0
            shared_ptr<IdentList> DoIdentList1(
                shared_ptr<IdentList>& il, shared_ptr<TokenValue>& id)
            {
                #ifdef DEEPDEBUG
                    std::printf("DoIdentList1\n");
                #endif
                il.get()->push_back(id->m_text);
                return il;
            }

            shared_ptr<IdentList> DoIdentList2(shared_ptr<TokenValue>& id)
            {
                #ifdef DEEPDEBUG
                    std::printf("DoIdentList2\n");
                #endif
                IdentList *il = new IdentList;
                il->push_back(id->m_text);
                return shared_ptr<IdentList>(il);
            }
        #endif

        shared_ptr<Initer> DoIniter1(shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIniter1\n");
            #endif
            Initer *i = new Initer;
            i->m_initer_type = Initer::SIMPLE;
            i->m_assign_expr = ae;
            return shared_ptr<Initer>(i);
        }

        shared_ptr<Initer> DoIniter2(shared_ptr<IniterList>& il)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIniter2\n");
            #endif
            Initer *i = new Initer;
            i->m_initer_type = Initer::COMPLEX;
            i->m_initer_list = il;
            return shared_ptr<Initer>(i);
        }

        shared_ptr<Initer> DoIniter3(shared_ptr<IniterList>& il)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIniter3\n");
            #endif
            Initer *i = new Initer;
            i->m_initer_type = Initer::COMPLEX;
            i->m_initer_list = il;
            return shared_ptr<Initer>(i);
        }

        shared_ptr<IniterList> DoIniterList1(
            shared_ptr<IniterList>& il, shared_ptr<Initer>& i)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIniterList1\n");
            #endif
            il.get()->push_back(i);
            return il;
        }

        shared_ptr<IniterList> DoIniterList2(shared_ptr<Initer>& i)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIniterList2\n");
            #endif
            IniterList *il = new IniterList;
            il->push_back(i);
            return shared_ptr<IniterList>(il);
        }

        shared_ptr<TypeName> DoTypeName1(
            shared_ptr<DeclSpecs>& ds, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeName1\n");
            #endif
            TypeName *tn = new TypeName;
            tn->m_decl_specs = ds;
            tn->m_declor = d;
            return shared_ptr<TypeName>(tn);
        }

        shared_ptr<TypeName> DoTypeName2(shared_ptr<DeclSpecs>& ds)
        {
            #ifdef DEEPDEBUG
                std::printf("DoTypeName2\n");
            #endif
            TypeName *tn = new TypeName;
            tn->m_decl_specs = ds;
            return shared_ptr<TypeName>(tn);
        }

        shared_ptr<Declor> DoAbsDeclor1(
            shared_ptr<Pointers>& ptrs, shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAbsDeclor1\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::POINTERS;
            declor->m_declor = d;
            declor->m_pointers = ptrs;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoAbsDeclor2(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAbsDeclor2\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoAbsDeclor3(shared_ptr<Pointers>& ptrs)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAbsDeclor3\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::POINTERS;
            declor->m_pointers = ptrs;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor1(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor1\n");
            #endif
            return d;
        }

        shared_ptr<Declor> DoDirAbsDeclor2()
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor2\n");
            #endif
            Declor *d = new Declor;
            d->m_declor_type = Declor::ARRAY;
            return shared_ptr<Declor>(d);
        }

        shared_ptr<Declor> DoDirAbsDeclor3(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor3\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor4(shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor4\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor5(
            shared_ptr<Declor>& d, shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor5\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::ARRAY;
            declor->m_declor = d;
            declor->m_const_expr = ce;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor6()
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor6\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor7(shared_ptr<Declor>& d)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor7\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor8(shared_ptr<ParamList>& pl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor8\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<Declor> DoDirAbsDeclor9(
            shared_ptr<Declor>& d, shared_ptr<ParamList>& pl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoDirAbsDeclor9\n");
            #endif
            Declor *declor = new Declor;
            declor->m_declor_type = Declor::FUNCTION;
            declor->m_declor = d;
            declor->m_param_list = pl;
            return shared_ptr<Declor>(declor);
        }

        shared_ptr<StmtList> DoStmtList1(
            shared_ptr<StmtList>& sl, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmtList1\n");
            #endif
            sl.get()->push_back(s);
            return sl;
        }

        shared_ptr<StmtList> DoStmtList2(shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmtList2\n");
            #endif
            StmtList *sl = new StmtList;
            sl->push_back(s);
            return shared_ptr<StmtList>(sl);
        }

        shared_ptr<Stmt> DoStmt1(shared_ptr<LabeledStmt>& ls)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmt1\n");
            #endif
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::LABELED;
            s->m_labeled_stmt = ls;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt2(shared_ptr<ExprStmt>& es)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmt2\n");
            #endif
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::EXPR;
            s->m_expr_stmt = es;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt3(shared_ptr<CompStmt>& cs)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmt3\n");
            #endif
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::COMP;
            s->m_comp_stmt = cs;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt4(shared_ptr<SelStmt>& ss)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmt4\n");
            #endif
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::SEL;
            s->m_sel_stmt = ss;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt5(shared_ptr<IterStmt>& is)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmt5\n");
            #endif
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::ITER;
            s->m_iter_stmt = is;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<Stmt> DoStmt6(shared_ptr<JumpStmt>& js)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStmt6\n");
            #endif
            Stmt *s = new Stmt;
            s->m_stmt_type = Stmt::JUMP;
            s->m_jump_stmt = js;
            return shared_ptr<Stmt>(s);
        }

        shared_ptr<LabeledStmt> DoLabeledStmt1(
            shared_ptr<TokenValue>& id, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLabeledStmt1\n");
            #endif
            LabeledStmt *ls = new LabeledStmt;
            ls->m_labeled_type = LabeledStmt::LABEL;
            ls->m_label = id->m_text.c_str();
            ls->m_stmt = s;
            return shared_ptr<LabeledStmt>(ls);
        }

        shared_ptr<LabeledStmt> DoLabeledStmt2(
            shared_ptr<CondExpr>& ce, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLabeledStmt2\n");
            #endif
            LabeledStmt *ls = new LabeledStmt;
            ls->m_labeled_type = LabeledStmt::CASE;
            ls->m_const_expr = ce;
            ls->m_stmt = s;
            return shared_ptr<LabeledStmt>(ls);
        }

        shared_ptr<LabeledStmt> DoLabeledStmt3(shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLabeledStmt3\n");
            #endif
            LabeledStmt *ls = new LabeledStmt;
            ls->m_labeled_type = LabeledStmt::DEFAULT;
            ls->m_stmt = s;
            return shared_ptr<LabeledStmt>(ls);
        }

        shared_ptr<ExprStmt> DoExprStmt1()
        {
            #ifdef DEEPDEBUG
                std::printf("DoExprStmt1\n");
            #endif
            return shared_ptr<ExprStmt>(new ExprStmt);
        }

        shared_ptr<ExprStmt> DoExprStmt2(shared_ptr<Expr>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExprStmt2\n");
            #endif
            ExprStmt *es = new ExprStmt;
            es->m_expr = e;
            return shared_ptr<ExprStmt>(es);
        }

        shared_ptr<CompStmt> DoCompStmt1()
        {
            #ifdef DEEPDEBUG
                std::printf("DoCompStmt1\n");
            #endif
            return shared_ptr<CompStmt>(new CompStmt);
        }

        shared_ptr<CompStmt> DoCompStmt2(shared_ptr<DeclList>& dl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCompStmt2\n");
            #endif
            CompStmt *cs = new CompStmt;
            cs->m_decl_list = dl;
            return shared_ptr<CompStmt>(cs);
        }

        shared_ptr<CompStmt> DoCompStmt3(shared_ptr<StmtList>& sl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCompStmt3\n");
            #endif
            CompStmt *cs = new CompStmt;
            cs->m_stmt_list = sl;
            return shared_ptr<CompStmt>(cs);
        }

        shared_ptr<CompStmt> DoCompStmt4(
            shared_ptr<DeclList>& dl, shared_ptr<StmtList>& sl)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCompStmt4\n");
            #endif
            CompStmt *cs = new CompStmt;
            cs->m_decl_list = dl;
            cs->m_stmt_list = sl;
            return shared_ptr<CompStmt>(cs);
        }

        shared_ptr<SelStmt> DoSelStmt1(
            shared_ptr<Expr>& e, shared_ptr<Stmt>& then_s, shared_ptr<Stmt>& else_s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoSelStmt1\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoSelStmt2\n");
            #endif
            SelStmt *ss = new SelStmt;
            ss->m_sel_type = SelStmt::IF_THEN;
            ss->m_expr = e;
            ss->m_then = then_s;
            return shared_ptr<SelStmt>(ss);
        }

        shared_ptr<SelStmt> DoSelStmt3(
            shared_ptr<Expr>& e, shared_ptr<Stmt>& then_s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoSelStmt3\n");
            #endif
            SelStmt *ss = new SelStmt;
            ss->m_sel_type = SelStmt::SWITCH;
            ss->m_expr = e;
            ss->m_then = then_s;
            return shared_ptr<SelStmt>(ss);
        }

        shared_ptr<IterStmt> DoIterStmt1(shared_ptr<Expr>& e, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt1\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::WHILE;
            is->m_expr1 = e;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt2(shared_ptr<Stmt>& s, shared_ptr<Expr>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt2\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::DO_WHILE;
            is->m_expr1 = e;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt3(shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt3\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt4(shared_ptr<Expr>& e3, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt4\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr3 = e3;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt5(shared_ptr<Expr>& e2, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt5\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr2 = e2;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt6(
            shared_ptr<Expr>& e2, shared_ptr<Expr>& e3, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt6\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e2;
            is->m_expr2 = e3;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt7(shared_ptr<Expr>& e1, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt7\n");
            #endif
            IterStmt *is = new IterStmt;
            is->m_iter_type = IterStmt::FOR;
            is->m_expr1 = e1;
            is->m_stmt = s;
            return shared_ptr<IterStmt>(is);
        }

        shared_ptr<IterStmt> DoIterStmt8(
            shared_ptr<Expr>& e1, shared_ptr<Expr>& e3, shared_ptr<Stmt>& s)
        {
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt8\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt9\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoIterStmt10\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoJumpStmt1\n");
            #endif
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::GOTO;
            js->m_label = id->m_text;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt2()
        {
            #ifdef DEEPDEBUG
                std::printf("DoJumpStmt2\n");
            #endif
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::CONTINUE;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoJumpStmt3\n");
            #endif
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::BREAK;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt4()
        {
            #ifdef DEEPDEBUG
                std::printf("DoJumpStmt4\n");
            #endif
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::RETURN_VOID;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<JumpStmt> DoJumpStmt5(shared_ptr<Expr>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoJumpStmt5\n");
            #endif
            JumpStmt *js = new JumpStmt;
            js->m_jump_type = JumpStmt::RETURN_EXPR;
            js->m_expr = e;
            return shared_ptr<JumpStmt>(js);
        }

        shared_ptr<Expr> DoExpr1(shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExpr1\n");
            #endif
            Expr *e = new Expr;
            e->push_back(ae);
            return shared_ptr<Expr>(e);
        }

        shared_ptr<Expr> DoExpr2(
            shared_ptr<Expr>& e, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExpr2\n");
            #endif
            e.get()->push_back(ae);
            return e;
        }

        shared_ptr<AssignExpr> DoAssignExpr1(shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr1\n");
            #endif
            AssignExpr *ae = new AssignExpr;
            ae->m_assign_type = AssignExpr::COND;
            ae->m_cond_expr = ce;
            return shared_ptr<AssignExpr>(ae);
        }

        shared_ptr<AssignExpr> DoAssignExpr2(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr2\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::SINGLE;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr3(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr3\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::MUL;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr4(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr4\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::DIV;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr5(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr5\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::MOD;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr6(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr6\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::ADD;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr7(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr7\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::SUB;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr8(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr8\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::L_SHIFT;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr9(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr9\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::R_SHIFT;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr10(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr10\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::AND;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr11(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr11\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::XOR;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<AssignExpr> DoAssignExpr12(
            shared_ptr<UnaryExpr>& ue, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAssignExpr12\n");
            #endif
            AssignExpr *newae = new AssignExpr;
            newae->m_assign_type = AssignExpr::OR;
            newae->m_unary_expr = ue;
            newae->m_assign_expr = ae;
            return shared_ptr<AssignExpr>(newae);
        }

        shared_ptr<CondExpr> DoCondExpr1(
            shared_ptr<LogOrExpr>& loe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCondExpr1\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoCondExpr2\n");
            #endif
            CondExpr *newce = new CondExpr;
            newce->m_cond_type = CondExpr::QUESTION;
            newce->m_log_or_expr = loe;
            newce->m_expr = e;
            newce->m_cond_expr = ce;
            return shared_ptr<CondExpr>(newce);
        }

        shared_ptr<CondExpr> DoConstExpr1(shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoConstExpr1\n");
            #endif
            return ce;
        }

        shared_ptr<LogOrExpr> DoLogOrExpr1(shared_ptr<LogAndExpr>& lae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLogOrExpr1\n");
            #endif
            LogOrExpr *loe = new LogOrExpr;
            loe->push_back(lae);
            return shared_ptr<LogOrExpr>(loe);
        }

        shared_ptr<LogOrExpr> DoLogOrExpr2(
            shared_ptr<LogOrExpr>& loe, shared_ptr<LogAndExpr>& lae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLogOrExpr2\n");
            #endif
            loe.get()->push_back(lae);
            return loe;
        }

        shared_ptr<LogAndExpr> DoLogAnd1(shared_ptr<InclOrExpr>& ioe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLogAnd1\n");
            #endif
            LogAndExpr *lae = new LogAndExpr;
            lae->push_back(ioe);
            return shared_ptr<LogAndExpr>(lae);
        }

        shared_ptr<LogAndExpr> DoLogAnd2(
            shared_ptr<LogAndExpr>& lae, shared_ptr<InclOrExpr>& ioe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoLogAnd2\n");
            #endif
            lae.get()->push_back(ioe);
            return lae;
        }

        shared_ptr<InclOrExpr> DoInclOrExpr1(shared_ptr<ExclOrExpr>& eoe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoInclOrExpr1\n");
            #endif
            InclOrExpr *ioe = new InclOrExpr;
            ioe->push_back(eoe);
            return shared_ptr<InclOrExpr>(ioe);
        }

        shared_ptr<InclOrExpr> DoInclOrExpr2(
            shared_ptr<InclOrExpr>& ioe,
            shared_ptr<ExclOrExpr>& eoe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoInclOrExpr2\n");
            #endif
            ioe.get()->push_back(eoe);
            return ioe;
        }

        shared_ptr<ExclOrExpr> DoExclOr1(shared_ptr<AndExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExclOr1\n");
            #endif
            ExclOrExpr *eoe = new ExclOrExpr;
            eoe->push_back(ae);
            return shared_ptr<ExclOrExpr>(eoe);
        }

        shared_ptr<ExclOrExpr> DoExclOr2(
            shared_ptr<ExclOrExpr>& eoe, shared_ptr<AndExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoExclOr2\n");
            #endif
            eoe.get()->push_back(ae);
            return eoe;
        }

        shared_ptr<AndExpr> DoAndExpr1(shared_ptr<EqualExpr>& ee)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAndExpr1\n");
            #endif
            AndExpr *ae = new AndExpr;
            ae->push_back(ee);
            return shared_ptr<AndExpr>(ae);
        }

        shared_ptr<AndExpr> DoAndExpr2(
            shared_ptr<AndExpr>& ae,
            shared_ptr<EqualExpr>& ee)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAndExpr2\n");
            #endif
            ae.get()->push_back(ee);
            return ae;
        }

        shared_ptr<EqualExpr> DoEqualExpr1(shared_ptr<RelExpr>& re)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEqualExpr1\n");
            #endif
            EqualExpr *ee = new EqualExpr;
            ee->m_equal_type = EqualExpr::SINGLE;
            ee->m_rel_expr = re;
            return shared_ptr<EqualExpr>(ee);
        }

        shared_ptr<EqualExpr> DoEqualExpr2(
            shared_ptr<EqualExpr>& ee,
            shared_ptr<RelExpr>& re)
        {
            #ifdef DEEPDEBUG
                std::printf("DoEqualExpr2\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoEqualExpr3\n");
            #endif
            EqualExpr *newee = new EqualExpr;
            newee->m_equal_type = EqualExpr::NE;
            newee->m_equal_expr = ee;
            newee->m_rel_expr = re;
            return shared_ptr<EqualExpr>(ee);
        }

        shared_ptr<RelExpr> DoRelExpr1(shared_ptr<ShiftExpr>& se)
        {
            #ifdef DEEPDEBUG
                std::printf("DoRelExpr1\n");
            #endif
            RelExpr *re = new RelExpr;
            re->m_rel_type = RelExpr::SINGLE;
            re->m_shift_expr = se;
            return shared_ptr<RelExpr>(re);
        }

        shared_ptr<RelExpr> DoRelExpr2(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            #ifdef DEEPDEBUG
                std::printf("DoRelExpr2\n");
            #endif
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::LT;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<RelExpr> DoRelExpr3(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            #ifdef DEEPDEBUG
                std::printf("DoRelExpr3\n");
            #endif
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::GT;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<RelExpr> DoRelExpr4(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            #ifdef DEEPDEBUG
                std::printf("DoRelExpr4\n");
            #endif
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::LE;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<RelExpr> DoRelExpr5(
            shared_ptr<RelExpr>& re, shared_ptr<ShiftExpr>& se)
        {
            #ifdef DEEPDEBUG
                std::printf("DoRelExpr5\n");
            #endif
            RelExpr *newre = new RelExpr;
            newre->m_rel_type = RelExpr::GE;
            newre->m_rel_expr = re;
            newre->m_shift_expr = se;
            return shared_ptr<RelExpr>(newre);
        }

        shared_ptr<ShiftExpr> DoShiftExpr1(shared_ptr<AddExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoShiftExpr1\n");
            #endif
            ShiftExpr *se = new ShiftExpr;
            se->m_shift_type = ShiftExpr::SINGLE;
            se->m_add_expr = ae;
            return shared_ptr<ShiftExpr>(se);
        }

        shared_ptr<ShiftExpr> DoShiftExpr2(
            shared_ptr<ShiftExpr>& se, shared_ptr<AddExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoShiftExpr2\n");
            #endif
            ShiftExpr *newse = new ShiftExpr;
            newse->m_shift_type = ShiftExpr::L_SHIFT;
            newse->m_shift_expr = se;
            newse->m_add_expr = ae;
            return shared_ptr<ShiftExpr>(newse);
        }

        shared_ptr<ShiftExpr> DoShiftExpr3(
            shared_ptr<ShiftExpr>& se, shared_ptr<AddExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoShiftExpr3\n");
            #endif
            ShiftExpr *newse = new ShiftExpr;
            newse->m_shift_type = ShiftExpr::R_SHIFT;
            newse->m_shift_expr = se;
            newse->m_add_expr = ae;
            return shared_ptr<ShiftExpr>(newse);
        }

        shared_ptr<AddExpr> DoAddExpr1(shared_ptr<MulExpr>& me)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAddExpr1\n");
            #endif
            AddExpr *ae = new AddExpr;
            ae->m_add_type = AddExpr::SINGLE;
            ae->m_mul_expr = me;
            return shared_ptr<AddExpr>(ae);
        }

        shared_ptr<AddExpr> DoAddExpr2(
            shared_ptr<AddExpr>& ae, shared_ptr<MulExpr>& me)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAddExpr2\n");
            #endif
            AddExpr *newae = new AddExpr;
            newae->m_add_type = AddExpr::PLUS;
            newae->m_add_expr = ae;
            newae->m_mul_expr = me;
            return shared_ptr<AddExpr>(newae);
        }

        shared_ptr<AddExpr> DoAddExpr3(
            shared_ptr<AddExpr>& ae, shared_ptr<MulExpr>& me)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAddExpr3\n");
            #endif
            AddExpr *newae = new AddExpr;
            newae->m_add_type = AddExpr::MINUS;
            newae->m_add_expr = ae;
            newae->m_mul_expr = me;
            return shared_ptr<AddExpr>(newae);
        }

        shared_ptr<MulExpr> DoMulExpr1(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoMulExpr1\n");
            #endif
            MulExpr *me = new MulExpr;
            me->m_mul_type = MulExpr::SINGLE;
            me->m_cast_expr = ce;
            return shared_ptr<MulExpr>(me);
        }

        shared_ptr<MulExpr> DoMulExpr2(
            shared_ptr<MulExpr>& me, shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoMulExpr2\n");
            #endif
            MulExpr *newme = new MulExpr;
            newme->m_mul_type = MulExpr::ASTERISK;
            newme->m_mul_expr = me;
            newme->m_cast_expr = ce;
            return shared_ptr<MulExpr>(newme);
        }

        shared_ptr<MulExpr> DoMulExpr3(
            shared_ptr<MulExpr>& me, shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoMulExpr3\n");
            #endif
            MulExpr *newme = new MulExpr;
            newme->m_mul_type = MulExpr::SLASH;
            newme->m_mul_expr = me;
            newme->m_cast_expr = ce;
            return shared_ptr<MulExpr>(newme);
        }

        shared_ptr<MulExpr> DoMulExpr4(
            shared_ptr<MulExpr>& me, shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoMulExpr4\n");
            #endif
            MulExpr *newme = new MulExpr;
            newme->m_mul_type = MulExpr::PERCENT;
            newme->m_mul_expr = me;
            newme->m_cast_expr = ce;
            return shared_ptr<MulExpr>(newme);
        }

        shared_ptr<CastExpr> DoCastExpr1(shared_ptr<UnaryExpr>& ue)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCastExpr1\n");
            #endif
            CastExpr *ce = new CastExpr;
            ce->m_cast_type = CastExpr::UNARY;
            ce->m_unary_expr = ue;
            return shared_ptr<CastExpr>(ce);
        }

        shared_ptr<CastExpr> DoCastExpr2(
            shared_ptr<TypeName>& tn, shared_ptr<IniterList>& il)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCastExpr2\n");
            #endif
            CastExpr *ce = new CastExpr;
            ce->m_cast_type = CastExpr::INITERLIST;
            ce->m_type_name = tn;
            ce->m_initer_list = il;
            return shared_ptr<CastExpr>(ce);
        }

        shared_ptr<CastExpr> DoCastExpr3(
            shared_ptr<TypeName>& tn, shared_ptr<IniterList>& il)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCastExpr3\n");
            #endif
            CastExpr *ce = new CastExpr;
            ce->m_cast_type = CastExpr::INITERLIST;
            ce->m_type_name = tn;
            ce->m_initer_list = il;
            return shared_ptr<CastExpr>(ce);
        }

        shared_ptr<CastExpr> DoCastExpr4(
            shared_ptr<TypeName>& tn, shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoCastExpr4\n");
            #endif
            CastExpr *newce = new CastExpr;
            newce->m_cast_type = CastExpr::CAST;
            newce->m_type_name = tn;
            newce->m_cast_expr = ce;
            return shared_ptr<CastExpr>(newce);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr1(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr1\n");
            #endif
            UnaryExpr *ue = new UnaryExpr;
            ue->m_unary_type = UnaryExpr::SINGLE;
            ue->m_postfix_expr = pe;
            return shared_ptr<UnaryExpr>(ue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr2(shared_ptr<UnaryExpr>& ue)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr2\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::INC;
            newue->m_unary_expr = ue;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr3(shared_ptr<UnaryExpr>& ue)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr3\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::DEC;
            newue->m_unary_expr = ue;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr4(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr4\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::AND;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr5(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr5\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::ASTERISK;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr6(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr6\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::PLUS;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr7(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr7\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::MINUS;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr8(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr8\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::BITWISE_NOT;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr9(shared_ptr<CastExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr9\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::NOT;
            newue->m_cast_expr = ce;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr10(shared_ptr<UnaryExpr>& ue)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr10\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::SIZEOF1;
            newue->m_unary_expr = ue;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr11(shared_ptr<TypeName>& tn)
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr11\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::SIZEOF2;
            newue->m_type_name = tn;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<UnaryExpr> DoUnaryExpr12()
        {
            #ifdef DEEPDEBUG
                std::printf("DoUnaryExpr12\n");
            #endif
            UnaryExpr *newue = new UnaryExpr;
            newue->m_unary_type = UnaryExpr::ALIGNOF;
            return shared_ptr<UnaryExpr>(newue);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr1(shared_ptr<PrimExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr1\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::SINGLE;
            newpe->m_prim_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr2(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr2\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::ARRAYITEM;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr3(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr3\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::FUNCCALL1;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr4(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr4\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::FUNCCALL2;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr5(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr5\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::DOT;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr6(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr6\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::ARROW;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr7(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr7\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::INC;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PostfixExpr> DoPostfixExpr8(shared_ptr<PostfixExpr>& pe)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPostfixExpr8\n");
            #endif
            PostfixExpr *newpe = new PostfixExpr;
            newpe->m_postfix_type = PostfixExpr::DEC;
            newpe->m_postfix_expr = pe;
            return shared_ptr<PostfixExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr1(shared_ptr<TokenValue>& token)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPrimExpr1\n");
            #endif
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::IDENTIFIER;
            newpe->m_text = token->m_text;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr2(shared_ptr<TokenValue>& token)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPrimExpr2\n");
            #endif
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
            #ifdef DEEPDEBUG
                std::printf("DoPrimExpr3\n");
            #endif
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::STRING;
            newpe->m_text = token->m_text;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr4(shared_ptr<Expr>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoPrimExpr4\n");
            #endif
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::PAREN;
            newpe->m_expr = e;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<PrimExpr> DoPrimExpr5()
        {
            #ifdef DEEPDEBUG
                std::printf("DoPrimExpr5\n");
            #endif
            PrimExpr *newpe = new PrimExpr;
            newpe->m_prim_type = PrimExpr::SELECTION;
            return shared_ptr<PrimExpr>(newpe);
        }

        shared_ptr<GeneSel> DoGeneSel1(
            shared_ptr<AssignExpr>& ae, shared_ptr<GeneAssocList>& gal)
        {
            #ifdef DEEPDEBUG
                std::printf("DoGeneSel1\n");
            #endif
            GeneSel *gs = new GeneSel;
            gs->m_assign_expr = ae;
            gs->m_gene_assoc_list = gal;
            return shared_ptr<GeneSel>(gs);
        }

        shared_ptr<GeneAssocList> DoGeneAssocList1(
            shared_ptr<GeneAssocList>& gal, shared_ptr<GeneAssoc>& ga)
        {
            #ifdef DEEPDEBUG
                std::printf("DoGeneAssocList1\n");
            #endif
            gal.get()->push_back(ga);
            return gal;
        }

        shared_ptr<GeneAssocList> DoGeneAssocList2(shared_ptr<GeneAssoc>& ga)
        {
            #ifdef DEEPDEBUG
                std::printf("DoGeneAssocList2\n");
            #endif
            GeneAssocList *gal = new GeneAssocList;
            gal->push_back(ga);
            return shared_ptr<GeneAssocList>(gal);
        }

        shared_ptr<GeneAssoc> DoGeneAssoc1(
            shared_ptr<TypeName>& tn, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoGeneAssoc1\n");
            #endif
            GeneAssoc *ga = new GeneAssoc;
            ga->m_gene_assoc_type = GeneAssoc::NONDEFAULT;
            ga->m_type_name = tn;
            ga->m_assign_expr = ae;
            return shared_ptr<GeneAssoc>(ga);
        }

        shared_ptr<GeneAssoc> DoGeneAssoc2(shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoGeneAssoc2\n");
            #endif
            GeneAssoc *ga = new GeneAssoc;
            ga->m_gene_assoc_type = GeneAssoc::DEFAULT;
            ga->m_assign_expr = ae;
            return shared_ptr<GeneAssoc>(ga);
        }

        shared_ptr<ArgExprList> DoArgExprList1(
            shared_ptr<ArgExprList>& ael, shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoArgExprList1\n");
            #endif
            ael.get()->push_back(ae);
            return ael;
        }

        shared_ptr<ArgExprList> DoArgExprList2(shared_ptr<AssignExpr>& ae)
        {
            #ifdef DEEPDEBUG
                std::printf("DoArgExprList2\n");
            #endif
            ArgExprList *ael = new ArgExprList;
            ael->push_back(ae);
            return shared_ptr<ArgExprList>(ael);
        }

        shared_ptr<FuncSpec> DoFuncSpec1()
        {
            #ifdef DEEPDEBUG
                std::printf("DoFuncSpec1\n");
            #endif
            FuncSpec *fs = new FuncSpec;
            fs->m_flag = TF_INLINE;
            return shared_ptr<FuncSpec>(fs);
        }

        shared_ptr<FuncSpec> DoFuncSpec2()
        {
            #ifdef DEEPDEBUG
                std::printf("DoFuncSpec2\n");
            #endif
            FuncSpec *fs = new FuncSpec;
            fs->m_flag = TF_INLINE;
            return shared_ptr<FuncSpec>(fs);
        }

        shared_ptr<FuncSpec> DoFuncSpec3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoFuncSpec3\n");
            #endif
            FuncSpec *fs = new FuncSpec;
            fs->m_flag = 0;
            return shared_ptr<FuncSpec>(fs);
        }

        shared_ptr<AlignSpec> DoAlignSpec1(shared_ptr<TypeName>& tn)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAlignSpec1\n");
            #endif
            AlignSpec *as = new AlignSpec;
            as->m_align_spec_type = AlignSpec::TYPENAME;
            as->m_type_name = tn;
            return shared_ptr<AlignSpec>(as);
        }

        shared_ptr<AlignSpec> DoAlignSpec2(shared_ptr<CondExpr>& ce)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAlignSpec2\n");
            #endif
            AlignSpec *as = new AlignSpec;
            as->m_align_spec_type = AlignSpec::CONSTEXPR;
            as->m_const_expr = ce;
            return shared_ptr<AlignSpec>(as);
        }

        shared_ptr<StaticAssertDecl> DoStaticAssertDecl1(
            shared_ptr<CondExpr>& ce, shared_ptr<TokenValue>& str)
        {
            #ifdef DEEPDEBUG
                std::printf("DoStaticAssertDecl1\n");
            #endif
            StaticAssertDecl *sad = new StaticAssertDecl;
            sad->m_const_expr = ce;
            sad->m_str = str->m_text;
            return shared_ptr<StaticAssertDecl>(sad);
        }

        shared_ptr<AsmSpec> DoAsmSpec1(
            shared_ptr<TypeQualList>& tql, shared_ptr<AsmOperands>& aos)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmSpec1\n");
            #endif
            AsmSpec *as = new AsmSpec;
            as->m_type_qual_list = tql;
            as->m_asm_operands = aos;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmSpec> DoAsmSpec2(shared_ptr<TypeQualList>& tql)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmSpec2\n");
            #endif
            AsmSpec *as = new AsmSpec;
            as->m_type_qual_list = tql;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmSpec> DoAsmSpec3(shared_ptr<AsmOperands>& aos)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmSpec3\n");
            #endif
            AsmSpec *as = new AsmSpec;
            as->m_asm_operands = aos;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmSpec> DoAsmSpec4()
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmSpec4\n");
            #endif
            AsmSpec *as = new AsmSpec;
            return shared_ptr<AsmSpec>(as);
        }

        shared_ptr<AsmBlock> DoAsmBlock1(shared_ptr<AsmOperands>& aos)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmBlock1\n");
            #endif
            AsmBlock *ab = new AsmBlock;
            ab->m_asm_operands = aos;
            return shared_ptr<AsmBlock>(ab);
        }

        shared_ptr<AsmOperands> DoAsmOperands1(
            shared_ptr<AsmOperands>& aos, shared_ptr<AsmOperand>& ao)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperands1\n");
            #endif
            aos.get()->push_back(ao);
            return aos;
        }

        shared_ptr<AsmOperands> DoAsmOperands2(shared_ptr<AsmOperand>& ao)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperands2\n");
            #endif
            AsmOperands *aos = new AsmOperands;
            aos->push_back(ao);
            return shared_ptr<AsmOperands>(aos);
        }

        shared_ptr<AsmOperand> DoAsmOperand1(shared_ptr<TokenValue>& value)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperand1\n");
            #endif
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::STRING;
            ao->m_text = value->m_text;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand2(shared_ptr<TokenValue>& value)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperand2\n");
            #endif
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::IDENTIFIER;
            ao->m_text = value->m_text;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand3()
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperand3\n");
            #endif
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::COMMA;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand4()
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperand4\n");
            #endif
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::COLON;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand5(shared_ptr<Expr>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperand5\n");
            #endif
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::PAREN;
            ao->m_expr = e;
            return shared_ptr<AsmOperand>(ao);
        }

        shared_ptr<AsmOperand> DoAsmOperand6(shared_ptr<Expr>& e)
        {
            #ifdef DEEPDEBUG
                std::printf("DoAsmOperand6\n");
            #endif
            AsmOperand *ao = new AsmOperand;
            ao->m_operand_type = AsmOperand::BRACKET;
            ao->m_expr = e;
            return shared_ptr<AsmOperand>(ao);
        }

    public:
              CR_Location& location()       { return m_loc; }
        const CR_Location& location() const { return m_loc; }

        //
        // errors and warnings
        //
        void message(const std::string& str)
        {
            std::string loc = location().to_string();
            printf("%s: %s\n", loc.c_str(), str.c_str());
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
        CR_Location                     m_loc;
        int                             m_num_errors;
        int                             m_num_warnings;
    };
} // namespace cparser

#endif  // ndef PARSERSITE_H_
