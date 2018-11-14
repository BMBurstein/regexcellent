#include "expression_builder.hpp"
#include "nfarunner.hpp"

#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    char* tests[] = {
        nullptr,
        "abe",
        "abcde",
        "acde",
        "aceace",
        "abaceacabdert"
    };
    if(argc == 1) {
        argv = tests;
        argc = sizeof(tests) / sizeof(tests[0]);
    }
    std::cout << std::boolalpha;

    //auto regex = cat( cat( cat( cat( star( any<char>() ), lit('a') ), alt( lit('b'), lit('c') ) ), opt( lit('d') ) ), lit('e') ).compile();
    auto regex = (lit<char>('a') + (lit<char>('b') | lit('c')) + !lit('d') + lit('e')).compile();

    std::cout << "Search:\n";
    for(int i=1; i<argc; ++i) {
        auto res = nfa_search(regex, argv[i], argv[i]+std::strlen(argv[i]));
        std::cout << " " << argv[i] << " (" << res.size() << "):\n";
        for(auto it : res) {
            std::cout << "  " << (it.first - argv[i]) << ':' << (it.second - argv[i]) << '\n';
        }
    }

    std::cout << "\nMatch:\n";
    for(int i=1; i<argc; ++i) {
        auto res = nfa_match(regex, argv[i], argv[i]+std::strlen(argv[i]));
        std::cout << " " << argv[i] << " (" << res.size() << "):\n";
        for(auto it : res) {
            std::cout << "  " << (it.first - argv[i]) << ':' << (it.second - argv[i]) << '\n';
        }
    }
}
