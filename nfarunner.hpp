#pragma once

#include "nfamachine.hpp"

#include <map>
#include <vector>

template <class T, class FwdIt>
class NFARunner {
public:
    NFARunner(NFAMachine<T> const& m, FwdIt begin, FwdIt const& end) : m(m), begin(begin), end(end) {}

    auto search() {
        while(begin != end) {
            addStates({ m.start }, { begin });
            next.swap(cur);
            next.clear();

            auto const v = *begin;
            ++begin;
            for(auto const& s: cur) {
                if(s.first->test(v)) {
                    addStates(s.first->out, s.second);
                }
            }
        }

        return matches;
    }

private:
    using StateSet = std::map<NFANode<T> const*, std::vector<FwdIt>>;
    StateSet next, cur;

    std::vector<std::pair<FwdIt, FwdIt>> matches;

    void addStates(StateVec<T> const& states, std::vector<FwdIt> starts) {
        for(auto const& s: states) {
            if(s->accepts) {
                for(auto start: starts) {
                    matches.emplace_back(start, begin);
                }
            }
            if(s->consumes) {
                next[s].insert(next[s].end(), starts.begin(), starts.end());
            }
            else {
                addStates(s->out, starts);
            }
        }
    }

    NFAMachine<T> const& m;
    FwdIt begin;
    FwdIt const& end;
};