#pragma once

#include "nfanode.hpp"
#include "nfamachine.hpp"

#include <memory>
#include <utility>
#include <vector>

template <class T>
class ExpressionBuilder {
public:
    NFAMachine<T> compile() const {
        NodeVec<T> v;
        auto nfa = makeNFA(v);
        connect(nfa, makeNFAPart<NFAAcceptNode>(v));
        return {std::move(nfa.start), std::move(v)};
    }

    virtual ~ExpressionBuilder() {};

protected:
    struct NFAPart {
        NFANode<T> const* start;
        std::vector<NFANode<T> *> out;
    };

    template <template<typename> class Node_T, typename... Args>
    static NFAPart makeNFAPart(NodeVec<T>& v, Args&&... args) {
        v.emplace_back(std::make_unique<Node_T<T>>(std::forward<Args>(args)...));
        auto p = v.back().get();
        return {p, {p}};
    }

    static void connect(NFAPart& part_a, NFAPart const& part_b) {
        for(auto& p : part_a.out) {
            p->out.emplace_back(part_b.start);
        }
    }

public:
    virtual NFAPart makeNFA(NodeVec<T>& v) const = 0;
};

template <class T>
class lit : public ExpressionBuilder<T> {
public:
    lit(T l) : val(std::move(l)) {}
private:
    T val;

    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        return ExpressionBuilder<T>::template makeNFAPart<NFACompareNode>(v, val);
    }
};

template <class T>
class any : public ExpressionBuilder<T> {
private:
    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        return ExpressionBuilder<T>::template makeNFAPart<NFAAnyNode>(v);
    }
};

template <class T>
class cat : public ExpressionBuilder<T> {
public:
    cat(ExpressionBuilder<T> const& a, ExpressionBuilder<T> const& b) : a(a), b(b) {}

private:
    ExpressionBuilder<T> const& a;
    ExpressionBuilder<T> const& b;

    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        auto part_a = a.makeNFA(v);
        auto part_b = b.makeNFA(v);
        this->connect(part_a, part_b);
        return {std::move(part_a.start), std::move(part_b.out)};
    }
};

template <class T>
class alt : public ExpressionBuilder<T> {
public:
    alt(ExpressionBuilder<T> const& a, ExpressionBuilder<T> const& b) : a(a), b(b) {}

private:
    ExpressionBuilder<T> const& a;
    ExpressionBuilder<T> const& b;

    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        auto part_a = a.makeNFA(v);
        auto part_b = b.makeNFA(v);
        auto s = ExpressionBuilder<T>::template makeNFAPart<NFASplitNode>(v);
        this->connect(s, part_a);
        this->connect(s, part_b);
        s.out = std::move(part_a.out);
        s.out.insert(s.out.end(), part_b.out.begin(), part_b.out.end());
        return s;
    }
};

template <class T>
class star : public ExpressionBuilder<T> {
public:
    star(ExpressionBuilder<T> const& a) : a(a) {}

private:
    ExpressionBuilder<T> const& a;

    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        auto part = a.makeNFA(v);
        auto s = ExpressionBuilder<T>::template makeNFAPart<NFASplitNode>(v);
        this->connect(s, part);
        this->connect(part, s);
        return s;
    }
};

template <class T>
class plus : public ExpressionBuilder<T> {
public:
    plus(ExpressionBuilder<T> const& a) : a(a) {}

private:
    ExpressionBuilder<T> const& a;

    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        auto part = a.makeNFA(v);
        auto s = ExpressionBuilder<T>::template makeNFAPart<NFASplitNode>(v);
        this->connect(part, s);
        this->connect(s, part);
        return {part.start, s.out};
    }
};

template <class T>
class opt : public ExpressionBuilder<T> {
public:
    opt(ExpressionBuilder<T> const& a) : a(a) {}

private:
    ExpressionBuilder<T> const& a;

    typename ExpressionBuilder<T>::NFAPart makeNFA(NodeVec<T>& v) const override {
        auto part = a.makeNFA(v);
        auto s = ExpressionBuilder<T>::template makeNFAPart<NFASplitNode>(v);
        this->connect(s, part);
        auto p = s.out[0];
        s.out = std::move(part.out);
        s.out.emplace_back(p);
        return s;
    }
};

template <class T>
cat<T> operator+(ExpressionBuilder<T> const& a, ExpressionBuilder<T> const& b) {
    return cat(a, b);
}

template <class T>
plus<T> operator+(ExpressionBuilder<T> const& a) {
    return plus(a);
}

template <class T>
alt<T> operator|(ExpressionBuilder<T> const& a, ExpressionBuilder<T> const& b) {
    return alt(a, b);
}

template <class T>
star<T> operator*(ExpressionBuilder<T> const& a) {
    return star(a);
}

template <class T>
opt<T> operator!(ExpressionBuilder<T> const& a) {
    return opt(a);
}
