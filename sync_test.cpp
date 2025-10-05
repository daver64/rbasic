#include <iostream>
#include <cstdio>

int main() {
    std::ios::sync_with_stdio(true);
    std::cin.tie(&std::cout);
    
    printf("Before cout\n");
    fflush(stdout);
    
    std::cout << "Hello from cout with sync!" << std::endl;
    std::cout.flush();
    
    printf("After cout\n");
    fflush(stdout);
    
    return 0;
}