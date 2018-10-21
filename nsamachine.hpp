#include "nsanode.hpp"

#include <vector>
#include <memory>
#include <iterator>
#include <set>

template <class T>
class NSAMachine {
public:
    NSAMachine(NSAMachine const&) = delete;
    NSAMachine(NSAMachine &&) = default;

    NSAMachine(NSANode<T> const s, std::vector<std::unique_ptr<NSANode<T>>> v) : start(std::move(s)), nodes(std::move(v)) {}
    
    // NSAMachine(std::unique_ptr<NSANode<T>> node) {
    //     out.emplace_back(node.get());
    //     start = node.get();
    //     nodes.emplace_back(std::move(node));
    // }
    
    // NSAMachine& concat(NSAMachine&& m) {
    //     nodes.insert(nodes.end(),
    //                  std::make_move_iterator(m.nodes.begin()),
    //                  std::make_move_iterator(m.nodes.end())
    //                 );
        
    //     for(auto& p : out)
    //     {
    //         p->out.emplace_back(m.start);
    //     }
        
    //     out = std::move(m.out);

    //     return *this;
    // }
    
    // NSAMachine& alt(NSAMachine&& m) {
    //     nodes.insert(nodes.end(),
    //                  std::make_move_iterator(m.nodes.begin()),
    //                  std::make_move_iterator(m.nodes.end())
    //                 );
        
    //     auto s = std::make_unique<NSASplitNode<T>>();
    //     s->out.emplace_back(start);
    //     s->out.emplace_back(m.start);
    //     start = s.get();
    //     nodes.emplace_back(std::move(s));
        
    //     out.insert(out.end(), m.out.begin(), m.out.end());

    //     return *this;
    // }

    // NSAMachine& star() {
    //     auto s = std::make_unique<NSASplitNode<T>>();
    //     s->out.emplace_back(start);
    //     start = s.get();
        
    //     for(auto& p : out)
    //     {
    //         p->out.emplace_back(start);
    //     }
    //     out.clear();
    //     out.emplace_back(s.get());

    //     nodes.emplace_back(std::move(s));

    //     return *this;
    // }

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
    std::vector<std::unique_ptr<NSANode<T>>> nodes;
};
