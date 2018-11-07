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

    NFAMachine(NFANode<T> const* s, NodeVec<T> v) : start(std::move(s)), nodes(std::move(v)) {
        reset();
    }

    void reset() {
        next.clear();
        hasMatch = false;
        addStates({start});
    }

    template <class FwdIt>
    bool match(FwdIt begin, FwdIt const& end) {
        reset();
        FwdIt start;
        while(begin != end) {
            //addStates({start});
            if(step(*begin)) {
                return true;
            }
            ++begin;
            if(next.empty()) {
                start = begin;
            }
        }

        return false;
    }

    bool step(T const& v) {
        next.swap(cur);
        next.clear();

        for(auto const& s: cur) {
            if(s->test(v)) {
                addStates(s->out);
            }
        }

        return hasMatch;
    }

private:
    using StateSet = std::set<NFANode<T> const*>;
    StateSet next, cur;

    bool hasMatch = false;

    void addStates(StateVec<T> const& states) {
        for(auto const& s: states) {
            if(s->accepts) {
                hasMatch = true;
            }
            if(s->consumes) {
                next.emplace(s);
            }
            else {
                addStates(s->out);
            }
        }
    }

    NFANode<T> const* start;
    NodeVec<T> nodes;

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
