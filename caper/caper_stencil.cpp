// 2014/03/21 Naoyuki Hirayama

#include <iostream>
#include <sstream>
#include <cassert>
#include <exception>
#include "caper_stencil.hpp"
#include "caper_format.hpp"

void stencil_output(
    std::ostream& os,
    int indent,
    const char* t,
    const std::map<std::string, StencilCallback>& m) {

    std::stringstream ss(t);
    int c = ss.get();
    if (c != '\n') {
        ss.unget();
    }

    bool beginning_of_line = true;
    while((c = ss.get()) != std::char_traits<char>::eof()) {
        if (beginning_of_line) {
            for (int i = 0 ; i < indent ; i++) { os.put(' '); }
            beginning_of_line = false;
        }
        if (c == '$') {
            bool chomp = false;
            c = ss.get();
            if (c == '$') {
                chomp = true;
                c = ss.get();
            }
            if (c != '{') {
                throw std::runtime_error(
                    format("stencil: unexpected char: %c", char(c)));
            }
            assert(c == '{');
            std::stringstream k;
            while ((c = ss.get()) != '}') {
                k << char(c);
            }
            auto f = m.find(k.str());
            if (f == m.end()) {
                throw std::runtime_error(
                    "undefined template parameter: " + k.str());
            }
            (*f).second(os);
            if (chomp) {
                ss.get();
            }
        } else {
            os << char(c);
            if (c == '\n') {
                beginning_of_line = true;
            }
        }
    }
}

/*
int main() {
    stencil(
        "${hello} = ${world}\n",
        {
            {"hello", []() {
                    return "guten tag";
                }},
            {"world", []() {
                    return "welt";
                }}
        });
    return 0;
}
*/
