#include "capella_dependency.hpp"

////////////////////////////////////////////////////////////////
// DependencyMaker
struct DependencyMaker : public boost::static_visitor<void> {
    DependencyMaker(Dependency& y) : dependency(y) {}
    Dependency& dependency;

    // match
    template <class T>
    void operator()(const T&) const {}

    template <class T>
    void apply_to_vector(const std::vector<T>& x) const {
        for (const auto& y: x) {
            boost::apply_visitor(DependencyMaker(dependency), y);
        }
    }

    void operator()(const Module& x) const {
        apply_to_vector(x.declarations);
    }

    void operator()(const TypeDef& x) const {
        switch (x.right.which()) {
            case 1: { // Scalor
                Scalor s = boost::get<Scalor>(x.right);
                dependency.add_edge(x.name.s, s.type.s);
                break;
            }
            case 2: { // List
                List s = boost::get<List>(x.right);
                dependency.add_edge(x.name.s, s.etype.s);
                break;
            }
            case 3: { // Variant
                Variant v = boost::get<Variant>(x.right);
                for (const auto& y: v.choises) {
                    dependency.add_edge(x.name.s, y.s);
                }
                break;
            }
            case 4: { // Tuple
                Tuple s = boost::get<Tuple>(x.right);
                for (const auto& ti: s.elements) {
                    switch (ti.which()) {
                        case 0: dependency.add_edge(x.name.s, boost::get<Scalor>(ti).type.s); break;
                        case 1: dependency.add_edge(x.name.s, boost::get<List>(ti).etype.s); break;
                    }
                }
                break;
            }
        }
    }
};

void make_dependency(Dependency& dependency, const Value& v) {
    DependencyMaker dm(dependency);
    boost::apply_visitor(dm, v.data);
}
