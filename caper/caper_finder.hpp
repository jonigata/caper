// 2014/03/23 Naoyuki Hirayama

/*!
	@file	  caper_finder.hpp
	@brief	  <ŠT—v>

	<à–¾>
*/

#ifndef CAPER_FINDER_HPP_
#define CAPER_FINDER_HPP_

template <class T, class V>
class find_iterator {
public:
    typedef typename T::const_iterator  original_iterator_type;

public:
    find_iterator(const T& c, const V& v)
        : c_(c), it_(c.find(v)) {
    }

    operator bool() const {
        return it_ != c_.end();
    }

    const typename T::value_type::second_type& operator*() const {
        return (*it_).second;
    }

private:
    const T&                c_;
    original_iterator_type  it_;
    
};

template <class T, class V>
find_iterator<T, V> finder(const T& c, const V& v) {
    return find_iterator<T, V>(c, v);
}

#endif // CAPER_FINDER_HPP_
