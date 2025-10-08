// SDL2 Drawing Demo
// Creates a window with a yellow triangle and red rectangle outline

import "blib/sdl2.bas";

function main() {
    print("=== SDL2 Drawing Demo ===");
    print("Shows a yellow triangle and red rectangle");
    print("Press ESCAPE to exit");
    print("");
    
    // Initialize SDL with 800x600 window
    if (sdl_init("SDL2 Drawing Demo", 800, 600) == 0) {
        print("Failed to initialize SDL2 - exiting");
        return;
    }
    
    print("Window created! Drawing shapes...");
    
    // Main event loop
    while (sdl_is_running()) {
        // Process events (checks for quit and escape key)
        sdl_process_events();
        
        // Clear screen to blue
        sdl_clear_blue();
        
        // Draw yellow filled triangle
        sdl_set_color(255, 255, 0); // Yellow
        sdl_fill_triangle(400, 150,  // Top point
                         350, 250,  // Bottom left
                         450, 250); // Bottom right
        
        // Draw red rectangle outline
        sdl_set_color(255, 0, 0); // Red
        sdl_draw_rect(300, 350, 200, 100); // x, y, width, height
        
        // Present the frame
        sdl_present();
        
        // Small delay to prevent excessive CPU usage
        sdl_delay(16); // ~60 FPS
    }
    
    // Cleanup
    print("Cleaning up SDL2...");
    sdl_cleanup();
    print("Drawing demo completed!");
}

// Run the demo
main();