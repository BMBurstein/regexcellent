#include "expression_builder.hpp"

#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    std::cout << std::boolalpha;

    //auto regex = cat( cat( cat( star( any<char>() ), lit('a') ), alt( lit('b'), lit('c') ) ), opt( lit('d') ) ).compile();
    auto regex = (*any<char>() + lit('a') + (lit('b') | lit('c')) + !lit('d')).compile();

    for(int i=1; i<argc; ++i) {
        std::cout << argv[i] << ":\t" << regex.run(argv[i], argv[i]+std::strlen(argv[i])) << std::endl;
    }
}
