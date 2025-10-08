// SDL2_gfx Circle Demo
// Demonstrates filled circles and triangles using SDL2_gfx

import "blib/sdl2.bas";

// Initialize SDL2
var init_result = sdl_init("Circle Demo", 400, 300);
if (init_result != 0) {
    print("Failed to initialize SDL2");
}

print("SDL2_gfx Circle Demo - Press ESC to exit");

// Main loop
while (sdl_running == 1) {
    // Clear to blue background
    sdl_clear_blue();
    
    // Draw some filled circles at different positions
    sdl_fill_circle(200, 150, 50);  // Center circle
    sdl_fill_circle(100, 100, 30);  // Top-left
    sdl_fill_circle(300, 100, 30);  // Top-right
    sdl_fill_circle(100, 200, 30);  // Bottom-left
    sdl_fill_circle(300, 200, 30);  // Bottom-right
    
    // Draw a filled triangle for comparison
    sdl_fill_triangle(350, 50, 400, 50, 375, 100);
    
    // Present the frame
    sdl_present();
    
    // Process events (escape to quit)
    sdl_process_events();
    
    // Small delay to prevent excessive CPU usage
    sdl_delay(16); // ~60 FPS
}

// Cleanup
sdl_cleanup();
print("Demo completed!");