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

private:
    
public:
    NFANode<T> const* start;
    NodeVec<T> const nodes;

    // template <class FwdIt>
    // class MatchResult {
    // public:
    //     MatchResult(FwdIt start, FwdIt end, std::size len)
    //       : start(std::move(start)), end(std::move(end)), len(len)
    //     {}

    //     FwdIt start;
    //     FwdIt end;
    //     std::size_t len;
    // };

    
};
