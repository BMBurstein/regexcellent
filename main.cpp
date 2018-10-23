#include "expression_builder.hpp"

#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    std::cout << std::boolalpha;

    //auto regex = cat( cat( star( any<char>() ), lit('a') ), alt( lit('b'), lit('c') ) ).compile();
    auto regex = (*any<char>() + lit('a') + (lit('b') | lit('c'))).compile();
    //auto regex = std::move(any<char>().star().concat(lit('a')).concat(std::move(lit('b').alt(lit('c')))).concat(accept<char>()));

    for(int i=1; i<argc; ++i) {
        std::cout << argv[i] << ":\t" << regex.run(argv[i], argv[i]+std::strlen(argv[i])) << std::endl;
    }
}

// cat( cat( star( any() ), lit(a) ), alt( lit(b), lit(c) ) )
//                    <>       <->            <->     <->
//               <------->               <---------------->
//         <----------------------->
//    <----------------------------------------------------->
// .*a(b|c)
