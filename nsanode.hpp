#pragma once

#include <set>
#include <vector>

template <class T>
class NSANode;

template <class T>
using StateCollection = std::vector<NSANode<T> const*>;

template <class T>
class NSANode {
public:
    NSANode(bool consumes, bool accepts = false) : accepts(accepts), consumes(consumes) {}
    virtual ~NSANode() {}

    const bool accepts;
    const bool consumes;

    virtual bool test(T const&) const { return true;}

    StateCollection<T> out;
};

template <class T>
class NSAAcceptNode : public NSANode<T> {
public:
    NSAAcceptNode() : NSANode<T>(false, true) {}
};

template <class T>
class NSASplitNode : public NSANode<T> {
public:
    NSASplitNode() : NSANode<T>(false) {}
};

template <class T>
class NSACompareNode : public NSANode<T> {
public:
    NSACompareNode(T val) : NSANode<T>(true), val(std::move(val)) {}

    bool test(T const& v) const override {
        return val == v;
    }

private:
    T val;
};

template <class T>
class NSAAnyNode : public NSANode<T> {
public:
    NSAAnyNode() : NSANode<T>(true) {}
};
