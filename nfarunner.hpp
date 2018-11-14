#pragma once

#include "nfanode.hpp"
#include "nfamachine.hpp"

#include <map>
#include <vector>

template <class T, class FwdIt>
class NFARunner {
public:
    NFARunner(NFAMachine<T> const& m, FwdIt begin, FwdIt const& end) : start(m.start), begin(begin), end(end) {}

    auto search() { return impl<true>(); }
    auto match() { return impl<false>(); }

private:
    template <bool IsSearch>
    auto impl() {
        if(!IsSearch) {
            addStates({ start }, { begin });
        }
        while(begin != end) {
            if(IsSearch) {
                addStates({ start }, { begin });
            }
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

    NFANode<T> const* start;
    FwdIt begin;
    FwdIt const& end;
};

template <class T, class FwdIt>
auto nfa_search(NFAMachine<T> const& m, FwdIt begin, FwdIt const& end) {
    return NFARunner(m, begin, end).search();
}

template <class T, class FwdIt>
auto nfa_match(NFAMachine<T> const& m, FwdIt begin, FwdIt const& end) {
    return NFARunner(m, begin, end).match();
}
