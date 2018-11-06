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

    template <class InputIt>
    bool run(InputIt begin, InputIt const& end) {
        while(begin != end) {
            if(step(*begin)) {
                return true;
            }
            ++begin;
        }

        return false;
    }

    bool step(T const& v) {
        StateSet cur(std::move(next));
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
    StateSet next;

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
};
