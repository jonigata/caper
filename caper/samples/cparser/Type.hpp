// Written by Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
// This file is public domain software.

#ifndef TYPE_HPP_
#define TYPE_HPP_

#include <string>   // for std::string
#include <deque>    // for std::deque
#include <cassert>  // for assert
#include <vector>   // for std::vector

namespace cparser
{
    enum TypeFlags
    {
        TF_ZERO         = (1 << 0),
        TF_VOID         = (1 << 1),
        TF_CHAR         = (1 << 2),
        TF_SHORT        = (1 << 3),
        TF_INT          = (1 << 4),
        TF_LONG         = (1 << 5),
        TF_LONGLONG     = (1 << 6),
        TF_FLOAT        = (1 << 7),
        TF_DOUBLE       = (1 << 8),
        TF_SIGNED       = (1 << 9),
        TF_UNSIGNED     = (1 << 10),
        TF_ALIAS        = (1 << 11),
        TF_STRUCT       = (1 << 12),
        TF_UNION        = (1 << 13),
        TF_ENUM         = (1 << 14),
        TF_POINTER      = (1 << 15),
        TF_ARRAY        = (1 << 16),
        TF_FUNCTION     = (1 << 17),
        TF_CDECL        = (1 << 18),
        TF_STDCALL      = (1 << 19),
        TF_FASTCALL     = (1 << 20),
        TF_CONST        = (1 << 21),
        TF_VOLATILE     = (1 << 22),
        TF_COMPLEX      = (1 << 23),
        TF_IMAGINARY    = (1 << 24),
        TF_ATOMIC       = (1 << 25),
        TF_EXTERN       = (1 << 26),
        TF_STATIC       = (1 << 27),
        TF_THREADLOCAL  = (1 << 28),
        TF_INLINE       = (1 << 29),
        TF_FORCEINLINE  = (1 << 30),
        TF_NORETURN     = (1 << 31)
    };
    typedef unsigned long TYPEFLAGS;

    //
    // TypeCell
    //
    struct TypeCell
    {
        TYPEFLAGS                            m_flags;
        int                                  m_param;
        std::string                          m_name;

        // parameters for function; members for structure
        std::vector<std::vector<TypeCell> >  m_children;

        TypeCell()
        : m_flags(0), m_param(0)
        {
        }

        TypeCell(TYPEFLAGS flags)
        : m_flags(flags), m_param(0)
        {
        }

        TypeCell(TYPEFLAGS flags, const std::string& name)
        : m_flags(flags), m_param(0), m_name(name)
        {
        }

        TypeCell(const TypeCell& tc)
        : m_flags(tc.m_flags),
          m_param(tc.m_param),
          m_name(tc.m_name),
          m_children(tc.m_children)
        {
        }

        void operator=(const TypeCell& tc)
        {
            m_flags = tc.m_flags;
            m_param = tc.m_param;
            m_name = tc.m_name;
            m_children = tc.m_children;
        }
    };

    //
    // TypeExpr
    //
	typedef std::vector<TypeCell> TypeExpr;
} // namespace cparser

#endif  // ndef TYPE_HPP_
