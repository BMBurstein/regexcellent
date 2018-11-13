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
        "abaceacabdert"
    };
    if(argc == 1) {
        argv = tests;
        argc = sizeof(tests) / sizeof(tests[0]);
    }
    std::cout << std::boolalpha;

    //auto regex = cat( cat( cat( cat( star( any<char>() ), lit('a') ), alt( lit('b'), lit('c') ) ), opt( lit('d') ) ), lit('e') ).compile();
    auto regex = (lit<char>('a') + (lit<char>('b') | lit('c')) + !lit('d') + lit('e')).compile();

    for(int i=1; i<argc; ++i) {
        auto res = NFARunner(regex, argv[i], argv[i]+std::strlen(argv[i])).search();
        std::cout << argv[i] << " (" << res.size() << "):\n";
        for(auto it : res) {
            std::cout << "  " << (it.first - argv[i]) << ':' << (it.second - argv[i]) << '\n';
        }
    }
}
