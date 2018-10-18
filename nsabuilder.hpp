#pragma once

#include "nsanode.hpp"

#include <vector>
#include <memory>
#include <iterator>

template <class T>
class NSAMachine {
public:
    NSAMachine(NSAMachine const&) = delete;
    NSAMachine(NSAMachine &&) = default;

    NSAMachine(std::unique_ptr<NSANode<T>> node) {
        out.emplace_back(node.get());
        start = node.get();
        nodes.emplace_back(std::move(node));
    }
    
    NSAMachine& concat(NSAMachine&& m) {
        nodes.insert(nodes.end(),
                     std::make_move_iterator(m.nodes.begin()),
                     std::make_move_iterator(m.nodes.end())
                    );
        
        for(auto& p : out)
        {
            p->out.emplace_back(m.start);
        }
        
        out = std::move(m.out);

        return *this;
    }
    
    NSAMachine& alt(NSAMachine&& m) {
        nodes.insert(nodes.end(),
                     std::make_move_iterator(m.nodes.begin()),
                     std::make_move_iterator(m.nodes.end())
                    );
        
        auto s = std::make_unique<NSASplitNode<T>>();
        s->out.emplace_back(start);
        s->out.emplace_back(m.start);
        start = s.get();
        nodes.emplace_back(std::move(s));
        
        out.insert(out.end(), m.out.begin(), m.out.end());

        return *this;
    }

    NSAMachine& star() {
        auto s = std::make_unique<NSASplitNode<T>>();
        s->out.emplace_back(start);
        start = s.get();
        
        for(auto& p : out)
        {
            p->out.emplace_back(start);
        }
        out.clear();
        out.emplace_back(s.get());

        nodes.emplace_back(std::move(s));

        return *this;
    }

private:
public:
    NSANode<T> const* start;
    std::vector<std::unique_ptr<NSANode<T>>> nodes;
    std::vector<NSANode<T> *> out;
};

template <class T>
NSAMachine<T> lit(T const& l) {
    return NSAMachine<T>(std::make_unique<NSACompareNode<T>>(l));
}

template <class T>
NSAMachine<T> any() {
    return NSAMachine<T>(std::make_unique<NSAAnyNode<T>>());
}

template <class T>
NSAMachine<T> accept() {
    return NSAMachine<T>(std::make_unique<NSAAcceptNode<T>>());
}

template <class T>
NSAMachine<T> cat(NSAMachine<T> a, NSAMachine<T> b) {
    return std::move(a.concat(std::move(b)));
}

template <class T>
NSAMachine<T> alt(NSAMachine<T> a, NSAMachine<T> b) {
    return std::move(a.alt(std::move(b)));
}

template <class T>
NSAMachine<T> star(NSAMachine<T> a) {
    return std::move(a.star());
}