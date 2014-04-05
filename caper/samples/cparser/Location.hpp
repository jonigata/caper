// Written by Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
// This file is public domain software.

#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <string>       // std::string
#include <iostream>     // std::basic_ostream

namespace cparser
{
    //
    // Location
    //
    struct Location
    {
        std::string m_file;
        int m_line;

        Location() : m_line(1) { }

        Location(const Location& loc)
        : m_file(loc.m_file), m_line(loc.m_line)
        {
        }

        Location(const char *file, int line) : m_file(file), m_line(line) { }

        Location(const std::string& file, int line) : m_file(file), m_line(line)
        {
        }

        void set(const char *file, int line)
        {
            m_file = file;
            m_line = line;
        }

        Location& operator=(const Location& loc)
        {
            m_file = loc.m_file;
            m_line = loc.m_line;
            return *this;
        }

        Location& operator++()
        {
            m_line++;
            return *this;
        }

        Location operator++(int)
        {
            Location loc(*this);
            m_line++;
            return loc;
        }
    };

    template <class CharT, class Traits>
    std::basic_ostream<CharT,Traits>&
    operator<<(std::basic_ostream<CharT,Traits>& os, const Location& loc)
    {
        os << loc.m_file << " (" << loc.m_line << ")";
        return os;
    }
}   // cparser

#endif  // def LOCATION_HPP_
