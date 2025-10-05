#include "runtime/basic_runtime.h"
#include <iostream>

int main() {
    basic_runtime::init_runtime();
    
    // Test direct iostream
    std::cout << "Direct cout test" << std::endl;
    std::cout.flush();
    
    // Test runtime function
    basic_runtime::print("Runtime print test");
    basic_runtime::print_line();
    
    return 0;
}