#include <iostream>
#include <windows.h>

int main() {
    // Try to attach to parent console or allocate new one
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        AllocConsole();
    }
    
    // Redirect stdout to console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    
    std::cout << "Hello from MinGW64 with console allocation!" << std::endl;
    std::cout.flush();
    
    // Give time to see output
    Sleep(1000);
    
    return 0;
}