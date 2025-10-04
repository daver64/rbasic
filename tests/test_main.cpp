#include <iostream>
#include <cassert>

// Forward declarations of test functions
void test_lexer();
void test_parser();
void test_integration();

int main() {
    std::cout << "Running rbasic tests..." << std::endl;
    
    try {
        test_lexer();
        std::cout << "[PASS] Lexer tests passed" << std::endl;
        
        test_parser();
        std::cout << "[PASS] Parser tests passed" << std::endl;
        
        test_integration();
        std::cout << "[PASS] Integration tests passed" << std::endl;
        
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}