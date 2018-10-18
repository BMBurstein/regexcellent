#pragma once
#include "nsanode.hpp"

template <class T>
class NSARunner {
public:
    void addStart(NSANode<T> const* s) {
        start.emplace_back(s);
    }

    template <class InputIt>
    bool run(InputIt begin, InputIt const& end) {
        StateCollection<T> cur;
        
        next.clear();
        if(addStates(start)) {
            return true;
        }
        while(begin != end) {
            std::swap(cur, next);
            next.clear();
            for(auto const& s: cur) {
                if(s->test(*begin)) {
                    if(addStates(s->out)) {
                        return true;
                    }
                }
            }
            ++begin;
        }

        return false;
    }

private:
    StateCollection<T> start;
    StateCollection<T> next;

    bool addStates(StateCollection<T> const& states) {
        for(auto const& s: states) {
            if(s->accepts) {
                return true;
            }
            if(s->consumes) {
                next.emplace_back(s);
            }
            else {
                if(addStates(s->out)) {
                    return true;
                }
            }
        }
        return false;
    }
};
