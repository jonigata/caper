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
    StencilCallback(bool n) {
        f_ = [&](std::ostream& os){ os << (n ? "true" : "false"); };
    }
    StencilCallback(int n) {
        f_ = [&](std::ostream& os){ os << std::to_string(n); };
    }
    StencilCallback(size_t n) {
        f_ = [&](std::ostream& os){ os << std::to_string(n); };
    }
    StencilCallback(const char* s) {
        f_ = [&](std::ostream& os){ os << s; };
    }
    StencilCallback(const std::string& s) {
        f_ = [&](std::ostream& os){ os << s; };
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

void stencil(
    std::ostream& os,
    const char* t,
    const std::map<std::string, StencilCallback>& m);

#endif // CAPER_STENCIL_HPP_
