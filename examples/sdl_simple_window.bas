// Simple SDL2 Window Test
// Uses built-in SDL constants (no manual definitions needed)

print("SDL2 Simple Window Test");
print("======================");
print("");

// Initialize SDL2
print("Initializing SDL2...");
var result = sdl_init(SDL_INIT_VIDEO);
if (result != 0) {
    print("SDL_Init failed: " + sdl_get_error());
} else {
    print("SDL2 initialized successfully");
    
    // Create window
    print("Creating window...");
    var window = sdl_create_window("rbasic SDL2 Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == 0) {
        print("Window creation failed: " + sdl_get_error());
    } else {
        print("Window created successfully (handle: " + str(window) + ")");
        
        // Create renderer
        print("Creating renderer...");
        var renderer = sdl_create_renderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == 0) {
            print("Renderer creation failed: " + sdl_get_error());
        } else {
            print("Renderer created successfully (handle: " + str(renderer) + ")");
            
            // Clear screen to blue
            print("Setting blue background...");
            sdl_set_render_draw_color(renderer, 50, 100, 200, 255);
            sdl_render_clear(renderer);
            sdl_render_present(renderer);
            
            print("");
            print("Window is displayed! Waiting 3 seconds...");
            sdl_delay(3000);
            
            // Cleanup
            print("Cleaning up...");
            sdl_destroy_renderer(renderer);
            sdl_destroy_window(window);
        }
    }
    
    sdl_quit();
    print("SDL2 quit successfully");
}

print("");
print("Test complete!");
