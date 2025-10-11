import "blib/sdl2.bas";

print("=== SDL Simple Window Test ===");

// Initialize SDL and create window
if (sdl_init("Simple Test", 400, 300) != 1) {
    print("ERROR: SDL initialization failed");
    exit;
}

print("SDL initialized successfully");
print("Window created");

print("Starting main loop - press ESC to quit");

// Main loop
while (sdl_is_running()) {
    // Clear to blue
    sdl_clear_blue();
    
    // Draw a white rectangle in center
    sdl_set_colour(255, 255, 255);
    sdl_fill_rect(150, 100, 100, 100);
    
    // Present frame
    sdl_present();
    
    // Process events (handles ESC key and window close)
    sdl_process_events();
    
    // Small delay for ~60 FPS
    sdl_delay(16);
}

print("Main loop exited");
print("Cleaning up SDL...");
sdl_cleanup();
print("SDL test complete");