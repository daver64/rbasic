// Texture Size Test - Demonstrates fixed texture rendering
// Shows texture as 128x128 region in upper left instead of covering whole window

import "blib/sdl2.bas";
import "blib/graphics/core.bas";
import "blib/graphics/textures.bas";

print("=== Texture Size Test ===");

// Initialize SDL2 
if (sdl_init("Texture Size Test", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Initialize graphics module
if (graphics_init(sdl_renderer, 800, 600) == 0) {
    print("Failed to initialize graphics module");
    sdl_cleanup();
    return;
}

// Note: You'll need to put a test image file here
// For testing, create a simple colored square image or use any PNG/JPG file
var test_texture = load_texture("test.png");

if (is_null(test_texture)) {
    print("Could not load test.png - create a test image file or change the filename");
    graphics_cleanup();
    sdl_cleanup();
    return;
}

print("Starting texture size test...");
print("You should see the texture as a 128x128 square in the upper left");
print("Press ESCAPE to exit");

var frame_count = 0;

// Main test loop
while (sdl_is_running()) {
    // Clear screen to dark gray
    SDL_SetRenderDrawColor(sdl_renderer, 64, 64, 64, 255);
    SDL_RenderClear(sdl_renderer);
    
    // Test 1: Draw texture at its original size at position (0, 0)
    draw_texture(test_texture, 0, 0);
    
    // Test 2: Draw texture as 128x128 at position (150, 0) - should be smaller/larger depending on original size
    draw_texture_sized(test_texture, 150, 0, 128, 128);
    
    // Test 3: Draw texture as 64x64 at position (300, 0) - small version
    draw_texture_sized(test_texture, 300, 0, 64, 64);
    
    // Test 4: Draw texture as 256x256 at position (400, 0) - large version
    draw_texture_sized(test_texture, 400, 0, 256, 256);
    
    // Draw labels (would be better with text rendering, but just positions for now)
    // Original size at (0,0), 128x128 at (150,0), 64x64 at (300,0), 256x256 at (400,0)
    
    // Present frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Simple frame rate control
    sdl_delay(16); // ~60 FPS
    
    frame_count = frame_count + 1;
}

print("Texture size test completed!");
print("Total frames rendered: " + str(frame_count));

// Cleanup
destroy_texture(test_texture);
graphics_cleanup();
sdl_cleanup();
print("Texture Size Test finished successfully!");