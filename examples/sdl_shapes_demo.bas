// SDL2 Shapes Demo with SDL2_gfx
// Demonstrates advanced shape drawing with circles, ellipses, and triangles
//
// Build with: cmake -DWITH_SDL2=ON .. && make
// Run interpreted: ./rbasic -i examples/sdl_shapes_demo.bas
// Run compiled: ./rbasic -c examples/sdl_shapes_demo.bas -o sdl_shapes_demo && ./sdl_shapes_demo
//
// Requirements: SDL2 and SDL2_gfx libraries
// Linux: sudo apt install libsdl2-dev libsdl2-gfx-dev
// macOS: brew install sdl2 sdl2_gfx

// SDL2 constants
var SDL_INIT_VIDEO = 0x00000020;
var SDL_WINDOW_SHOWN = 0x00000004;
var SDL_RENDERER_ACCELERATED = 0x00000002;
var SDL_RENDERER_PRESENTVSYNC = 0x00000004;
var SDL_WINDOWPOS_CENTERED = 0x2FFF0000;
var SDL_QUIT = 0x100;
var SDL_KEYDOWN = 0x300;

print("SDL2 Shapes Demo");
print("===============");
print("This demo requires SDL2_gfx for circle and ellipse rendering");
print("");

// Initialize SDL2
var result = sdl_init(SDL_INIT_VIDEO);
if (result != 0) {
    print("SDL_Init failed: " + sdl_get_error());
    exit();
}

// Create window
var window = sdl_create_window("rbasic SDL2 Shapes Demo",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               800, 600,
                               SDL_WINDOW_SHOWN);

if (window < 0) {
    print("Window creation failed: " + sdl_get_error());
    sdl_quit();
    exit();
}

// Create renderer
var renderer = sdl_create_renderer(window, -1,
                                   SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC);

if (renderer < 0) {
    print("Renderer creation failed: " + sdl_get_error());
    sdl_destroy_window(window);
    sdl_quit();
    exit();
}

print("Window created successfully");
print("Press ESC or close window to exit");
print("");

// Animation variables
var running = 1;
var angle = 0;
var radius = 50;
var radius_direction = 1;

// Main loop
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
    
    // Update animation
    angle = angle + 1;
    if (angle >= 360) {
        angle = 0;
    }
    
    radius = radius + radius_direction;
    if (radius >= 80) {
        radius_direction = -1;
    }
    if (radius <= 30) {
        radius_direction = 1;
    }
    
    // Clear screen (dark blue)
    sdl_set_render_draw_color(renderer, 20, 20, 60, 255);
    sdl_render_clear(renderer);
    
    // Draw rectangles (corners)
    sdl_set_render_draw_color(renderer, 255, 100, 100, 255);
    sdl_render_fill_rect(renderer, 10, 10, 100, 80);
    
    sdl_set_render_draw_color(renderer, 100, 255, 100, 255);
    sdl_render_fill_rect(renderer, 690, 10, 100, 80);
    
    sdl_set_render_draw_color(renderer, 100, 100, 255, 255);
    sdl_render_fill_rect(renderer, 10, 510, 100, 80);
    
    sdl_set_render_draw_color(renderer, 255, 255, 100, 255);
    sdl_render_fill_rect(renderer, 690, 510, 100, 80);
    
    // Draw circles (SDL2_gfx required - will be no-ops if not available)
    sdl_set_render_draw_color(renderer, 255, 200, 0, 255);
    sdl_render_fill_circle(renderer, 400, 300, radius);
    
    sdl_set_render_draw_color(renderer, 255, 255, 255, 255);
    sdl_render_draw_circle(renderer, 400, 300, radius + 10);
    
    // Draw animated circles around center
    var num_circles = 8;
    var i = 0;
    while (i < num_circles) {
        var circle_angle = angle + (i * 45);
        var rad = circle_angle * 3.14159 / 180.0;
        
        var cx = 400 + (150 * cos(rad));
        var cy = 300 + (150 * sin(rad));
        
        sdl_set_render_draw_color(renderer, 0, 255, 255, 255);
        sdl_render_fill_circle(renderer, cx, cy, 20);
        
        i = i + 1;
    }
    
    // Draw lines forming a star pattern
    sdl_set_render_draw_color(renderer, 255, 255, 255, 128);
    i = 0;
    while (i < num_circles) {
        var angle1 = angle + (i * 45);
        var rad1 = angle1 * 3.14159 / 180.0;
        var x1 = 400 + (150 * cos(rad1));
        var y1 = 300 + (150 * sin(rad1));
        
        sdl_render_draw_line(renderer, 400, 300, x1, y1);
        i = i + 1;
    }
    
    // Draw rectangles outline only
    sdl_set_render_draw_color(renderer, 255, 0, 255, 255);
    sdl_render_draw_rect(renderer, 200, 150, 100, 80);
    sdl_render_draw_rect(renderer, 500, 150, 100, 80);
    sdl_render_draw_rect(renderer, 200, 370, 100, 80);
    sdl_render_draw_rect(renderer, 500, 370, 100, 80);
    
    // Present the rendered frame
    sdl_render_present(renderer);
    
    // ~60 FPS
    sdl_delay(16);
}

// Cleanup
print("Cleaning up...");
sdl_destroy_renderer(renderer);
sdl_destroy_window(window);
sdl_quit();
print("SDL2 Shapes Demo completed successfully");
