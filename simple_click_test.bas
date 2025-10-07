// Simple mouse click test - green window, click to close
print("=== Simple Mouse Click Test ===");
print("Click anywhere in the window to close it");

// Declare all FFI functions first
ffi integer SDL_Init(flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateWindow(title as string, x as integer, y as integer, w as integer, h as integer, flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateRenderer(window as pointer, index as integer, flags as integer) from "SDL2.dll";
ffi integer SDL_SetRenderDrawColor(renderer as pointer, r as integer, g as integer, b as integer, a as integer) from "SDL2.dll";
ffi integer SDL_RenderClear(renderer as pointer) from "SDL2.dll";
ffi void SDL_RenderPresent(renderer as pointer) from "SDL2.dll";
ffi integer SDL_PollEvent(event as pointer) from "SDL2.dll";
ffi void SDL_Delay(ms as integer) from "SDL2.dll";
ffi void SDL_DestroyRenderer(renderer as pointer) from "SDL2.dll";
ffi void SDL_DestroyWindow(window as pointer) from "SDL2.dll";
ffi void SDL_Quit() from "SDL2.dll";

// Initialize SDL
var init_result = SDL_Init(SDL_INIT_VIDEO);
print("SDL initialized");

// Create window
var window = SDL_CreateWindow("Click to Close", 100, 100, 400, 300, SDL_WINDOW_SHOWN);
print("Window created");

// Create renderer
var renderer = SDL_CreateRenderer(window, -1, 2);  // SDL_RENDERER_ACCELERATED
print("Renderer created");

// Set green background
SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green
SDL_RenderClear(renderer);
SDL_RenderPresent(renderer);
print("Green background set");

// Create event buffer and start event loop
var event_buffer = create_sdl_event();
print("Starting event loop - click anywhere to close, or press Escape...");

var running = 1;
while (running == 1) {
    var has_event = SDL_PollEvent(event_buffer);
    if (has_event == 1) {
        var event_type = deref_int(event_buffer);  // Use deref_int like working example
        
        // Only print if it's not a null event
        if (event_type != 0) {
            print("Event detected, type: " + event_type);
        }
        
        if (event_type == SDL_QUIT) {
            print("Quit event - closing");
            running = 0;
        } else if (event_type == SDL_MOUSEBUTTONDOWN) {
            print("Mouse click detected - closing window");
            running = 0;
        } else if (event_type == SDL_KEYDOWN) {
            var key_code = get_key_code(event_buffer);
            if (key_code == SDL_SCANCODE_ESCAPE) {
                print("Escape key pressed - closing window");
                running = 0;
            }
        }
    }
    
    // Small delay to prevent 100% CPU usage
    SDL_Delay(10);
}

// Cleanup
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();

print("Test completed successfully!");