#pragma once

#include "nsanode.hpp"
#include "nsamachine.hpp"

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