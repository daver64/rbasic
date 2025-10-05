// SDL2 Graphics Demo - Full FFI Capabilities Test
// Demonstrates advanced SDL2 integration with rbasic FFI

// SDL2 Core Functions
ffi "SDL2.dll" SDL_Init(flags as integer) as integer;
ffi "SDL2.dll" SDL_Quit();

// Window and Renderer Management  
ffi "SDL2.dll" SDL_CreateWindow(title as string, x as integer, y as integer, 
                                w as integer, h as integer, flags as integer) as pointer;
ffi "SDL2.dll" SDL_DestroyWindow(window as pointer);
ffi "SDL2.dll" SDL_CreateRenderer(window as pointer, index as integer, flags as integer) as pointer;
ffi "SDL2.dll" SDL_DestroyRenderer(renderer as pointer);

// Rendering Functions
ffi "SDL2.dll" SDL_SetRenderDrawColor(renderer as pointer, r as integer, g as integer, 
                                      b as integer, a as integer) as integer;
ffi "SDL2.dll" SDL_RenderClear(renderer as pointer) as integer;
ffi "SDL2.dll" SDL_RenderPresent(renderer as pointer);
ffi "SDL2.dll" SDL_RenderDrawPoint(renderer as pointer, x as integer, y as integer) as integer;
ffi "SDL2.dll" SDL_RenderDrawLine(renderer as pointer, x1 as integer, y1 as integer,
                                  x2 as integer, y2 as integer) as integer;

// Timing
ffi "kernel32.dll" Sleep(ms as integer) as integer;

// SDL2 Constants (from header parser definitions)
var SDL_INIT_VIDEO = 32;
var SDL_WINDOW_SHOWN = 4;
var SDL_RENDERER_ACCELERATED = 2;

function main() {
    print("=== SDL2 Graphics Demo ===");
    
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        print("Failed to initialize SDL2");
        return 1;
    }
    
    print("SDL2 initialized successfully");
    
    // Create window
    var window = SDL_CreateWindow("rbasic SDL2 Demo", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (window == null) {
        print("Failed to create window");
        SDL_Quit();
        return 1;
    }
    
    print("Window created successfully");
    
    // Create renderer
    var renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == null) {
        print("Failed to create renderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    print("Renderer created successfully");
    print("Running graphics demo for 3 seconds...");
    
    // Simple graphics demo
    for (var frame = 0; frame < 180; frame = frame + 1) {
        // Clear screen with blue background
        SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
        SDL_RenderClear(renderer);
        
        // Draw some graphics
        // White points
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (var i = 0; i < 50; i = i + 1) {
            var x = (frame + i * 16) mod 800;
            var y = 300 + sin(i * 0.1 + frame * 0.05) * 100;
            SDL_RenderDrawPoint(renderer, x, int(y));
        }
        
        // Red lines
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (var j = 0; j < 10; j = j + 1) {
            var x1 = j * 80;
            var y1 = 100;
            var x2 = j * 80 + 40;
            var y2 = 500;
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
        
        // Present the frame
        SDL_RenderPresent(renderer);
        
        // ~60 FPS timing
        Sleep(16);
        
        // Progress indicator
        if (frame mod 60 == 0) {
            print("Frame:", frame);
        }
    }
    
    print("Demo complete - cleaning up...");
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    print("SDL2 demo completed successfully!");
    return 0;
}