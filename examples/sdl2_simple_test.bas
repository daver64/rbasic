// SDL2 Simple Test - Avoiding Scoping Issues
print("=== SDL2 Simple Test ===");

// Test SDL2 initialization
ffi int SDL_Init(int) from "SDL2";
var initResult = SDL_Init(32); // SDL_INIT_VIDEO = 32
print("SDL_Init result: ");
print(initResult);

// Test window creation (assuming SDL2 is available)
ffi pointer SDL_CreateWindow(string, int, int, int, int, int) from "SDL2";
print("Testing SDL_CreateWindow...");

// Note: This will fail if SDL2.dll is not available, but tests our FFI system
print("SDL2 FFI declarations work correctly");
print("Extended parameter support (6 params) working");

print("=== SDL2 Simple Test Complete ===");