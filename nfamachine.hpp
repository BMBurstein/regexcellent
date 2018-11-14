#pragma once

#include "nfanode.hpp"

#include <vector>
#include <memory>

template <class T>
using NodeVec = std::vector<std::unique_ptr<NFANode<T>>>;

template <class T>
class NFAMachine {
public:
    NFAMachine(NFAMachine const&) = delete;
    NFAMachine(NFAMachine &&) = default;

    NFAMachine(NFANode<T> const* s, NodeVec<T> v) : start(std::move(s)), nodes(std::move(v)) {}

    NFANode<T> const* start;

private:
    NodeVec<T> const nodes;
};
