// SDL2 Basic Test - What Works Right Now
print("=== SDL2 Basic Test ===");

// Test SDL2 initialization
ffi int SDL_Init(int) from "SDL2";
var initResult = SDL_Init(32); // SDL_INIT_VIDEO = 32
print("SDL_Init result: ");
print(initResult);

if (initResult == 0) {
    print("SDL2 initialized successfully!");
    
    // Test window creation
    ffi pointer SDL_CreateWindow(string, int, int, int, int, int) from "SDL2";
    var window = SDL_CreateWindow("rbasic SDL2 Test", 100, 100, 800, 600, 4); // SDL_WINDOW_SHOWN = 4
    
    if (window != 0) {
        print("Window created successfully!");
        
        // Test renderer creation  
        ffi pointer SDL_CreateRenderer(pointer, int, int) from "SDL2";
        var renderer = SDL_CreateRenderer(window, -1, 2); // SDL_RENDERER_ACCELERATED = 2
        
        if (renderer != 0) {
            print("Renderer created successfully!");
            
            // Test clearing screen
            ffi int SDL_RenderClear(pointer) from "SDL2";
            var clearRes = SDL_RenderClear(renderer);
            print("Clear result: ");
            print(clearRes);
            
            // Test presenting
            ffi void SDL_RenderPresent(pointer) from "SDL2";
            SDL_RenderPresent(renderer);
            print("Screen presented!");
            
            // Sleep for 2 seconds to see the window
            ffi void Sleep(int) from "kernel32";
            Sleep(2000);
            
            // Cleanup
            ffi void SDL_DestroyRenderer(pointer) from "SDL2";
            SDL_DestroyRenderer(renderer);
            print("Renderer destroyed");
        } else {
            print("Failed to create renderer");
        }
        
        ffi void SDL_DestroyWindow(pointer) from "SDL2";
        SDL_DestroyWindow(window);
        print("Window destroyed");
    } else {
        print("Failed to create window");
    }
    
    ffi void SDL_Quit() from "SDL2";
    SDL_Quit();
    print("SDL2 shut down");
} else {
    print("Failed to initialize SDL2");
}

print("=== SDL2 Test Complete ===");