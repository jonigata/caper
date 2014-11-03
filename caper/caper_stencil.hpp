// 2014/03/21 Naoyuki Hirayama

/*!
	@file	  caper_stencil.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef CAPER_STENCIL_HPP_
#define CAPER_STENCIL_HPP_

#include <iostream>
#include <functional>
#include <string>
#include <map>

class StencilCallback {
public:
    StencilCallback() {}
    StencilCallback(bool n) {
        f_ = [=](std::ostream& os){ os << (n ? "true" : "false"); };
    }
    StencilCallback(int n) {
        f_ = [=](std::ostream& os){ os << std::to_string(n); };
    }
    StencilCallback(size_t n) {
        f_ = [=](std::ostream& os){ os << std::to_string(n); };
    }
    StencilCallback(const char* ss) {
        std::string s(ss);
        f_ = [=](std::ostream& os){ os << s; };
    }
    StencilCallback(const std::string& s) {
        f_ = [=](std::ostream& os){ os << s; };
    }
    template <class F>
    StencilCallback(F f) {
        f_ = f;
    }

    void operator()(std::ostream& os) const {
        f_(os);
    }

private:
    std::function<void (std::ostream&)> f_;
    
};

void stencil_output(
    std::ostream& os,
    const char* t,
    const std::map<std::string, StencilCallback>& m);

struct StencilBinding {
    std::string     name;
    StencilCallback callback;

    StencilBinding(const std::string& n, StencilCallback cb)
        : name(n), callback{ cb } {}
};

inline
void stencil_setup(std::map<std::string, StencilCallback>&) {
}

template <class ...T>
void stencil_setup(
    std::map<std::string, StencilCallback>& m,
    const StencilBinding& b,
    T... args) {
    m[b.name] = b.callback;
    stencil_setup(m, args...);
}

inline
void stencil(
    std::ostream& os, const char* t
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4,
    const StencilBinding& b5
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4, b5);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4,
    const StencilBinding& b5,
    const StencilBinding& b6
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4, b5, b6);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4,
    const StencilBinding& b5,
    const StencilBinding& b6,
    const StencilBinding& b7
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4, b5, b6, b7);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4,
    const StencilBinding& b5,
    const StencilBinding& b6,
    const StencilBinding& b7,
    const StencilBinding& b8
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4, b5, b6, b7, b8);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4,
    const StencilBinding& b5,
    const StencilBinding& b6,
    const StencilBinding& b7,
    const StencilBinding& b8,
    const StencilBinding& b9
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9);
    stencil_output(os, t, m);
}

inline
void stencil(
    std::ostream& os, const char* t,
    const StencilBinding& b0,
    const StencilBinding& b1,
    const StencilBinding& b2,
    const StencilBinding& b3,
    const StencilBinding& b4,
    const StencilBinding& b5,
    const StencilBinding& b6,
    const StencilBinding& b7,
    const StencilBinding& b8,
    const StencilBinding& b9,
    const StencilBinding& b10
    ) {
    std::map<std::string, StencilCallback> m;
    stencil_setup(m, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10);
    stencil_output(os, t, m);
}

#endif // CAPER_STENCIL_HPP_
