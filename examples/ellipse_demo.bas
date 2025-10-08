// SDL2_gfx Ellipse Demo - Test the filled ellipse function
// This demo shows ellipses drawn with the new SDL2_gfx function

import "blib/sdl2.bas";

// Main program
print("SDL2_gfx Ellipse Demo");
print("Press ESCAPE to quit");

// Initialize SDL2 graphics
var success = sdl_init("Ellipse Demo", 800, 600);
if (success == 0) {
    print("Failed to initialize SDL2");
    exit;
}

// Main loop
while (sdl_is_running() == 1) {
    // Clear screen to blue
    sdl_clear_blue();
    
    // Draw multiple ellipses with different sizes and positions
    // sdl_fill_ellipse(x, y, rx, ry) - rx=horizontal radius, ry=vertical radius
    
    // Large ellipse in center (purple)
    sdl_fill_ellipse(400, 300, 150, 80);
    
    // Medium ellipse in top-left (purple)
    sdl_fill_ellipse(200, 150, 80, 60);
    
    // Small wide ellipse in bottom-right (purple)  
    sdl_fill_ellipse(600, 450, 100, 40);
    
    // Tall narrow ellipse on left side (purple)
    sdl_fill_ellipse(100, 300, 30, 120);
    
    // Medium ellipse in top-right (purple)
    sdl_fill_ellipse(650, 150, 60, 90);
    
    // Present the frame
    sdl_present();
    
    // Process events (check for escape key or window close)
    sdl_process_events();
    
    // Small delay to prevent excessive CPU usage
    sdl_delay(16); // ~60 FPS
}

// Cleanup
sdl_cleanup();
print("Ellipse demo complete!");