////////////////////////////////////////////////////////////////////////////
// TypeSystem.h
// Copyright (C) 2014 Katayama Hirofumi MZ.  All rights reserved.
////////////////////////////////////////////////////////////////////////////
// This file is part of CodeReverse.
////////////////////////////////////////////////////////////////////////////

#ifndef TYPESYSTEM_H_
#define TYPESYSTEM_H_

////////////////////////////////////////////////////////////////////////////
// CR_TypeFlags

enum
{
    TF_VOID         = (1 << 0),
    TF_CHAR         = (1 << 1),
    TF_SHORT        = (1 << 2),
    TF_LONG         = (1 << 3),
    TF_LONGLONG     = (1 << 4),
    TF_INT          = (1 << 5),
    TF_VA_LIST      = (1 << 6),
    TF_FLOAT        = (1 << 7),
    TF_DOUBLE       = (1 << 8),
    TF_SIGNED       = 0,
    TF_UNSIGNED     = (1 << 9),
    TF_XSIGNED      = (1 << 10), // CodeReverse extension: signed and/or unsigned
    TF_STRUCT       = (1 << 11),
    TF_UNION        = (1 << 12),
    TF_ENUM         = (1 << 13),
    TF_POINTER      = (1 << 14),
    TF_ARRAY        = (1 << 15),
    TF_FUNCTION     = (1 << 16),
    TF_CDECL        = (1 << 17),
    TF_STDCALL      = (1 << 18),
    TF_FASTCALL     = (1 << 19),
    TF_CONST        = (1 << 20),
    TF_VOLATILE     = (1 << 21),
    TF_COMPLEX      = (1 << 22),
    TF_IMAGINARY    = (1 << 23),
    TF_ATOMIC       = (1 << 24),
    TF_EXTERN       = (1 << 25),
    TF_STATIC       = (1 << 26),
    TF_THREADLOCAL  = (1 << 27),
    TF_INLINE       = (1 << 28),
    TF_ALIAS        = (1 << 29)
};
typedef unsigned long CR_TypeFlags;

////////////////////////////////////////////////////////////////////////////
// CrNormalizeTypeFlags

inline CR_TypeFlags CrNormalizeTypeFlags(CR_TypeFlags flags)
{
    if (flags & TF_INT)
    {
        if (flags & TF_SHORT)
            flags &= ~TF_INT;
        else if (flags & TF_LONG)
            flags &= ~TF_INT;
        else if (flags & TF_LONGLONG)
            flags &= ~TF_INT;
    }
    if ((flags & TF_UNSIGNED) &&
        !(flags & (TF_CHAR | TF_SHORT | TF_LONG | TF_LONGLONG | TF_INT)))
    {
        flags |= TF_INT;
    }
    if (flags == 0)
        flags = TF_INT;
    return flags & ~(TF_EXTERN | TF_STATIC);
}

////////////////////////////////////////////////////////////////////////////
// IDs

// CR_ID --- ID
typedef std::size_t CR_ID;

// CR_TypeID --- type ID
typedef CR_ID CR_TypeID;

// CR_FuncID --- function ID
typedef CR_ID CR_FuncID;

// CR_VarID --- variable ID
typedef CR_ID CR_VarID;

// CR_StructID --- struct or union ID
typedef CR_ID CR_StructID;

// CR_EnumID --- enum ID
typedef CR_ID CR_EnumID;

// cr_invalid_id --- invalid ID
#define cr_invalid_id   static_cast<CR_ID>(-1)

////////////////////////////////////////////////////////////////////////////
// CR_TypeSet

typedef CR_DeqSet<CR_TypeID> CR_TypeSet;

////////////////////////////////////////////////////////////////////////////
// CR_LogFunc

struct CR_LogFunc
{
    bool                    m_ellipsis;
    CR_TypeSet              m_type_list;
    CR_StringSet            m_name_list;
    CR_TypeID               m_return_type;
    enum {
        FT_CDECL, FT_STDCALL, FT_FASTCALL
    } m_func_type;

    CR_LogFunc() :
        m_ellipsis(false),
        m_return_type(0),
        m_func_type(FT_CDECL)
    {
    }

    CR_LogFunc(const CR_LogFunc& lf) :
        m_ellipsis(lf.m_ellipsis),
        m_type_list(lf.m_type_list),
        m_name_list(lf.m_name_list),
        m_return_type(lf.m_return_type),
        m_func_type(lf.m_func_type)
    {
    }

    void operator=(const CR_LogFunc& lf)
    {
        m_ellipsis = lf.m_ellipsis;
        m_type_list = lf.m_type_list;
        m_name_list = lf.m_name_list;
        m_return_type = lf.m_return_type;
        m_func_type = lf.m_func_type;
    }
};

////////////////////////////////////////////////////////////////////////////
// CR_LogType

struct CR_LogType
{
    CR_TypeFlags m_flags;

    // For TF_POINTER:              the type ID (CR_TypeID)
    // For TF_ARRAY:                the type ID (CR_TypeID)
    // For TF_CONST:                the type ID (CR_TypeID)
    // For TF_CONST | TF_POINTER:   the type ID (CR_TypeID)
    // For TF_FUNCTION:             the function ID (CR_FuncID)
    // For TF_STRUCT:               the struct ID (CR_StructID)
    // For TF_ENUM:                 the enum ID (CR_EnumID)
    // For TF_UNION:                the union ID (CR_UnionID)
    // otherwise: zero
    CR_ID        m_id;

    int          m_count;   // for TF_ARRAY

    CR_LogType() : m_flags(0), m_id(0), m_count(0) { }

    CR_LogType(CR_TypeFlags flags) : m_flags(flags), m_id(0), m_count(0) { }

    bool operator==(const CR_LogType& type) const
    {
        return m_flags == type.m_flags &&
               m_id == type.m_id &&
               m_count == type.m_count;
    }

    bool operator!=(const CR_LogType& type) const
    {
        return m_flags != type.m_flags ||
               m_id != type.m_id ||
               m_count != type.m_count;
    }
};

////////////////////////////////////////////////////////////////////////////
// CR_LogStruct -- logical structure or union

struct CR_LogStruct
{
    bool                    m_struct_or_union;
    CR_TypeSet              m_type_list;
    CR_StringSet            m_name_list;
    CR_DeqSet<int>          m_bitfield;
    int                     m_pack;

    CR_LogStruct(bool struct_or_union = true) :
        m_struct_or_union(struct_or_union)
    {
    }

    CR_LogStruct(const CR_LogStruct& ls) :
        m_struct_or_union(ls.m_struct_or_union),
        m_type_list(ls.m_type_list),
        m_name_list(ls.m_name_list),
        m_bitfield(ls.m_bitfield),
        m_pack(ls.m_pack)
    {
    }

    void operator=(const CR_LogStruct& ls)
    {
        m_struct_or_union = ls.m_struct_or_union;
        m_type_list = ls.m_type_list;
        m_name_list = ls.m_name_list;
        m_bitfield = ls.m_bitfield;
        m_pack = ls.m_pack;
    }

    int FindName(const CR_String& name) const
    {
        for (size_t i = 0; i < m_name_list.size(); i++)
        {
            if (m_name_list[i] == name)
                return i;
        }
        return -1;
    }

    bool operator==(const CR_LogStruct& ls) const
    {
        return m_struct_or_union == ls.m_struct_or_union &&
               m_type_list == ls.m_type_list &&
               m_name_list == ls.m_name_list &&
               m_bitfield == ls.m_bitfield &&
               m_pack == ls.m_pack;
    }

    bool operator!=(const CR_LogStruct& ls) const
    {
        return m_struct_or_union != ls.m_struct_or_union ||
               m_type_list != ls.m_type_list ||
               m_name_list != ls.m_name_list ||
               m_bitfield != ls.m_bitfield ||
               m_pack != ls.m_pack;
    }
};

////////////////////////////////////////////////////////////////////////////
// CR_LogEnum

struct CR_LogEnum
{
    CR_LogEnum() { }

    CR_LogEnum(const CR_LogEnum& le) :
        m_mNameToValue(le.m_mNameToValue),
        m_mValueToName(le.m_mValueToName)
    {
    }

    void operator=(const CR_LogEnum& le)
    {
        m_mNameToValue = le.m_mNameToValue;
        m_mValueToName = le.m_mValueToName;
    }

    CR_UnorderedMap<CR_String, int>& MapNameToValue()
    { return m_mNameToValue; }

    CR_UnorderedMap<int, CR_String>& MapValueToName()
    { return m_mValueToName; }

    const CR_UnorderedMap<CR_String, int>& MapNameToValue() const
    { return m_mNameToValue; }

    const CR_UnorderedMap<int, CR_String>& MapValueToName() const
    { return m_mValueToName; }

protected:
    CR_UnorderedMap<CR_String, int>     m_mNameToValue;
    CR_UnorderedMap<int, CR_String>     m_mValueToName;
};

////////////////////////////////////////////////////////////////////////////
// CR_LogVar

struct CR_LogVar
{
    CR_LogVar() : m_has_value(false)
    {
    }

    bool            m_has_value;
    CR_TypeID       m_type_id;
    union
    {
        char        m_char_value;
        short       m_short_value;
        long        m_long_value;
        long long   m_long_long_value;
        int         m_int_value;
        float       m_float_value;
        double      m_double_value;
        long double m_long_double_value;
        void *      m_pointer_value;
    };
};

////////////////////////////////////////////////////////////////////////////
// CR_NameScope

class CR_NameScope
{
public:
    CR_NameScope() : m_is_64bit(false)
    {
        Init();
    }

    CR_NameScope(const CR_NameScope& ns) :
        m_is_64bit(ns.m_is_64bit),
        m_mNameToTypeID(ns.m_mNameToTypeID),
        m_mTypeIDToName(ns.m_mTypeIDToName),
        m_mNameToVarID(ns.m_mNameToVarID),
        m_mVarIDToName(ns.m_mVarIDToName),
        m_mNameToFuncTypeID(ns.m_mNameToFuncTypeID),
        m_types(ns.m_types),
        m_funcs(ns.m_funcs),
        m_structs(ns.m_structs),
        m_enums(ns.m_enums),
        m_vars(ns.m_vars)
    {
    }

    void operator=(const CR_NameScope& ns)
    {
        m_is_64bit = ns.m_is_64bit;
        m_mNameToTypeID = ns.m_mNameToTypeID;
        m_mTypeIDToName = ns.m_mTypeIDToName;
        m_mNameToVarID = ns.m_mNameToVarID;
        m_mVarIDToName = ns.m_mVarIDToName;
        m_mNameToFuncTypeID = ns.m_mNameToFuncTypeID;
        m_types = ns.m_types;
        m_funcs = ns.m_funcs;
        m_structs = ns.m_structs;
        m_enums = ns.m_enums;
        m_vars = ns.m_vars;
    }

    bool Is64Bit() const
    {
        return m_is_64bit;
    }

    void Set64Bit(bool is_64bit)
    {
        m_is_64bit = is_64bit;
    }

    void Init()
    {
        AddType("void", TF_VOID);

        AddType("char", TF_CHAR);
        AddType("short", TF_SHORT);
        AddType("long", TF_LONG);
        AddType("long long", TF_LONGLONG);
        AddType("int", TF_INT);

        AddType("unsigned char", TF_UNSIGNED | TF_CHAR);
        AddType("unsigned short", TF_UNSIGNED | TF_SHORT);
        AddType("unsigned long", TF_UNSIGNED | TF_LONG);
        AddType("unsigned long long", TF_UNSIGNED | TF_LONGLONG);
        AddType("unsigned int", TF_UNSIGNED | TF_INT);

        AddType("float", TF_FLOAT);
        AddType("double", TF_DOUBLE);
        AddType("long double", TF_LONG | TF_DOUBLE);

        AddType("va_list", TF_VA_LIST);

        // CodeReverse extension
        AddType("xsigned char", TF_XSIGNED | TF_CHAR);
        AddType("xsigned short", TF_XSIGNED | TF_SHORT);
        AddType("xsigned long", TF_XSIGNED | TF_LONG);
        AddType("xsigned long long", TF_XSIGNED | TF_LONGLONG);
        AddType("xsigned int", TF_XSIGNED | TF_INT);
    }

    CR_TypeID TypeIDFromName(const CR_String& name) const
    {
        auto it = m_mNameToTypeID.find(name);
        if (it != m_mNameToTypeID.end())
            return it->second;
        else
            return cr_invalid_id;
    }

    CR_String NameFromTypeID(CR_TypeID tid) const
    {
        auto it = m_mTypeIDToName.find(tid);
        if (it != m_mTypeIDToName.end())
            return it->second;
        else
            return "";
    }

    CR_TypeID AddType(const CR_String& name, const CR_LogType& lt)
    {
        CR_TypeID tid = m_types.Insert(lt);
        if (!name.empty())
        {
            m_mNameToTypeID[name] = tid;
            m_mTypeIDToName[tid] = name;
        }
        return tid;
    }

    CR_TypeID AddType(const CR_String& name, CR_TypeFlags flags)
    {
        return AddType(name, CR_LogType(flags));
    }

    CR_TypeID AddAliasType(const CR_String& name, CR_TypeID tid)
    {
        assert(!name.empty());
        CR_LogType lt;
        lt.m_flags = TF_ALIAS;
        lt.m_id = tid;
        m_types.push_back(lt);
        tid = static_cast<CR_TypeID>(m_types.size()) - 1;
        m_mNameToTypeID[name] = tid;
        m_mTypeIDToName[tid] = name;
        return tid;
    }

    CR_VarID AddVar(const CR_String& name, CR_TypeID tid)
    {
        assert(tid != cr_invalid_id);
        CR_LogVar var;
        var.m_type_id = tid;
        var.m_int_value = 0;
        m_vars.push_back(var);
        CR_VarID vid = static_cast<CR_VarID>(m_vars.size()) - 1;;
        if (!name.empty())
        {
            m_mNameToVarID[name] = vid;
            m_mVarIDToName[vid] = name;
        }
        return vid;
    }

    CR_VarID AddVar(const CR_String& name, const CR_LogType& lt)
    {
        CR_TypeID tid = m_types.Insert(lt);
        return AddVar(name, tid);
    }

    CR_TypeID AddConstType(CR_TypeID tid)
    {
        assert(tid != cr_invalid_id);
        CR_LogType lt;
        lt.m_flags = TF_CONST;
        lt.m_id = tid;
        CR_TypeID newtid = m_types.Insert(lt);
        CR_String name = NameFromTypeID(tid);
        if (!name.empty())
        {
            name = CR_String("const ") + name;
            m_mNameToTypeID[name] = newtid;
            m_mTypeIDToName[newtid] = name;
        }
        return newtid;
    }

    CR_TypeID AddPtrType(CR_TypeID tid, CR_TypeFlags flags = 0)
    {
        assert(tid != cr_invalid_id);
        CR_LogType lt;
        lt.m_flags = TF_POINTER | flags;
        lt.m_id = tid;
        CR_TypeID newtid = m_types.Insert(lt);
        CR_LogType type = m_types[tid];
        CR_String name = NameFromTypeID(tid);
        if (!name.empty() && !(type.m_flags & TF_FUNCTION))
        {
            if (flags & TF_CONST)
                name += "* const ";
            else
                name += "*";
            m_mNameToTypeID[name] = newtid;
            m_mTypeIDToName[newtid] = name;
        }
        return newtid;
    }

    CR_TypeID AddArrayType(CR_TypeID tid, int count)
    {
        assert(tid != cr_invalid_id);
        CR_LogType lt;
        lt.m_flags = TF_ARRAY;
        lt.m_id = tid;
        lt.m_count = count;
        tid = m_types.Insert(lt);
        return tid;
    }

    CR_TypeID AddFuncType(const CR_LogFunc& lf)
    {
        m_funcs.push_back(lf);
        CR_FuncID fid = static_cast<CR_FuncID>(m_funcs.size()) - 1;
        CR_LogType lt;
        lt.m_flags = TF_FUNCTION;
        lt.m_id = fid;
        CR_TypeID tid = m_types.Insert(lt);
        return tid;
    }

    CR_TypeID AddStructType(const CR_String& name, const CR_LogStruct& ls)
    {
        CR_LogType lt;
        if (name.empty())
        {
            m_structs.push_back(ls);
            CR_StructID sid = static_cast<CR_StructID>(m_structs.size()) - 1;
            lt.m_flags = (ls.m_struct_or_union ? TF_STRUCT : TF_UNION);
            lt.m_id = sid;
            CR_TypeID newtid = m_types.Insert(lt);
            return newtid;
        }
        auto it = m_mNameToTypeID.find(name);
        if (it == m_mNameToTypeID.end())
        {
            m_structs.push_back(ls);
            CR_StructID sid = static_cast<CR_StructID>(m_structs.size()) - 1;
            lt.m_flags = (ls.m_struct_or_union ? TF_STRUCT : TF_UNION);
            lt.m_id = sid;
            CR_TypeID newtid = m_types.Insert(lt);
            if (ls.m_struct_or_union)
            {
                CR_String newname(CR_String("struct ") + name);
                m_mNameToTypeID[newname] = newtid;
                m_mTypeIDToName[newtid] = newname;
            }
            else
            {
                CR_String newname(CR_String("union ") + name);
                m_mNameToTypeID[newname] = newtid;
                m_mTypeIDToName[newtid] = newname;
            }
            return newtid;
        }
        else
        {
            CR_TypeID tid = it->second;
            while (m_types[tid].m_flags & TF_ALIAS)
                tid = m_types[tid].m_id;
            assert(m_types[tid].m_flags & (TF_STRUCT | TF_UNION));
            CR_StructID sid = m_types[tid].m_id;
            if (ls.m_type_list.size())
                m_structs[sid] = ls;
            return tid;
        }
    }

    CR_TypeID AddEnumType(const CR_String& name, const CR_LogEnum& le)
    {
        CR_LogType lt;
        if (name.empty())
        {
            m_enums.push_back(le);
            CR_EnumID eid = static_cast<CR_EnumID>(m_enums.size()) - 1;
            lt.m_flags = TF_ENUM;
            lt.m_id = eid;
            CR_TypeID newtid = m_types.Insert(lt);
            return newtid;
        }
        auto it = m_mNameToTypeID.find(name);
        if (it == m_mNameToTypeID.end())
        {
            m_enums.push_back(le);
            CR_EnumID eid = static_cast<CR_EnumID>(m_enums.size()) - 1;
            lt.m_flags = TF_ENUM;
            lt.m_id = eid;
            CR_TypeID newtid = m_types.Insert(lt);
            CR_String newname(CR_String("enum ") + name);
            m_mNameToTypeID[newname] = newtid;
            m_mTypeIDToName[newtid] = newname;
            return newtid;
        }
        else
        {
            CR_TypeID tid = it->second;
            while (m_types[tid].m_flags & TF_ALIAS)
                tid = m_types[tid].m_id;
            assert(m_types[tid].m_flags & TF_ENUM);
            CR_EnumID eid = m_types[tid].m_id;
            m_enums[eid] = le;
            return tid;
        }
    }

    int GetSizeofStruct(CR_StructID sid) const
    {
        assert(sid != cr_invalid_id);
        if (sid == cr_invalid_id)
            return 0;

        const CR_LogStruct& ls = m_structs[sid];
        int size = 0, align = 0, bitremain = 0, oldtypesize = 0;
        CR_TypeID oldtid = cr_invalid_id;
        const std::size_t count = ls.m_type_list.size();
        for (std::size_t i = 0; i < count; i++)
        {
            auto tid = ls.m_type_list[i];
            int typesize = GetSizeofType(tid);
            int bits = ls.m_bitfield[i];
            if (bits)
            {
                // bitfield
                assert(bits <= typesize * 8);
                if ((oldtid == cr_invalid_id || tid == oldtid) && bitremain >= bits)
                {
                    bitremain -= bits;
                }
                else if (bitremain == 0)
                {
                    bitremain += typesize * 8;
                    bitremain -= bits;
                }
                else
                {
                    size += oldtypesize;
                    bitremain += oldtypesize * 8;
                    bitremain -= bits;
                }
            }
            else
            {
                if (bitremain)
                    size += oldtypesize;

                size += typesize;

                // consider struct packing...
                if (align && typesize >= ls.m_pack)
                {
                    size += ls.m_pack - (typesize + align) % ls.m_pack;
                    align = typesize % ls.m_pack;
                }
                else
                {
                    align += typesize;
                    align %= ls.m_pack;
                }

                bitremain = 0;
            }
            oldtid = tid;
            oldtypesize = typesize;
        }
        if (bitremain)
            size += oldtypesize;
        return size;
    }

    int GetSizeofUnion(CR_StructID sid) const
    {
        assert(sid != cr_invalid_id);
        if (sid == cr_invalid_id)
            return 0;

        const CR_LogStruct& ls = m_structs[sid];
        int maxsize = 0, size;
        for (auto tid : ls.m_type_list)
        {
            size = GetSizeofType(tid);
            if (maxsize < size)
                maxsize = size;
        }
        return maxsize;
    }

    int GetSizeofType(CR_TypeID tid) const
    {
        assert(tid != cr_invalid_id);
        if (tid == cr_invalid_id)
            return 0;
        auto& type = m_types[tid];
        if (type.m_flags & TF_ALIAS)
            return GetSizeofType(type.m_id);
        if ((type.m_flags & TF_POINTER) || (type.m_flags & TF_VA_LIST))
            return (Is64Bit() ? 8 : 4);
        if (type.m_flags & TF_ARRAY)
            return GetSizeofType(type.m_id) * type.m_count;
        if (type.m_flags & TF_CONST)
            return GetSizeofType(type.m_id);
        if (type.m_flags & TF_FUNCTION)
            return (Is64Bit() ? 8 : 4);
        if (type.m_flags & TF_STRUCT)
            return GetSizeofStruct(type.m_id);
        if (type.m_flags & TF_UNION)
            return GetSizeofUnion(type.m_id);
        if (type.m_flags & TF_ENUM)
            return 4;
        if (type.m_flags & TF_LONGLONG)
            return 8;
        if ((type.m_flags & TF_LONG) && !(type.m_flags & TF_DOUBLE))
            return 4;
        if ((type.m_flags & TF_LONG) && (type.m_flags & TF_DOUBLE))
            return 10;
        if (type.m_flags & TF_SHORT)
            return 2;
        if (type.m_flags & TF_CHAR)
            return 1;
        if (type.m_flags & TF_FLOAT)
            return 4;
        if (type.m_flags & TF_DOUBLE)
            return 8;
        return 0;
    }

    CR_String StringOfEnumType(const CR_String& name, CR_EnumID eid) const
    {
        assert(eid != cr_invalid_id);
        if (eid == cr_invalid_id)
        {
            return "";
        }
        CR_String str = "enum ";
        str += name;
        str += " ";
        if (!m_enums[eid].MapValueToName().empty())
        {
            str += "{ ";
            auto& e = m_enums[eid];
            for (auto it : e.MapValueToName())
            {
                str += it.second;
                str += " = ";
                char buf[32];
                std::sprintf(buf, "%d", it.first);
                str += buf;
                str += "; ";
            }
            str += "} ";
        }
        return str;
    }

    CR_String StringOfStructType(const CR_String& name, CR_StructID sid) const
    {
        assert(sid != cr_invalid_id);
        if (sid == cr_invalid_id)
        {
            return "";
        }
        CR_String str;
        if (m_structs[sid].m_struct_or_union)
            str = "struct ";
        else
            str = "union ";
        str += name;
        str += " ";
        auto& s = m_structs[sid];
        if (s.m_type_list.size())
        {
            str += "{ ";
            const std::size_t siz = s.m_type_list.size();
            for (std::size_t i = 0; i < siz; i++)
            {
                str += StringOfType(s.m_type_list[i], s.m_name_list[i]);
                if (s.m_bitfield[i])
                {
                    char buf[64];
                    sprintf(buf, " : %u", s.m_bitfield[i]);
                    str += buf;
                }
                str += "; ";
            }
            str += "} ";
        }
        return str;
    }

    CR_String StringOfType(CR_TypeID tid, const CR_String& content) const
    {
        assert(tid != cr_invalid_id);
        if (tid == cr_invalid_id)
        {
            return "";
        }
        const CR_LogType& type = m_types[tid];
        auto it = m_mTypeIDToName.find(tid);
        if (it != m_mTypeIDToName.end() &&
            ((type.m_flags & (TF_STRUCT | TF_UNION | TF_ENUM)) == 0))
        {
            return it->second + " " + content;
        }
        if (type.m_flags & TF_ENUM)
        {
            CR_String name = NameFromTypeID(tid);
            return StringOfEnumType(name, type.m_id) + content;
        }
        if (type.m_flags & (TF_STRUCT | TF_UNION))
        {
            CR_String name = NameFromTypeID(tid);
            return StringOfStructType(name, type.m_id) + content;
        }
        if (type.m_flags & TF_POINTER)
        {
            const CR_LogType& type2 = m_types[type.m_id];
            if ((type2.m_flags & TF_FUNCTION) || (type2.m_flags & TF_ARRAY) ||
                (type2.m_flags & TF_POINTER))
            {
                if (type2.m_flags & TF_FUNCTION)
                {
                    if (type.m_flags & TF_CDECL)
                        return StringOfType(type.m_id, "(__cdecl *" + content + ")");
                    if (type.m_flags & TF_STDCALL)
                        return StringOfType(type.m_id, "(__stdcall *" + content + ")");
                    if (type.m_flags & TF_FASTCALL)
                        return StringOfType(type.m_id, "(__fastcall *" + content + ")");
                }
                return StringOfType(type.m_id, "(*" + content + ")");
            }
            else if (type.m_flags & TF_CONST)
                return StringOfType(type.m_id, "") + " * const " + content;
            else
                return StringOfType(type.m_id, "") + " *" + content;
        }
        if (type.m_flags & TF_ARRAY)
        {
            if (type.m_count)
            {
                char buf[64];
                std::sprintf(buf, "[%d]", type.m_count);
                return StringOfType(type.m_id, content + buf);
            }
            else
                return StringOfType(type.m_id, content + "[]");
        }
        if (type.m_flags & TF_CONST)
        {
            return CR_String("const ") + StringOfType(type.m_id, content);
        }
        if (type.m_flags & TF_FUNCTION)
        {
            const CR_LogFunc& lf = m_funcs[type.m_id];
            CR_String rettype = StringOfType(lf.m_return_type, "");
            CR_String paramlist =
                StringOfParamList(lf.m_type_list, lf.m_name_list);
            CR_String convension;
            if (type.m_flags & TF_CDECL)
                convension = "__cdecl";
            if (type.m_flags & TF_STDCALL)
                convension = "__stdcall";
            if (type.m_flags & TF_FASTCALL)
                convension = "__fastcall";
            if (lf.m_ellipsis)
                paramlist += ", ...";
            return rettype + " " + convension + " " + content + "(" + paramlist + ")";
        }
        return "";
    }

    CR_String StringOfParamList(
        const CR_TypeSet& type_list,
        const CR_StringSet& name_list) const
    {
        assert(type_list.size() == name_list.size());
        std::size_t i, size = type_list.size();
        CR_String str;
        if (size > 0)
        {
            assert(type_list[0] != cr_invalid_id);
            str += StringOfType(type_list[0], name_list[0]);
            for (i = 1; i < size; i++)
            {
                str += ", ";
                assert(type_list[i] != cr_invalid_id);
                str += StringOfType(type_list[i], name_list[i]);
            }
        }
        return str;
    }

    int GetIntValueFromVarName(const CR_String& name) const
    {
        auto it = m_mNameToVarID.find(name);
        if (it == m_mNameToVarID.end())
            return 0;
        return m_vars[it->second].m_int_value;
    }

    bool IsFuncType(CR_TypeID tid) const
    {
        assert(tid != cr_invalid_id);
        if (tid == cr_invalid_id)
            return false;
        tid = ResolveAlias(tid);
        if (m_types[tid].m_flags & TF_FUNCTION)
            return true;
        return false;
    }

    bool IsIntegerType(CR_TypeID tid) const
    {
        assert(tid != cr_invalid_id);
        if (tid == cr_invalid_id)
            return false;
        tid = ResolveAlias(tid);
        const CR_TypeFlags not_flags =
            (TF_DOUBLE | TF_FLOAT | TF_POINTER | TF_ARRAY | TF_FUNCTION |
             TF_VA_LIST | TF_STRUCT | TF_UNION | TF_ENUM);
        if (m_types[tid].m_flags & not_flags)
            return false;
        const CR_TypeFlags flags =
            (TF_INT | TF_CHAR | TF_SHORT | TF_LONG | TF_LONGLONG);
        if (m_types[tid].m_flags & flags)
            return true;
        if (m_types[tid].m_flags & TF_CONST)
            return IsIntegerType(m_types[tid].m_id);
        return false;
    }

    bool IsFloatingType(CR_TypeID tid) const
    {
        assert(tid != cr_invalid_id);
        if (tid == cr_invalid_id)
            return false;
        tid = ResolveAlias(tid);
        if (m_types[tid].m_flags & (TF_DOUBLE | TF_FLOAT))
            return true;
        if (m_types[tid].m_flags & TF_CONST)
            return IsFloatingType(m_types[tid].m_id);
        return false;
    }

    bool IsUnsignedType(CR_TypeID tid) const
    {
        assert(tid != cr_invalid_id);
        if (tid == cr_invalid_id)
            return false;
        tid = ResolveAlias(tid);
        if (m_types[tid].m_flags & TF_UNSIGNED)
            return true;
        if (m_types[tid].m_flags & TF_CONST)
            return IsUnsignedType(m_types[tid].m_id);
        return false;
    }

    CR_TypeID ResolveAlias(CR_TypeID tid) const
    {
        if (tid == cr_invalid_id)
            return tid;
        return ResolveAlias_(tid);
    }

    CR_TypeID ResolveAlias_(CR_TypeID tid) const
    {
        assert(tid != cr_invalid_id);
        while (m_types[tid].m_flags & TF_ALIAS)
            tid = m_types[tid].m_id;
        return tid;
    }

    void AddTypeFlags(CR_TypeID tid, CR_TypeFlags flags)
    {
        m_types[tid].m_flags |= flags;
    }

    CR_LogType& LogType(CR_TypeID tid)
    {
        return m_types[tid];
    }

    const CR_LogType& LogType(CR_TypeID tid) const
    {
        return m_types[tid];
    }

    CR_LogVar& LogVar(CR_VarID vid)
    {
        return m_vars[vid];
    }

    const CR_LogVar& LogVar(CR_VarID vid) const
    {
        return m_vars[vid];
    }

    CR_Map<CR_String, CR_TypeID>& MapNameToTypeID()
    {
        return m_mNameToTypeID;
    }

    const CR_Map<CR_String, CR_TypeID>& MapNameToTypeID() const
    {
        return m_mNameToTypeID;
    }

    CR_Map<CR_TypeID, CR_String>& MapTypeIDToName()
    {
        return m_mTypeIDToName;
    }

    const CR_Map<CR_TypeID, CR_String>& MapTypeIDToName() const
    {
        return m_mTypeIDToName;
    }

    CR_Map<CR_String, CR_VarID>& MapNameToVarID()
    {
        return m_mNameToVarID;
    }

    const CR_Map<CR_String, CR_VarID>& MapNameToVarID() const
    {
        return m_mNameToVarID;
    }

    CR_Map<CR_VarID, CR_String>& MapVarIDToName()
    {
        return m_mVarIDToName;
    }

    const CR_Map<CR_VarID, CR_String>& MapVarIDToName() const
    {
        return m_mVarIDToName;
    }

    CR_DeqSet<CR_LogVar>& Vars()
    {
        return m_vars;
    }

    const CR_DeqSet<CR_LogVar>& Vars() const
    {
        return m_vars;
    }

    CR_LogStruct& LogStruct(CR_StructID sid)
    {
        assert(sid < m_structs.size());
        return m_structs[sid];
    }

    const CR_LogStruct& LogStruct(CR_StructID sid) const
    {
        assert(sid < m_structs.size());
        return m_structs[sid];
    }

protected:
    bool                            m_is_64bit;
    CR_Map<CR_String, CR_TypeID>    m_mNameToTypeID;
    CR_Map<CR_TypeID, CR_String>    m_mTypeIDToName;
    CR_Map<CR_String, CR_VarID>     m_mNameToVarID;
    CR_Map<CR_VarID, CR_String>     m_mVarIDToName;
    CR_Map<CR_String, CR_TypeID>    m_mNameToFuncTypeID;
    CR_DeqSet<CR_LogType>           m_types;
    CR_DeqSet<CR_LogFunc>           m_funcs;
    CR_DeqSet<CR_LogStruct>         m_structs;
    CR_DeqSet<CR_LogEnum>           m_enums;
    CR_DeqSet<CR_LogVar>            m_vars;
};

#endif  // ndef TYPESYSTEM_H_
