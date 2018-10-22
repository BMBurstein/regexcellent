#include "nsanode.hpp"

#include <vector>
#include <memory>

template <class T>
using NodeVec = std::vector<std::unique_ptr<NSANode<T>>>;

template <class T>
class NSAMachine {
public:
    NSAMachine(NSAMachine const&) = delete;
    NSAMachine(NSAMachine &&) = default;

    NSAMachine(NSANode<T> const* s, NodeVec<T> v) : start(std::move(s)), nodes(std::move(v)) {}

    template <class InputIt>
    bool run(InputIt begin, InputIt const& end) {
        StateSet cur, next;
        
        next.clear();
        if(addStates({start}, next)) {
            return true;
        }
        while(begin != end) {
            std::swap(cur, next);
            next.clear();
            for(auto const& s: cur) {
                if(s->test(*begin)) {
                    if(addStates(s->out, next)) {
                        return true;
                    }
                }
            }
            ++begin;
        }

        return false;
    }

private:
    using StateSet = std::set<NSANode<T> const*>;

    static bool addStates(StateVec<T> const& states, StateSet& next) {
        for(auto const& s: states) {
            if(s->accepts) {
                return true;
            }
            if(s->consumes) {
                next.emplace(s);
            }
            else {
                if(addStates(s->out, next)) {
                    return true;
                }
            }
        }
        return false;
    }

    NSANode<T> const* start;
    NodeVec<T> nodes;
};
