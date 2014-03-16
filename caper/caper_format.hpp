// 2014/03/16 Naoyuki Hirayama

#ifndef CAPER_FORMAT_HPP_
#define CAPER_FORMAT_HPP_

#include <boost/format.hpp>

// typesafe sprintf
inline
void format(boost::format& f) {
}

template <typename TV, typename... T> inline
void format(boost::format& f, TV arg, T... args) {
    f % arg;
    format(f, args...);
}

template <typename ...T> inline
std::string format(const std::string& fs, T... args) {
    boost::format f(fs);
    format(f, args...);
    return f.str();
}

#endif // CAPER_FORMAT_HPP_
