// Graphics Test - Simple test with blue screen and yellow line
// Press ESCAPE or close window to quit

import "blib/sdl2.bas";

print("Graphics Test - Blue screen with yellow line");
print("Press ESCAPE to quit");

// Initialize SDL
if (sdl_init("Graphics Test", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Main rendering loop
while (sdl_is_running()) {
    // Clear to blue background
    sdl_clear_blue();
    
    // Draw a yellow line
    sdl_draw_coloured_line(100, 100, 700, 500, 255, 255, 0);  // Yellow line
    
    // Present the frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Frame rate control
    sdl_delay(16); // ~60 FPS
}

// Cleanup
sdl_cleanup();
print("Graphics test complete!");
