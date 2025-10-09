// Simple SDL2 Extended Functions Test
// Tests basic new SDL2 functions without complex loops

import "blib/sdl2.bas";

print("SDL2 Extended Functions Test");

// Initialize SDL
if (sdl_init("SDL2 Extended Test", 400, 300) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Test window title change
sdl_set_window_title("SDL2 Extended Test - Running");

var start_time = sdl_get_ticks();
var frame_count = 0;

// Main rendering loop
while (sdl_is_running()) {
    // Clear screen
    sdl_clear_blue();
    
    // Test point drawing - simple pattern
    sdl_set_color(255, 0, 0); // Red
    sdl_draw_point(100, 100);
    sdl_draw_point(101, 100);
    sdl_draw_point(102, 100);
    
    sdl_set_color(0, 255, 0); // Green  
    sdl_draw_point(100, 101);
    sdl_draw_point(101, 101);
    sdl_draw_point(102, 101);
    
    sdl_set_color(0, 0, 255); // Blue
    sdl_draw_point(100, 102);
    sdl_draw_point(101, 102);
    sdl_draw_point(102, 102);
    
    // Test line drawing
    sdl_set_color(255, 255, 0); // Yellow
    sdl_draw_line(50, 50, 150, 50);
    sdl_draw_line(50, 150, 150, 150);
    sdl_draw_line(50, 50, 50, 150);
    sdl_draw_line(150, 50, 150, 150);
    
    // Test timing
    var current_time = sdl_get_ticks();
    var elapsed = current_time - start_time;
    
    // Update window title with timing info every 60 frames
    frame_count = frame_count + 1;
    if (frame_count % 60 == 0) {
        sdl_set_window_title("SDL2 Test - Frame: " + str(frame_count) + " Time: " + str(elapsed) + "ms");
    }
    
    // Present frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Delay
    sdl_delay(16);
}

print("Extended functions test complete!");
sdl_cleanup();