// Written by Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
// This file is public domain software.

#ifndef CSCANNER_HPP_
#define CSCANNER_HPP_

#include <cassert>  // assert
#include <cstdlib>  // std::strto*
#include <cstdio>   // EOF
#include <cstring>  // std::memmove

#include <string>   // std::string
#include <vector>   // std::vector
#include <set>      // std::set

#include "Location.hpp"      // cparser::Location
#include "CParserAST.hpp"    // cparser::Node, cparser::TokenInfo
#include "CParser.hpp"       // cparser::Parser

namespace cparser
{
    //
    // Scanner<Iterator, ParserSite>
    //
    template <class Iterator, class ParserSite>
    class Scanner
    {
    public:
        typedef ParserSite                  parser_site_type;
        typedef Iterator                    iterator_type;
        typedef TokenInfo<cparser::Token>   info_type;
        typedef shared_ptr<info_type>       shared_info_type;

    public:
        Scanner(parser_site_type& parser_site)
         : m_parser_site(parser_site), m_head_of_line(true), m_index(0)
        {
        }

        void scan(std::vector<info_type>& infos, Iterator begin, Iterator end)
        {
            m_begin = begin;
            m_current = begin;
            m_end = end;

            Token token;
            info_type info;
            do
            {
                token = get_token(info);
                info.set_token(token);
                infos.push_back(info);
            } while (token != cparser::eof);

            rescan1(infos);
            rescan2(infos.begin(), infos.end());
            rescan3(infos);
            rescan4(infos.begin(), infos.end());
            m_type_names.clear();
        }

        int get_pack() const
        {
            return (m_packs.empty() ? 1 : m_packs.back());
        }

        template <class TokenInfoIt>
        void show_tokens(TokenInfoIt begin, TokenInfoIt end)
        {
            for (TokenInfoIt it = begin; it != end; ++it)
            {
                std::cout << token_to_string(*it) << " ";
            }
        }

        std::string token_to_string(const info_type& info)
        {
            std::string str = token_label(info.m_token);

            switch (info.m_token)
            {
            case T_IDENTIFIER:
            case T_TYPEDEF_NAME:
            case T_TYPEDEF_TAG:
            case T_TAGNAME:
                str += "(";
                str += info.m_text;
                str += ")";
                break;

            default:
                break;
            }

            return str;
        }

        Token get_token(TokenValue& info)
        {
            using namespace std;
            using namespace cparser;
            char c, d, e;

            for (;;)
            {
                c = skip_space();   // open

                // skip comments
                if (c == '/')
                {
                    d = getch();
                    if (d == '/')
                    {
                        line_comment();
                        commit();
                        continue;   // closed
                    }
                    else if (d == '*')
                    {
                        block_comment();
                        commit();
                        continue;   // closed
                    }
                    ungetch();   // closed
                    continue;
                }

                // preprocessor directive
                if (c == '#' && m_head_of_line)
                {
                    c = skip_blank();   // open
                    ungetch();          // closed
                    if (lexeme("pragma"))   // #pragma
                    {
                        c = skip_blank();   // open
                        ungetch();           // closed
                        if (lexeme("message"))
                        {
                            c = skip_blank();   // open
                            if (c == '(')
                            {
                                c = skip_blank();   // open
                                if (c == '"')
                                {
                                    // #pragma message("...")
                                    std::string str;
                                    c = nonescaped_string_guts(str);   // open
                                    message(str);
                                    lib(str);
                                    ungetch();   // closed
                                }
                            }
                        }
                        else if (lexeme("pack"))
                        {
                            c = skip_blank();   // open
                            ungetch();           // closed
                            if (c == '(') // #pragma pack(...)
                            {
                                c = skip_blank();   // open
                                ungetch();           // closed
                                if (lexeme("push"))
                                {
                                    c = skip_blank();   // open
                                    ungetch();           // closed
                                    if (c == ',') // #pragma pack(push, ...)
                                    {
                                        c = skip_blank();   // open
                                        ungetch();           // closed
                                        if (lexeme("_CRT_PACKING"))
                                        {
                                            pack_push(1);
                                        }
                                        else if (isdigit(c))
                                        {
                                            char sz[32];
                                            int i = 0;
                                            sz[i++] = c;
                                            for (;;)
                                            {
                                                c = getch();
                                                if (!isdigit(c))
                                                    break;
                                                sz[i++] = c;
                                            }
                                            sz[i] = 0;
                                            int n = strtol(sz, NULL, 10);
                                            pack_push(n);
                                        }
                                    }
                                }
                                else if (lexeme("pop"))
                                {
                                    // #pragma pack(pop)
                                    pack_pop();
                                }
                            }
                        }
                        else if (lexeme("comment"))
                        {
                            c = skip_blank();   // open
                            if (c == '(') // #pragma comment(...)
                            {
                                c = skip_blank();   // open
                                ungetch();           // closed
                                if (lexeme("lib"))
                                {
                                    c = skip_blank();   // open
                                    if (c == ',')
                                    {
                                        c = skip_blank();   // open
                                        if (c == '"')
                                        {
                                            // #pragma comment(lib, "...")
                                            std::string str;
                                            c = nonescaped_string_guts(str);   // open
                                            lib(str);
                                            ungetch();   // closed
                                        }
                                    }
                                    else
                                        ungetch();   // closed
                                }
                                else if (lexeme("linker"))
                                {
                                    c = skip_blank();   // open
                                    if (c == ',')
                                    {
                                        c = skip_blank();   // open
                                        if (c == '"')
                                        {
                                            // #pragma comment(linker, "...")
                                            std::string str;
                                            c = nonescaped_string_guts(str);   // open
                                            linker(str);
                                            ungetch();   // closed
                                        }
                                    }
                                    else
                                        ungetch();   // closed
                                }
                            }
                            else
                                ungetch();   // closed
                        }
                    }
                    else if (lexeme("line") || isdigit(c))
                    {
                        if (!isdigit(c))
                            c = skip_blank();

                        // #line ...
                        // D+
                        if (isdigit(c))
                        {
                            std::string str2;
                            str2 += c;
                            for (;;)
                            {
                                c = getch();
                                if (!isdigit(c))
                                    break;
                                str2 += c;
                            }
                            if (!isdigit(c))
                                ungetch();   // closed

                            int lineno = std::atoi(str2.c_str());
                            c = skip_blank();   // open
                            if (c == '"')
                            {
                                // #line lineno "file"
                                std::string str;
                                c = nonescaped_string_guts(str);   // open
                                #if 1
                                    location().set(str.c_str(), lineno - 1);
                                #endif
                            }
                            else
                            {
                                // #line lineno
                                #if 1
                                    location().m_line = lineno - 1;
                                #endif
                            }   // open
                        }
                    }

                    // upto newline
                    while (c != EOF && c != '\n')
                        c = getch();

                    if (c == '\n')
                    {
                        newline();  // closed
                    }

                    commit();
                    continue;
                }

                info.m_text.clear();
                info.location() = location();
                info.m_pack = get_pack();
                info.m_flags = 0;

                // it is not the head of line
                m_head_of_line = false;

                // string or char literal
                if (c == 'L' || c == 'u' || c == 'U')
                {
                    d = getch(); // open
                    if (d == '"')
                    {
                        c = string_guts(info.m_text); // open
                        ungetch();   // closed
                        return commit_token(T_STRING);    // "..."
                    }
                    else if (d == '\'')
                    {
                        char ch;
                        c = char_guts(ch);  // open
                        info.m_text = c;
                        ungetch();   // closed
                        return commit_token(T_CONSTANT);  // '...'
                    }
                    else if (c == 'u' && d == '8')
                    {
                        e = getch(); // open
                        if (e == '"')
                        {
                            c = string_guts(info.m_text); // open
                            ungetch();   // closed
                            return commit_token(T_STRING);    // u8"..."
                        }
                        ungetch();   // closed
                    }
                    ungetch();   // closed
                }
                else if (c == '"')
                {
                    c = string_guts(info.m_text); // open
                    ungetch();   // closed
                    return commit_token(T_STRING);    // "..."
                }

                // constant
                {
                    std::string constant, str;
                    if (c == '0')
                    {
                        c = getch(); // open
                        if (c == 'x' || c == 'X')   // 0x
                        {
                            // H*
                            c = getch(); // open
                            if (isxdigit(c))
                            {
                                constant += c;
                                c = hex_guts(str); // open
                                constant += str;
                            }

                            // IS?
                            ungetch();   // closed
                            info.m_flags |= integer_suffix();

                            if (info.m_flags & TF_LONGLONG)
								info.m_long_long_value = strtoll(constant.c_str(), NULL, 16);
                            else
                                info.m_int_value = strtol(constant.c_str(), NULL, 16);

                            info.m_text = "0x";
                            info.m_text += constant;
                            return commit_token(T_CONSTANT);
                        }
                        else if (c == '.')
                        {
                            constant += c;

                            // D*
                            while (isdigit(c = getch()))    // open
                                constant += c;

                            // E?
                            if (c == 'E' || c == 'e')
                            {
                                constant += c;
                                c = getch(); // open
                                if (c == '+' || c == '-')
                                {
                                    constant += c;
                                    c = getch(); // open
                                }

                                if (isdigit(c))
                                {
                                    constant += c;
                                    for (;;)
                                    {
                                        c = getch(); // open
                                        if (!isdigit(c))
                                            break;
                                        constant += c;
                                    }
                                }
                            }

                            // FS?
                            if (c == 'f' || c == 'F')
                            {
                                c = getch(); // open
                                info.m_flags |= TF_FLOAT;
                                info.m_float_value = float(strtod(constant.c_str(), NULL));
                            }
                            else if (c == 'l' || c == 'L')
                            {
                                c = getch(); // open
                                info.m_flags |= TF_LONG | TF_DOUBLE;
                                info.m_long_double_value = strtold(constant.c_str(), NULL);
                            }
                            else
                            {
                                info.m_flags |= TF_DOUBLE;
                                info.m_long_double_value = strtod(constant.c_str(), NULL);
                            }

                            ungetch();   // closed
                            info.m_text = constant;
                            return commit_token(T_CONSTANT);
                        }
                        else    // octal
                        {
                            ungetch();   // closed
                            c = oct_guts(str); // open
                            constant += str;

                            // IS?
                            ungetch();   // closed
                            info.m_flags |= integer_suffix();   // closed

                            if (info.m_flags & TF_LONGLONG)
                                info.m_long_long_value = strtoll(constant.c_str(), NULL, 8);
                            else
                                info.m_int_value = strtol(constant.c_str(), NULL, 8);

                            return commit_token(T_CONSTANT);
                        }
                    }
                    else if (isdigit(c))
                    {
                        // D+
                        do
                        {
                            constant += c;
                            c = getch(); // open
                        } while (isdigit(c));

                        if (c == '.')
                        {
                            constant += c;

                            // D*
                            while (isdigit(c = getch()))
                                constant += c;    // open

                            // E?
                            if (c == 'E' || c == 'e')
                            {
                                constant += c;
                                c = getch(); // open
                                if (c == '+' || c == '-')
                                {
                                    constant += c;
                                    c = getch(); // open
                                }

                                if (isdigit(c))
                                {
                                    constant += c;
                                    for (;;)
                                    {
                                        c = getch(); // open
                                        if (!isdigit(c))
                                            break;
                                        constant += c;
                                    }
                                }
                            }

                            // FS?
                            if (c == 'f' || c == 'F')
                            {
                                info.m_flags |= TF_FLOAT;
                                info.m_float_value = float(strtod(constant.c_str(), NULL));
                                c = getch(); // open
                            }
                            else if (c == 'l' || c == 'L')
                            {
                                info.m_flags |= TF_LONG | TF_DOUBLE;
                                info.m_long_double_value = strtold(constant.c_str(), NULL);
                                c = getch(); // open
                            }
                            else
                            {
                                info.m_flags |= TF_DOUBLE;
                                info.m_double_value = strtod(constant.c_str(), NULL);
                            }

                            ungetch();   // closed
                            info.m_text = constant;
                            return commit_token(T_CONSTANT);
                        }
                        else
                        {
                            // IS?
                            ungetch();   // closed
                            info.m_flags |= integer_suffix();   // closed
                            info.m_int_value = strtol(constant.c_str(), NULL, 10);
                            info.m_text = constant;
                            return commit_token(T_CONSTANT);
                        }
                    }
                    else if (c == '.')
                    {
                        constant += c;
                        c = getch();
                        if (isdigit(c))    // open
                        {
                            constant += c;
                            // D+
                            for (;;)
                            {
                                c = getch(); // open
                                if (!isdigit(c))
                                    break;
                                constant += c;
                            }

                            // E?
                            if (c == 'E' || c == 'e')
                            {
                                constant += c;
                                c = getch(); // open
                                if (c == '+' || c == '-')
                                {
                                    constant += c;
                                    c = getch(); // open
                                }

                                for (;;)
                                {
                                    if (!isdigit(c))
                                        break;
                                    constant += c;
                                    c = getch(); // open
                                }
                            }

                            // FS?
                            if (c == 'f' || c == 'F')
                            {
                                c = getch(); // open
                                info.m_flags |= TF_FLOAT;
                                info.m_float_value = strtof(constant.c_str(), NULL);
                            }
                            else if (c == 'l' || c == 'L')
                            {
                                c = getch(); // open
                                info.m_flags |= TF_LONG | TF_DOUBLE;
                                info.m_long_double_value = strtold(constant.c_str(), NULL);
                            }
                            else
                            {
                                info.m_flags |= TF_DOUBLE;
                                info.m_double_value = strtod(constant.c_str(), NULL);
                            }

                            ungetch();   // closed
                            info.m_text = constant;
                            return commit_token(T_CONSTANT);
                        }
                        ungetch();
                        c = '.';
                    }
                }

                // identifier or keyword
                info.m_text.clear();
                if (isalpha(c) || c == '_') // open
                {
                    info.m_text += c;
                    for (;;)
                    {
                        d = getch(); // open
                        if (isalnum(d) || d == '_')
                            info.m_text += d;
                        else
                            break;
                    }
                    ungetch();   // closed

                    std::string& str(info.m_text);

                    if (str.find("__builtin_") == 0 && str.size() > 10)
                    {
                        str = str.substr(10);
                        c = str[0];
                    }

                    if (c == '_')
                    {
                        if (str.size() >= 2 && str[1] != '_')
                        {
                            d = str[1];
                            if (d == 'A' && str == "_Alignas") return commit_token(T_ALIGNAS);
                            else if (d == 'A' && str == "_Alignof") return commit_token(T_ALIGNOF);
                            else if (d == 'A' && str == "_Atomic") return commit_token(T_ATOMIC);
                            else if (d == 'B' && str == "_Bool") return commit_token(T_BOOL);
                            else if (d == 'C' && str == "_Complex") return commit_token(T_COMPLEX);
                            else if (d == 'G' && str == "_Generic") return commit_token(T_GENERIC);
                            else if (d == 'I' && str == "_Imaginary") return commit_token(T_IMAGINARY);
                            else if (d == 'N' && str == "_Noreturn") return commit_token(T_NORETURN);
                            else if (d == 'S' && str == "_Static_assert") return commit_token(T_STATIC_ASSERT);
                            else if (d == 'T' && str == "_Thread_local") return commit_token(T_THREAD_LOCAL);
                        }
                        else if (str.size() >= 3)
                        {
                            d = str[2];
                            if (d == 'a' && str == "__asm") return commit_token(T_ASM);
                            else if (d == 'a' && str == "__asm__") return commit_token(T_ASM);
                            else if (d == 'a' && str == "__attribute__") return commit_token(T_GNU_ATTRIBUTE);
                            else if (d == 'c' && str == "__cdecl") return commit_token(T_CDECL);
                            else if (d == 'c' && str == "__cdecl__") return commit_token(T_CDECL);
                            else if (d == 'c' && str == "__const__") return commit_token(T_CONST);
                            else if (d == 'd' && str == "__declspec") return commit_token(T_DECLSPEC);
                            else if (d == 'e' && str == "__extension__") return commit_token(T_GNU_EXTENSION);
                            else if (d == 'f' && str == "__fastcall") return commit_token(T_FASTCALL);
                            else if (d == 'f' && str == "__fastcall__") return commit_token(T_FASTCALL);
                            else if (d == 'f' && str == "__forceinline") return commit_token(T_FORCEINLINE);
                            else if (d == 'i' && str == "__inline") return commit_token(T_INLINE);
                            else if (d == 'i' && str == "__inline__") return commit_token(T_INLINE);
                            else if (d == 'i' && str == "__int32") return commit_token(T_INT32);
                            else if (d == 'i' && str == "__int64") return commit_token(T_INT64);
                            else if (d == 'n' && str == "__noreturn__") return commit_token(T_NORETURN);
                            else if (d == 'n' && str == "__nothrow__") return commit_token(T_NOTHROW);
                            else if (d == 'p' && str == "__pragma") return commit_token(T_PRAGMA);
                            else if (d == 'p' && str == "__ptr32") return commit_token(T_PTR32);
                            else if (d == 'p' && str == "__ptr64") return commit_token(T_PTR64);
                            else if (d == 'r' && str == "__restrict__") return commit_token(T_RESTRICT);
                            else if (d == 's' && str == "__signed__") return commit_token(T_SIGNED);
                            else if (d == 's' && str == "__stdcall") return commit_token(T_STDCALL);
                            else if (d == 's' && str == "__stdcall__") return commit_token(T_STDCALL);
                            else if (d == 'u' && str == "__unaligned") return commit_token(T_UNALIGNED);
                            else if (d == 'v' && str == "__volatile__") return commit_token(T_VOLATILE);
                            else if (d == 'w' && str == "__w64") return commit_token(T_W64);
                        }
                    }
                    else if (c < 'd')
                    {
                        if (c == 'a' && str == "asm") return commit_token(T_ASM);
                        else if (c == 'a' && str == "auto") return commit_token(T_AUTO);
                        else if (c == 'b' && str == "break") return commit_token(T_BREAK);
                        else if (c == 'c' && str == "case") return commit_token(T_CASE);
                        else if (c == 'c' && str == "char") return commit_token(T_CHAR);
                        else if (c == 'c' && str == "const") return commit_token(T_CONST);
                        else if (c == 'c' && str == "continue") return commit_token(T_CONTINUE);
                    }
                    else if (c < 'f')
                    {
                        if (c == 'd' && str == "default") return commit_token(T_DEFAULT);
                        else if (c == 'd' && str == "do") return commit_token(T_DO);
                        else if (c == 'd' && str == "double") return commit_token(T_DOUBLE);
                        else if (c == 'e' && str == "else") return commit_token(T_ELSE);
                        else if (c == 'e' && str == "enum") return commit_token(T_ENUM);
                        else if (c == 'e' && str == "extern") return commit_token(T_EXTERN);
                    }
                    else if (c < 's')
                    {
                        if (c == 'f' && str == "float") return commit_token(T_FLOAT);
                        else if (c == 'f' && str == "for") return commit_token(T_FOR);
                        else if (c == 'g' && str == "goto") return commit_token(T_GOTO);
                        else if (c == 'i' && str == "if") return commit_token(T_IF);
                        else if (c == 'i' && str == "inline") return commit_token(T_INLINE);
                        else if (c == 'i' && str == "int") return commit_token(T_INT);
                        else if (c == 'l' && str == "long") return commit_token(T_LONG);
                        else if (c == 'n' && str == "noreturn") return commit_token(T_NORETURN);
                        else if (c == 'r' && str == "register") return commit_token(T_REGISTER);
                        else if (c == 'r' && str == "restrict") return commit_token(T_RESTRICT);
                        else if (c == 'r' && str == "return") return commit_token(T_RETURN);
                    }
                    else
                    {
                        if (c == 's' && str == "short") return commit_token(T_SHORT);
                        else if (c == 's' && str == "signed") return commit_token(T_SIGNED);
                        else if (c == 's' && str == "sizeof") return commit_token(T_SIZEOF);
                        else if (c == 's' && str == "static") return commit_token(T_STATIC);
                        else if (c == 's' && str == "struct") return commit_token(T_STRUCT);
                        else if (c == 's' && str == "switch") return commit_token(T_SWITCH);
                        else if (c == 't' && str == "typedef") return commit_token(T_TYPEDEF);
                        else if (c == 'u' && str == "union") return commit_token(T_UNION);
                        else if (c == 'u' && str == "unsigned") return commit_token(T_UNSIGNED);
                        else if (c == 'v' && str == "void") return commit_token(T_VOID);
                        else if (c == 'v' && str == "volatile") return commit_token(T_VOLATILE);
                        else if (c == 'w' && str == "while") return commit_token(T_WHILE);
                    }
                    return commit_token(T_IDENTIFIER);
                }

                // operators and symbols
                switch (c)  // open
                {
                case '>':
                    d = getch(); // open
                    if (d == '>')
                    {
                        e = getch(); // open
                        if (e == '=')   // closed
                            return commit_token(T_R_SHIFT_ASSIGN);
                        else
                        {
                            ungetch();   // closed
                            return commit_token(T_R_SHIFT);
                        }
                    }
                    else if (d == '=')  // closed
                        return commit_token(T_GE);
                    else if (d == ':')  // closed
                        return commit_token(T_L_BRACKET);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_GT);
                    }

                case '<':
                    d = getch(); // open
                    if (d == '<')
                    {
                        e = getch(); // open
                        if (e == '=')
                            return commit_token(T_L_SHIFT_ASSIGN);
                        else
                        {
                            ungetch();   // closed
                            return commit_token(T_L_SHIFT);
                        }
                    }
                    else if (d == '=')  // closed
                        return commit_token(T_LE);
                    else if (d == '%')  // closed
                        return commit_token(T_L_BRACE);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_LT);
                    }

                case '+':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_ADD_ASSIGN);
                    else if (d == '+')  // closed
                        return commit_token(T_INC);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_PLUS);
                    }

                case '-':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_SUB_ASSIGN);
                    else if (d == '-')  // closed
                        return commit_token(T_DEC);
                    else if (d == '>')  // closed
                        return commit_token(T_ARROW);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_MINUS);
                    }

                case '*':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_MUL_ASSIGN);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_ASTERISK);
                    }

                case '/':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_DIV_ASSIGN);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_SLASH);
                    }

                case '%':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_MOD_ASSIGN);
                    else if (d == '>')  // closed
                        return commit_token(T_R_BRACE);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_PERCENT);
                    }

                case '&':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_AND_ASSIGN);
                    else if (d == '&')  // closed
                        return commit_token(T_AND_ASSIGN);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_AND);
                    }
                case '^':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_XOR_ASSIGN);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_XOR);
                    }

                case '|':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_OR_ASSIGN);
                    else if (d == '|')  // closed
                        return commit_token(T_L_OR);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_OR);
                    }

                case '=':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_EQUAL);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_ASSIGN);
                    }

                case '!':
                    d = getch(); // open
                    if (d == '=')   // closed
                        return commit_token(T_NE);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_NOT);
                    }

                case ';':   // closed
                    return commit_token(T_SEMICOLON);

                case ':':
                    d = getch(); // open
                    if (d == '>')   // closed
                        return commit_token(T_R_BRACKET);
                    else
                    {
                        ungetch();   // closed
                        return commit_token(T_COLON);
                    }

                case '.':
                    d = getch(); // open
                    if (d == '.')
                    {
                        e = getch(); // open
                        if (e == '.')   // closed
                            return commit_token(T_ELLIPSIS);
                        ungetch();
                    }
                    ungetch();
                    return commit_token(T_DOT);

                case ',':   // closed
                    return commit_token(T_COMMA);

                case '{':   // closed
                    return commit_token(T_L_BRACE);

                case '}':   // closed
                    return commit_token(T_R_BRACE);

                case '(': return commit_token(T_L_PAREN);     // closed
                case ')': return commit_token(T_R_PAREN);     // closed
                case '[': return commit_token(T_L_BRACKET);   // closed
                case ']': return commit_token(T_R_BRACKET);   // closed
                case '~': return commit_token(T_BITWISE_NOT); // closed
                case '?': return commit_token(T_QUESTION);    // closed
                case EOF: return commit_token(eof);     // closed
                }

                // open
                m_parser_site.unexpected_character(c);
                break;
            }
            return eof;
        }   // get_token

    protected:
        char getch()
        {
            char c;

            if (m_buff.size() == m_index)
            {
                if (m_current == m_end)
                    c = EOF;
                else
                {
                    c = *m_current++;
                    m_buff.push_back(c);
                    m_index++;
                }
            }
            else
            {
                c = m_buff[m_index++];
            }

            return c;
        }   // getch

        void ungetch()
        {
            assert(m_index);
            if (0 < m_index)
                m_index--;
        }

        void commit()
        {
            using namespace std;
            if (m_index)
            {
                #if 1
                    m_buff.erase(m_buff.begin(), m_buff.begin() + m_index);
                #else
                    const size_t size = m_buff.size();
                    const size_t newsize = size - m_index;
                    assert(m_index <= size);

                    if (m_index < size)
                        memmove(&m_buff[0], &m_buff[m_index], newsize);
                    m_buff.resize(newsize);
                #endif
                m_index = 0;
            }
        }

        Token commit_token(Token token)
        {
            commit();
            return token;
        }

        bool lexeme(const char* s)
        {
            char c;
            std::size_t n = 1;
            while (*s)
            {
                c = getch();
                if (c == *s)
                {
                    s++; n++;
                }
                else
                {
                    assert(n <= m_index);
                    m_index -= n;
                    return false;   // closed
                }
            }
            if (*s == 0)
            {
                commit();
                return true;    // closed
            }
            return false;   // closed
        }   // lexeme

        void newline()
        {
            ++location();
            m_head_of_line = true;
        }

        char skip_blank()
        {
            char c;
            do
            {
                c = getch();
            } while (c == ' ' || c == '\t');
            return c;   // open
        }

        char skip_space()
        {
            using namespace std;
            char c;
            do
            {
                c = getch();
                if (c == '\n')
                    newline();
            } while (isspace(c));
            return c;   // open
        }

        void line_comment()
        {
            char c;
            do
            {
                c = getch();
                if (c == '\n')
                {
                    newline();
                    break;  // closed
                }
            } while (c != EOF);
        }

        void block_comment()
        {
            char c;
            do
            {
                c = getch();
                if (c == '\n')
                    newline();
                else if (c == '*')
                {
                    c = getch();
                    if (c == '/')
                        break;  // closed
                    ungetch();
                }
            } while (c != EOF);
        }

        bool escape_sequence(char& ch)
        {
            using namespace std;
            char c, sz[10];
            switch (c = getch()) // open
            {
            case 'b': ch = '\b'; return true;
            case 't': ch = '\t'; return true;
            case 'n': ch = '\n'; return true;
            case 'f': ch = '\f'; return true;
            case 'r': ch = '\r'; return true;
            case '\"': ch = '\"'; return true;
            case '\'': ch = '\''; return true;
            case '\\': ch = '\\'; return true;
            case 'x':
                if (isxdigit(c = getch()))  // open
                {
                    int i = 0;
                    sz[i++] = c;
                    if (isxdigit(c = getch())) // open
                        sz[i++] = c;
                    sz[i] = 0;
                    ch = static_cast<char>(strtol(sz, NULL, 16));
                    if (!isxdigit(c))
                        ungetch();   // closed
                    return true;    // closed
                }
                ch = '?';
                break;

            default:
                if ('0' <= c && c <= '7')
                {
                    int i = 0;
                    sz[i++] = c;
                    c = getch(); // open
                    if ('0' <= c && c <= '7')
                    {
                        sz[i++] = c;
                        c = getch(); // open
                        if ('0' <= c && c <= '7')
                        {
                            sz[i++] = c;
                            getch(); // open
                            c = 0;
                        }
                    }
                    sz[i] = 0;
                    ch = static_cast<char>(strtol(sz, NULL, 8));
                    if (!('0' <= c && c <= '7'))
                        ungetch();   // closed
                    return true;
                }
                ch = c;
            }
            return false;   // invalid
        }   // escape_sequence

        char string_guts(std::string& s)
        {
            char c;
            s.clear();
            do
            {
                if ((c = getch()) == '\\')   // open
                {
                    if (escape_sequence(c))
                    {
                        s += c;
                    }
                    else
                    {
                        m_parser_site.unsupported_escape_sequence();
                    }
                }
                else if (c == '\"')
                {
                    c = getch(); // open
                    break;
                }
                else if (c != EOF)
                    s += c;
                else
                {
                    m_parser_site.unexpected_eof();
                    break;  // open
                }
            } while (c != EOF);

            return c;   // open
        }   // string_guts

        char nonescaped_string_guts(std::string& s)
        {
            char c, d;
            s.clear();
            do
            {
                c = getch();
                if (c == '\\')
                {
                    d = getch(); // open
                    if (d != '\\')
                    {
                        s += c;
                        s += d;
                    }
                    else
                        s += c;
                }
                else if (c == '\"')
                {
                    c = getch(); // open
                    break;
                }
                else if (c != EOF)
                {
                    s += c;
                }
                else
                {
                    m_parser_site.unexpected_eof();
                    break;  // open
                }
            } while (c != EOF);

            return c;   // open
        }   // nonescaped_string_guts

        char char_guts(char& ch)
        {
            char c = getch();

            do
            {
                if (c == '\\')
                {
                    if (!escape_sequence(ch))
                    {
                        m_parser_site.unsupported_escape_sequence();
                    }
                }
                else if (c != EOF)
                {
                    ch = c;
                    c = getch();
                }
                else
                {
                    m_parser_site.unexpected_eof();
                    return c;
                }

                c = getch(); // open

                if (c != '\'')
                    m_parser_site.not_supported_yet("multicharacter character literal");
            } while (c != '\'');

            return c;
        }   // char_guts

        char hex_guts(std::string& str)
        {
            using namespace std;
            char c;
            for (;;)
            {
                c = getch();
                if (!isxdigit(c))
                    break;
                str += c;
            }
            return c;
        }

        char oct_guts(std::string& str)
        {
            char c;
            for (;;)
            {
                c = getch();
                if (!('0' <= c && c <= '7'))
                    break;

                str += c;
            }
            return c;
        }

        int integer_suffix()
        {
            char c, d, e;
            c = getch(); // open
            if (c == 'u' || c == 'U')
            {
                d = getch(); // open
                if (d == 'l' || d == 'L')
                {
                    e = getch(); // open
                    if (d == e)
                        return TF_LONGLONG | TF_UNSIGNED; // ULL, closed

                    ungetch();
                    return TF_LONG | TF_UNSIGNED; // UL, closed
                }
                ungetch();
                return TF_UNSIGNED; // U, closed
            }
            else if (c == 'l' || c == 'L')
            {
                d = getch(); // open
                if (c == d)
                {
                    e = getch(); // open
                    if (e == 'u' || e == 'U')
                        return TF_LONGLONG | TF_UNSIGNED; // LLU, closed

                    ungetch();
                    return TF_LONGLONG; // LL, closed
                }
                ungetch();
                return TF_LONG; // L, closed
            }
            ungetch();   // closed
			return 0;
        }   // integer_suffix

        //
        // packing
        //
        void pack_push(int pack)
        {
            m_packs.push_back(pack);
        }

        void pack_pop()
        {
            assert(m_packs.size());
            m_packs.pop_back();
        }

        //
        // pragma
        //
        void lib(const std::string str) { }
        void linker(const std::string str) { }

        //
        // rescan
        //
        template <class TokenInfoContainer>
        void rescan1(TokenInfoContainer& c)
        {
            TokenInfoContainer newc;
            typename TokenInfoContainer::iterator it, it2;
            const typename TokenInfoContainer::iterator end = c.end();
            for (it = c.begin(); it != end; ++it)
            {
                if (it->m_token == T_GNU_ATTRIBUTE)
                {
                    it2 = it;
                    it = skip_gnu_attribute(it, end);
                    if (it != end)
                    {
                        ++it2;  // T_GNU_ATTRIBUTE
                        ++it2;  // T_L_PAREN
                        ++it2;  // T_L_PAREN
                        switch (it2->m_token)
                        {
                        case T_CDECL: case T_STDCALL: case T_FASTCALL:
                            newc.push_back(*it2);
                            break;

                        default:
                            break;
                        }
                    }
                }
                else if (it->m_token == T_DECLSPEC || it->m_token == T_PRAGMA)
                {
                    it2 = it;
                    ++it2;
                    bool f = false;
                    int paren_nest = 0;
                    for (; it2 != end; ++it2)
                    {
                        if (it2->m_token == T_L_PAREN)
                        {
                            f = true;
                            paren_nest++;
                        }
                        else if (it2->m_token == T_R_PAREN)
                        {
                            paren_nest--;
                            if (paren_nest == 0)
                                break;
                        }
                    }
                    if (f)
                        it = it2;
                }
                else
                {
                    newc.push_back(*it);
                }
            }
            c = newc;
        }

        template <class TokenInfoIt>
        void rescan2(TokenInfoIt begin, TokenInfoIt end)
        {
            m_type_names.clear();
            m_type_names.insert("va_list");
            for (TokenInfoIt it = begin; it != end; ++it)
            {
                if (it->m_token == T_ENUM ||
                    it->m_token == T_STRUCT ||
                    it->m_token == T_UNION)
                {
                    ++it;
                    if (it->m_token == T_IDENTIFIER)
                    {
                        it->set_token(T_TAGNAME);
                    }
                }
            }

            for (TokenInfoIt it = begin; it != end; ++it)
            {
                if (it->m_token == T_TYPEDEF)
                {
                    it = rescan_typedef(++it, end);
                }
                else if (it->m_token == T_IDENTIFIER)
                {
                    if (m_type_names.count(it->m_text))
                    {
                        it->set_token(T_TYPEDEF_NAME);
                    }
                }
            }
        }

        template <class TokenInfoIt>
        TokenInfoIt rescan_typedef(TokenInfoIt begin, TokenInfoIt end)
        {
            int paren_nest = 0, brace_nest = 0, bracket_nest = 0;
            TokenInfoIt it;
            for (it = begin; it != end; ++it)
            {
                if (brace_nest == 0 && it->m_token == T_SEMICOLON)
                    break;
                else if (it->m_token == T_L_BRACE)
                    brace_nest++;
                else if (it->m_token == T_R_BRACE)
                    brace_nest--;
                else if (it->m_token == T_L_BRACKET)
                    bracket_nest++;
                else if (it->m_token == T_R_BRACKET)
                    bracket_nest--;
                else if (it->m_token == T_L_PAREN)
                    paren_nest++;
                else if (it->m_token == T_R_PAREN)
                {
                    paren_nest--;
                    ++it;
                    if (it->m_token == T_L_PAREN)
                    {
                        it = rescan_parameter_list(++it, end);
                    }
                    else
                        --it;
                }
                else if (it->m_token == T_IDENTIFIER)
                {
                    if (brace_nest == 0 && bracket_nest == 0)
                    {
                        if (m_type_names.count(it->m_text))
                        {
                            ++it;
                            if (it->m_token == T_SEMICOLON || it->m_token == T_R_PAREN ||
                                it->m_token == T_L_BRACKET || it->m_token == T_COMMA)
                            {
                                --it;
                                it->set_token(T_TYPEDEF_TAG);
                            }
                            else
                            {
                                --it;
                                it->set_token(T_TYPEDEF_NAME);
                            }
                        }
                        else
                        {
                            it->set_token(T_TYPEDEF_TAG);
                            m_type_names.insert(it->m_text);

                            ++it;
                            if (it->m_token == T_L_PAREN)
                            {
                                it = rescan_parameter_list(++it, end);
                            }
                            else
                                --it;
                        }
                    }
                    else if (m_type_names.count(it->m_text))
                    {
                        it->set_token(T_TYPEDEF_NAME);
                    }
                }
            }
            return it;
        }

        template <class TokenInfoIt>
        TokenInfoIt rescan_parameter_list(TokenInfoIt begin, TokenInfoIt end)
        {
            int paren_nest = 1;
            bool fresh = true;
            TokenInfoIt it;
            for (it = begin; it != end; ++it)
            {
                if (it->m_token == T_SEMICOLON)
                    break;
                else if (it->m_token == T_L_PAREN)
                {
                    paren_nest++;
                    fresh = true;
                }
                else if (it->m_token == T_R_PAREN)
                {
                    paren_nest--;
                    if (paren_nest == 0)
                        break;

                    ++it;
                    if (it->m_token == T_L_PAREN)
                    {
                        it = rescan_parameter_list(++it, end);
                    }
                    else
                        --it;
                }
                else if (it->m_token == T_IDENTIFIER)
                {
                    if (m_type_names.count(it->m_text))
                    {
                        ++it;
                        if (fresh)
                        {
                            --it;
                            it->set_token(T_TYPEDEF_NAME);
                        }
                        else
                        {
                            --it;
                        }
                    }
                    fresh = false;
                }
                else if (it->m_token == T_COMMA)
                {
                    fresh = true;
                }
            }
            return it;
        }

        template <class TokenInfoIt>
        TokenInfoIt skip_gnu_attribute(TokenInfoIt begin, TokenInfoIt end)
        {
            TokenInfoIt it = begin;
            if (it != end && it->m_token == T_GNU_ATTRIBUTE)
                ++it;

            if (it != end && it->m_token == T_L_PAREN)
            {
                ++it;
                int paren_nest = 1;
                for (; it != end; ++it)
                {
                    if (it->m_token == T_L_PAREN)
                    {
                        paren_nest++;
                    }
                    else if (it->m_token == T_R_PAREN)
                    {
                        paren_nest--;
                        if (paren_nest == 0)
                            break;
                    }
                }
            }
            return it;
        }

        template <class TokenInfoContainer>
        void rescan3(TokenInfoContainer& c)
        {
            TokenInfoContainer newc;
            typename TokenInfoContainer::iterator it, it2;
            const typename TokenInfoContainer::iterator end = c.end();
            for (it = c.begin(); it != end; ++it)
            {
                switch (it->m_token)
                {
                case T_CDECL: case T_STDCALL: case T_FASTCALL:
                    // TODO: do calling convention
                    if ((it + 1)->m_token == T_ASTERISK)
                    {
                        if (it->m_token == T_CDECL)
                            (it + 1)->m_flags |= TF_CDECL;
                        else if (it->m_token == T_STDCALL)
                            (it + 1)->m_flags |= TF_STDCALL;
                        else if (it->m_token == T_FASTCALL)
                            (it + 1)->m_flags |= TF_FASTCALL;
                    }
                    else if ((it + 1)->m_token == T_IDENTIFIER ||
                             (it + 1)->m_token == T_TYPEDEF_NAME)
                    {
                        if (it->m_token == T_CDECL)
                            (it + 1)->m_flags |= TF_CDECL;
                        else if (it->m_token == T_STDCALL)
                            (it + 1)->m_flags |= TF_STDCALL;
                        else if (it->m_token == T_FASTCALL)
                            (it + 1)->m_flags |= TF_FASTCALL;
                    }
                    break;

                case T_L_BRACKET:
                    it2 = it;
                    ++it2;
                    if (it2->m_token == T_IDENTIFIER ||
                        it2->m_token == T_TYPEDEF_NAME)
                    {
                        bool f =
                            (it2->m_text == "returnvalue") ||
                            (it2->m_text == "SA_Pre") ||
                            (it2->m_text == "SA_Post") ||
                            (it2->m_text == "SA_FormatString") ||
                            (it2->m_text == "source_annotation_attribute");
                        if (f)
                        {
                            int paren_nest = 0;
                            f = false;
                            for (; it2 != end; ++it2)
                            {
                                if (it2->m_token == T_L_PAREN)
                                {
                                    f = true;
                                    paren_nest++;
                                }
                                else if (it2->m_token == T_R_PAREN)
                                {
                                    paren_nest--;
                                    if (paren_nest == 0)
                                        break;
                                }
                            }
                            if (f)
                            {
                                f = false;
                                if (it2 != end)
                                {
                                    ++it2;
                                    if (it2->m_token == T_R_BRACKET)
                                    {
                                        it = it2;
                                        f = true;
                                    }
                                }
                            }
                        }
                        if (!f)
                            newc.push_back(*it);
                    }
                    else
                    {
                        newc.push_back(*it);
                    }
                    break;

                case T_GNU_EXTENSION:
                    break;

                default:
                    newc.push_back(*it);
                }
            }
            c = newc;
        }

        template <class TokenInfoIt>
        void rescan4(TokenInfoIt begin, TokenInfoIt end)
        {
            TokenInfoIt it, paren_it, it2;
            int paren_nest = 0;
            for (it = begin; it != end; ++it)
            {
                if (it->m_token == T_L_PAREN)
                {
                    paren_it = it;
                    paren_nest++;
                }
                else if (it->m_token == T_R_PAREN)
                {
                    paren_nest--;
                }
                else if (paren_nest >= 1 &&
                    it->m_token == T_TYPEDEF_NAME &&
                    ((it + 1)->m_token == T_R_PAREN || (it + 1)->m_token == T_COMMA))
                {
                    it2 = it - 1;
                    while (it2 != paren_it)
                    {
                        switch (it2->m_token)
                        {
                        case T_VOID: case T_CHAR: case T_SHORT: case T_INT:
                        case T_INT32: case T_INT64: case T_LONG:
                        case T_FLOAT: case T_DOUBLE:
                        case T_SIGNED: case T_UNSIGNED: case T_BOOL:
                        case T_TYPEDEF_NAME: case T_TAGNAME:
                        case T_ASTERISK:
                            it->m_token = T_IDENTIFIER;
                            break;

                        case T_CONST:
                            --it2;
                            continue;

                        default:
                            break;
                        }
                        break;
                    }
                }
            }
        }

    protected:
        parser_site_type&       m_parser_site;
        bool                    m_head_of_line;
        std::size_t             m_index;
        std::vector<char>       m_buff;
        std::vector<int>        m_packs;
        std::set<std::string>   m_type_names;

        iterator_type           m_begin;
        iterator_type           m_current;
        iterator_type           m_end;

        Location& location()
        {
            return m_parser_site.location();
        }

        const Location& location() const
        {
            return m_parser_site.location();
        }

        void message(const std::string& str)
        {
            m_parser_site.message(str);
        }
    }; // class Scanner<Iterator, ParserSite>

    template <class Iterator>
    bool parse(Iterator begin, Iterator end);
    bool parse_string(const char *s);
    bool parse_string(const std::string& str);
    bool parse_file(const char *filename);
} // namespace cparser

#endif  // ndef CSCANNER_HPP_
