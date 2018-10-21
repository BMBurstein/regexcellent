#pragma once

#include "nsanode.hpp"
#include "nsamachine.hpp"

#include <memory>
#include <utility>
#include <vector>

template <class T>
class accept;

template <class T>
class NSABuilder {
public:
    NSAMachine<T> compile() const {
        NodeVec v;
        auto nsa = makeNSA(v);
        connect(nsa, accept<T>().makeNSA());
        return {std::move(nsa.start), std::move(v)};
    }

    virtual ~NSABuilder() {};

protected:
    using NodeVec = std::vector<std::unique_ptr<NSANode<T>>>;

    struct NSAPart {
        NSANode<T> const* start;
        std::vector<NSANode<T> *> out;
    };

    template <template<typename> class Node_T, typename... Args>
    static typename std::unique_ptr<Node_T<T>>::pointer make_unique(NodeVec& v, Args&&... args) {
        v.emplace_back(std::make_unique<Node_T<T>>(std::forward<Args>(args)...));
        auto p = v.back().get();
        return p;
    }

    template <template<typename> class Node_T, typename... Args>
    static NSAPart makeNSAPart(NodeVec& v, Args&&... args) {
        auto p = make_unique<Node_T>(std::forward<Args>(args)...);
        return {p, {p}};
    }

    static void connect(NSAPart& part_a, NSAPart const& part_b) {
        for(auto& p : part_a.out) {
            p->out.emplace_back(part_b.start);
        }
    }

private:
    virtual NSAPart makeNSA(NodeVec& v) const = 0;
};

template <class T>
class lit : public NSABuilder<T> {
public:
    lit(T l) : val(std::move(l)) {}
private:
    T val;

    typename NSABuilder<T>::NSAPart makeNSA(typename NSABuilder<T>::NodeVec& v) const override {
        return NSABuilder<T>::template makeNSAPart<NSACompareNode>(v, val);
    }
};

// template <class T>
// NSAMachine<T> lit(T const& l) {
//     return NSAMachine<T>(std::make_unique<NSACompareNode<T>>(l));
// }

template <class T>
class any : public NSABuilder<T> {
private:
    typename NSABuilder<T>::NSAPart makeNSA(typename NSABuilder<T>::NodeVec& v) const override {
        return NSABuilder<T>::template makeNSAPart<NSAAnyNode>();
    }
};

// template <class T>
// NSAMachine<T> any() {
//     return NSAMachine<T>(std::make_unique<NSAAnyNode<T>>());
// }

template <class T>
class accept : public NSABuilder<T> {
private:
    typename NSABuilder<T>::NSAPart makeNSA(typename NSABuilder<T>::NodeVec& v) const override {
        return NSABuilder<T>::template makeNSAPart<NSAAcceptNode>();
    }
};

// template <class T>
// NSAMachine<T> accept() {
//     return NSAMachine<T>(std::make_unique<NSAAcceptNode<T>>());
// }

template <class T>
class cat : public NSABuilder<T> {
public:
    cat(NSABuilder<T> a, NSABuilder<T> b) : a(std::move(a)), b(std::move(b)) {}

private:
    NSABuilder<T> a, b;

    typename NSABuilder<T>::NSAPart makeNSA(typename NSABuilder<T>::NodeVec& v) const override {
        auto part_a = a.makeNSA(v);
        auto part_b = b.makeNSA(v);
        connect(part_a, part_b);
        return {std::move(part_a.start), std::move(part_b.out)};
    }
};

// template <class T>
// NSAMachine<T> cat(NSAMachine<T> a, NSAMachine<T> b) {
//     return std::move(a.concat(std::move(b)));
// }

template <class T>
class alt : public NSABuilder<T> {
public:
    alt(NSABuilder<T> a, NSABuilder<T> b) : a(std::move(a)), b(std::move(b)) {}

private:
    NSABuilder<T> a, b;

    typename NSABuilder<T>::NSAPart makeNSA(typename NSABuilder<T>::NodeVec& v) const override {
        auto part_a = a.makeNSA(v);
        auto part_b = b.makeNSA(v);
        auto s = NSABuilder<T>::template makeNSAPart<NSASplitNode>();
        connect(s, part_a);
        connect(s, part_b);
        s.out = std::move(part_a.out);
        s.out.insert(s.out.end(), part_b.out.begin(), part_b.out.end());
        return s;
    }
};

// template <class T>
// NSAMachine<T> alt(NSAMachine<T> a, NSAMachine<T> b) {
//     return std::move(a.alt(std::move(b)));
// }

template <class T>
class star : public NSABuilder<T> {
public:
    star(NSABuilder<T> const& a) : a(a) {}
    star(NSABuilder<T> && a) : a(std::move(a)) {}

private:
    NSABuilder<T> a;

    typename NSABuilder<T>::NSAPart makeNSA(typename NSABuilder<T>::NodeVec& v) const override {
        auto part = a.makeNSA(v);
        auto s = NSABuilder<T>::template makeNSAPart<NSASplitNode>();
        connect(s, part);
        connect(part, s);
        return s;
    }
};

// template <class T>
// NSAMachine<T> star(NSAMachine<T> a) {
//     return std::move(a.star());
// }

// template <class T>
// NSABuilder<T> operator+(NSABuilder<T> a, NSABuilder<T> b) {
//     return cat(std::move(a), std::move(b));
// }

// template <class T>
// NSABuilder<T> operator|(NSABuilder<T> a, NSABuilder<T> b) {
//     return alt(std::move(a), std::move(b));
// }

// template <class T>
// NSABuilder<T> operator*(NSABuilder<T> a) {
//     return star(std::move(a));
// }
