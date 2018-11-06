#pragma once

#include <set>
#include <vector>

template <class T>
class NFANode;

template <class T>
using StateVec = std::vector<NFANode<T> const*>;

template <class T>
class NFANode {
public:
    NFANode(bool consumes, bool accepts = false) : accepts(accepts), consumes(consumes) {}
    virtual ~NFANode() {}

    const bool accepts;
    const bool consumes;

    virtual bool test(T const&) const { return true; }

    StateVec<T> out;
};

template <class T>
class NFAAcceptNode : public NFANode<T> {
public:
    NFAAcceptNode() : NFANode<T>(false, true) {}
};

template <class T>
class NFASplitNode : public NFANode<T> {
public:
    NFASplitNode() : NFANode<T>(false) {}
};

template <class T>
class NFACompareNode : public NFANode<T> {
public:
    NFACompareNode(T val) : NFANode<T>(true), val(std::move(val)) {}

    bool test(T const& v) const override {
        return val == v;
    }

private:
    T val;
};

template <class T>
class NFAAnyNode : public NFANode<T> {
public:
    NFAAnyNode() : NFANode<T>(true) {}
};
