// SDL2 Demo - Phase 4 FFI with Pointers and Structures
// This demonstrates the new FFI capabilities including:
// - Pointer types and management
// - Structure handling
// - Automatic header parsing integration
// - SDL2 graphics programming

// FFI declarations for SDL2 core functions
ffi "SDL2.dll" SDL_Init(flags as integer) as integer;
ffi "SDL2.dll" SDL_CreateWindow(title as string, x as integer, y as integer, width as integer, height as integer, flags as integer) as pointer;
ffi "SDL2.dll" SDL_CreateRenderer(window as pointer, index as integer, flags as integer) as pointer;
ffi "SDL2.dll" SDL_SetRenderDrawColor(renderer as pointer, r as integer, g as integer, b as integer, a as integer) as integer;
ffi "SDL2.dll" SDL_RenderClear(renderer as pointer) as integer;
ffi "SDL2.dll" SDL_RenderPresent(renderer as pointer);
ffi "SDL2.dll" SDL_DestroyRenderer(renderer as pointer);
ffi "SDL2.dll" SDL_DestroyWindow(window as pointer);
ffi "SDL2.dll" SDL_Quit();
ffi "SDL2.dll" SDL_Delay(ms as integer);

// SDL2 Event handling
ffi "SDL2.dll" SDL_PollEvent(event as pointer) as integer;

// SDL2 Constants (these would normally be parsed from headers)
var SDL_INIT_VIDEO = 32;
var SDL_WINDOW_SHOWN = 4;
var SDL_RENDERER_ACCELERATED = 2;
var SDL_QUIT = 256;

function main() {
    print("SDL2 FFI Phase 4 Demo");
    print("Initializing SDL2...");
    
    // Initialize SDL2
    var init_result = SDL_Init(SDL_INIT_VIDEO);
    if (init_result != 0) {
        print("Failed to initialize SDL2");
        return 1;
    }
    
    print("Creating window...");
    
    // Create window using pointer return type
    var window = SDL_CreateWindow("rbasic SDL2 Demo", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (window == null) {
        print("Failed to create window");
        SDL_Quit();
        return 1;
    }
    
    print("Creating renderer...");
    
    // Create renderer
    var renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == null) {
        print("Failed to create renderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    print("SDL2 initialized successfully!");
    print("Running render loop for 3 seconds...");
    
    // Simple render loop
    var frames = 0;
    while (frames < 180) { // ~3 seconds at 60fps
        // Set background color (blue)
        SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
        
        // Clear screen
        SDL_RenderClear(renderer);
        
        // Present the rendered frame
        SDL_RenderPresent(renderer);
        
        // Delay to maintain ~60fps
        SDL_Delay(16);
        
        frames = frames + 1;
        
        if (frames % 60 == 0) {
            print("Frame: " + tostring(frames));
        }
    }
    
    print("Cleaning up...");
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    print("SDL2 demo completed successfully!");
    return 0;
}