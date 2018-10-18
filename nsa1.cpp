#include "nsabuilder.hpp"

#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    std::cout << std::boolalpha;

    auto regex = cat( cat( cat( star( any<char>() ), lit('a') ), alt( lit('b'), lit('c') ) ), accept<char>() );

    for(int i=1; i<argc; ++i) {
        std::cout << argv[i] << ":\t" << regex.run(argv[i], argv[i]+std::strlen(argv[i])) << std::endl;
    }
}

// auto regex = cat( cat( star( any() ), lit(a) ), alt( lit(b), lit(c) ) )
//                                 <>       <->            <->     <->
//                            <------->               <---------------->
//                      <----------------------->
//                 <----------------------------------------------------->
