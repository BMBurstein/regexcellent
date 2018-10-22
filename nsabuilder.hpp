#pragma once

#include "nsanode.hpp"
#include "nsamachine.hpp"

#include <memory>
#include <utility>
#include <vector>

template <class T>
class NSABuilder {
public:
    NSAMachine<T> compile() const {
        NodeVec<T> v;
        auto nsa = makeNSA(v);
        connect(nsa, makeNSAPart<NSAAcceptNode>(v));
        return {std::move(nsa.start), std::move(v)};
    }

    virtual ~NSABuilder() {};

protected:
    struct NSAPart {
        NSANode<T> const* start;
        std::vector<NSANode<T> *> out;
    };

    template <template<typename> class Node_T, typename... Args>
    static NSAPart makeNSAPart(NodeVec<T>& v, Args&&... args) {
        v.emplace_back(std::make_unique<Node_T<T>>(std::forward<Args>(args)...));
        auto p = v.back().get();
        return {p, {p}};
    }

    static void connect(NSAPart& part_a, NSAPart const& part_b) {
        for(auto& p : part_a.out) {
            p->out.emplace_back(part_b.start);
        }
    }

public:
    virtual NSAPart makeNSA(NodeVec<T>& v) const = 0;
};

template <class T>
class lit : public NSABuilder<T> {
public:
    lit(T l) : val(std::move(l)) {}
private:
    T val;

    typename NSABuilder<T>::NSAPart makeNSA(NodeVec<T>& v) const override {
        return NSABuilder<T>::template makeNSAPart<NSACompareNode>(v, val);
    }
};

template <class T>
class any : public NSABuilder<T> {
private:
    typename NSABuilder<T>::NSAPart makeNSA(NodeVec<T>& v) const override {
        return NSABuilder<T>::template makeNSAPart<NSAAnyNode>(v);
    }
};

template <class T>
class cat : public NSABuilder<T> {
public:
    cat(NSABuilder<T> const& a, NSABuilder<T> const& b) : a(a), b(b) {}

private:
    NSABuilder<T> const& a;
    NSABuilder<T> const& b;

    typename NSABuilder<T>::NSAPart makeNSA(NodeVec<T>& v) const override {
        auto part_a = a.makeNSA(v);
        auto part_b = b.makeNSA(v);
        this->connect(part_a, part_b);
        return {std::move(part_a.start), std::move(part_b.out)};
    }
};

template <class T>
class alt : public NSABuilder<T> {
public:
    alt(NSABuilder<T> const& a, NSABuilder<T> const& b) : a(a), b(b) {}

private:
    NSABuilder<T> const& a;
    NSABuilder<T> const& b;

    typename NSABuilder<T>::NSAPart makeNSA(NodeVec<T>& v) const override {
        auto part_a = a.makeNSA(v);
        auto part_b = b.makeNSA(v);
        auto s = NSABuilder<T>::template makeNSAPart<NSASplitNode>(v);
        this->connect(s, part_a);
        this->connect(s, part_b);
        s.out = std::move(part_a.out);
        s.out.insert(s.out.end(), part_b.out.begin(), part_b.out.end());
        return s;
    }
};

template <class T>
class star : public NSABuilder<T> {
public:
    star(NSABuilder<T> const& a) : a(a) {}

private:
    NSABuilder<T> const& a;

    typename NSABuilder<T>::NSAPart makeNSA(NodeVec<T>& v) const override {
        auto part = a.makeNSA(v);
        auto s = NSABuilder<T>::template makeNSAPart<NSASplitNode>(v);
        this->connect(s, part);
        this->connect(part, s);
        return s;
    }
};

template <class T>
cat<T> operator+(NSABuilder<T> const& a, NSABuilder<T> const& b) {
    return cat(a, b);
}

template <class T>
alt<T> operator|(NSABuilder<T> const& a, NSABuilder<T> const& b) {
    return alt(a, b);
}

template <class T>
star<T> operator*(NSABuilder<T> const& a) {
    return star(a);
}
