#include "runtime/basic_runtime.h"
#include <iostream>

int main() {
    std::cout << "Before init_runtime" << std::endl;
    basic_runtime::init_runtime();
    std::cout << "After init_runtime" << std::endl;
    
    BasicValue test_value("Test string");
    std::cout << "Created BasicValue" << std::endl;
    
    basic_runtime::print(test_value);
    std::cout << "After print call" << std::endl;
    
    return 0;
}