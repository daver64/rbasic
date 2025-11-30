// SDL2 Basic Window Demo
// Demonstrates SDL2 integration with window creation, rendering, and events
// 
// Build with: cmake -DWITH_SDL2=ON .. && make
// Run interpreted: ./rbasic -i examples/sdl_window_demo.bas
// Run compiled: ./rbasic -c examples/sdl_window_demo.bas -o sdl_window_demo && ./sdl_window_demo
//
// Requirements: SDL2 library installed
// Linux: sudo apt install libsdl2-dev libsdl2-gfx-dev
// macOS: brew install sdl2 sdl2_gfx

// SDL2 constants (defined in rbasic when built with -DWITH_SDL2=ON)
var SDL_INIT_VIDEO = 0x00000020;
var SDL_WINDOW_SHOWN = 0x00000004;
var SDL_RENDERER_ACCELERATED = 0x00000002;
var SDL_RENDERER_PRESENTVSYNC = 0x00000004;
var SDL_WINDOWPOS_CENTERED = 0x2FFF0000;

var SDL_QUIT = 0x100;
var SDL_KEYDOWN = 0x300;
var SDL_KEYUP = 0x301;

print("SDL2 Window Demo");
print("===============");
print("");

// Initialize SDL2
print("Initializing SDL2...");
var result = sdl_init(SDL_INIT_VIDEO);
if (result != 0) {
    print("SDL_Init failed: " + sdl_get_error());
    exit();
}
print("SDL2 initialized successfully");

// Create window
print("Creating window...");
var window = sdl_create_window("rbasic SDL2 Demo", 
                               SDL_WINDOWPOS_CENTERED, 
                               SDL_WINDOWPOS_CENTERED,
                               800, 600,
                               SDL_WINDOW_SHOWN);

if (window < 0) {
    print("Window creation failed: " + sdl_get_error());
    sdl_quit();
    exit();
}
print("Window created (handle: " + str(window) + ")");

// Create renderer
print("Creating renderer...");
var renderer = sdl_create_renderer(window, -1, 
                                   SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC);

if (renderer < 0) {
    print("Renderer creation failed: " + sdl_get_error());
    sdl_destroy_window(window);
    sdl_quit();
    exit();
}
print("Renderer created (handle: " + str(renderer) + ")");
print("");
print("Window is open. Press ESC or close window to exit.");

// Main loop
var running = 1;
var color_r = 0;
var color_direction = 1;

while (running == 1) {
    // Poll events
    while (sdl_poll_event() == 1) {
        var event_type = sdl_get_event_type();
        
        if (event_type == SDL_QUIT) {
            running = 0;
        }
        
        if (event_type == SDL_KEYDOWN) {
            var scancode = sdl_get_key_scancode();
            if (scancode == 41) {  // ESC key
                running = 0;
            }
        }
    }
    
    // Animate background color
    color_r = color_r + color_direction;
    if (color_r >= 255) {
        color_r = 255;
        color_direction = -1;
    }
    if (color_r <= 0) {
        color_r = 0;
        color_direction = 1;
    }
    
    // Clear screen with animated color
    sdl_set_render_draw_color(renderer, color_r, 64, 128, 255);
    sdl_render_clear(renderer);
    
    // Draw some rectangles
    sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl_render_fill_rect(renderer, 100, 100, 200, 150);
    
    sdl_set_render_draw_color(renderer, 255, 0, 0, 255);
    sdl_render_fill_rect(renderer, 500, 100, 200, 150);
    
    sdl_set_render_draw_color(renderer, 0, 255, 0, 255);
    sdl_render_fill_rect(renderer, 300, 350, 200, 150);
    
    // Draw some lines
    sdl_set_render_draw_color(renderer, 255, 255, 0, 255);
    sdl_render_draw_line(renderer, 0, 0, 800, 600);
    sdl_render_draw_line(renderer, 800, 0, 0, 600);
    
    // Present the rendered frame
    sdl_render_present(renderer);
    
    // Small delay to avoid consuming 100% CPU
    sdl_delay(16);  // ~60 FPS
}

// Cleanup
print("");
print("Cleaning up...");
sdl_destroy_renderer(renderer);
sdl_destroy_window(window);
sdl_quit();
print("SDL2 Demo completed successfully");
