// SDL2 Complete Example with Output Parameters and Struct Handling
print("=== SDL2 Complete Example ===");

// Initialize SDL2
ffi int SDL_Init(int) from "SDL2";
var initResult = SDL_Init(32); // SDL_INIT_VIDEO = 32

if (initResult == 0) {
    print("SDL2 initialized successfully!");
    
    // Create window
    ffi pointer SDL_CreateWindow(string, int, int, int, int, int) from "SDL2";
    var window = SDL_CreateWindow("rbasic SDL2 with Buffers", 100, 100, 800, 600, 4);
    
    if (window != 0) {
        print("Window created successfully!");
        
        // Create renderer
        ffi pointer SDL_CreateRenderer(pointer, int, int) from "SDL2";
        var renderer = SDL_CreateRenderer(window, -1, 2);
        
        if (renderer != 0) {
            print("Renderer created successfully!");
            
            // Create SDL_Event buffer for event handling
            var event = create_sdl_event();
            print("Event buffer created");
            
            // Create SDL_Rect for rendering
            var rect = create_sdl_rect(100, 100, 200, 150);
            print("Rectangle created at position (100,100) with size 200x150");
            
            // Verify rect values
            var rectX = get_rect_field(rect, "x");
            var rectY = get_rect_field(rect, "y");
            var rectW = get_rect_field(rect, "w");
            var rectH = get_rect_field(rect, "h");
            print("Rect verification - X: ");
            print(rectX);
            print(" Y: ");
            print(rectY);
            print(" W: ");
            print(rectW);
            print(" H: ");
            print(rectH);
            
            // Set render color (red background)
            ffi int SDL_SetRenderDrawColor(pointer, int, int, int, int) from "SDL2";
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            
            // Clear screen
            ffi int SDL_RenderClear(pointer) from "SDL2";
            SDL_RenderClear(renderer);
            
            // Present
            ffi void SDL_RenderPresent(pointer) from "SDL2";
            SDL_RenderPresent(renderer);
            print("Rendered red background");
            
            // Wait 3 seconds
            ffi void Sleep(int) from "kernel32";
            Sleep(3000);
            
            print("SDL2 example complete - cleaning up...");
            
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
    print("Failed to initialize SDL2 - this is expected if SDL2.dll is not available");
}

print("=== SDL2 Example Complete ===");