// Texture Demo - Shows fixed texture rendering with test_texture24.bmp
// Demonstrates texture at different sizes and positions

import "blib/sdl2.bas";
import "blib/graphics/core.bas";
import "blib/graphics/textures.bas";

print("=== Texture Rendering Demo ===");

// Initialize SDL2 
if (sdl_init("Texture Demo - Fixed Rendering", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Initialize graphics module
if (graphics_init(sdl_renderer, 800, 600) == 0) {
    print("Failed to initialize graphics module");
    sdl_cleanup();
    return;
}

// Load the test texture from project root
var test_texture = load_texture("test_pattern24.bmp");

if (is_null(test_texture)) {
    print("ERROR: Could not load test_pattern24.bmp from project root");
    print("Make sure test_pattern24.bmp exists in the project directory");
    graphics_cleanup();
    sdl_cleanup();
    return;
}

print("Texture loaded successfully!");
print("Starting texture demo...");
print("This demo shows:");
print("1. Original size texture (upper left)");
print("2. 128x128 version (as requested)");
print("3. 64x64 small version");
print("4. 256x256 large version");
print("Press ESCAPE to exit");

var frame_count = 0;

// Main demo loop
while (sdl_is_running()) {
    // Clear screen to dark blue
    SDL_SetRenderDrawColor(sdl_renderer, 0, 32, 64, 255);
    SDL_RenderClear(sdl_renderer);
    
    // Demo 1: Draw texture at original size at (10, 10)
    draw_texture(test_texture, 10, 10);
    
    // Demo 2: Draw texture as 128x128 at (200, 10) - the requested size
    draw_texture_sized(test_texture, 200, 10, 128, 128);
    
    // Demo 3: Draw texture as 64x64 at (350, 10) - small version
    draw_texture_sized(test_texture, 350, 10, 64, 64);
    
    // Demo 4: Draw texture as 256x256 at (450, 10) - large version  
    draw_texture_sized(test_texture, 450, 10, 256, 256);
    
    // Demo 5: Draw some differently sized versions vertically
    draw_texture_sized(test_texture, 10, 200, 32, 32);   // Tiny
    draw_texture_sized(test_texture, 60, 200, 96, 96);   // Medium
    draw_texture_sized(test_texture, 180, 200, 160, 160); // Large-ish
    
    // Present the frame
    sdl_present();
    
    // Process events (ESC to exit)
    sdl_process_events();
    
    // Frame rate control (~60 FPS)
    sdl_delay(16);
    
    frame_count = frame_count + 1;
    
    // Print status every 5 seconds
    if (frame_count % 300 == 0) {
        print("Demo running... Frame: " + str(frame_count));
        print("Top row: Original, 128x128, 64x64, 256x256");
        print("Bottom row: 32x32, 96x96, 160x160");
    }
}

print("Texture demo completed!");
print("Total frames rendered: " + str(frame_count));

// Cleanup
destroy_texture(test_texture);
graphics_cleanup();
sdl_cleanup();
print("Texture Demo finished successfully!");