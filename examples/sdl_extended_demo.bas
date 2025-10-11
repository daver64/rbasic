// SDL2 Extended Functions Demo
// Tests new SDL2 functions: points, window management, timing

import "blib/sdl2.bas";

print("SDL2 Extended Functions Demo");
print("Testing points, window management, and timing");

// Initialize SDL with initial window size
if (sdl_init("SDL2 Extended Demo", 600, 400) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Test window management - change title
sdl_set_window_title("SDL2 Extended Demo - Running");

var frame_count = 0;
var start_time = sdl_get_ticks();

// Main rendering loop
while (sdl_is_running()) {
    // Clear to dark blue background
    sdl_clear_blue();
    
    // Test point drawing - create a pattern of coloured points
    var time = sdl_get_ticks();
    var offset = (time / 10) % 360; // Rotating offset
    
    // Draw points in a spiral pattern
    for (var point_i = 0; point_i < 100; point_i = point_i + 1) {
        var angle = (point_i * 10 + offset) % 360;
        var radius = 50 + (point_i / 2);
        var x = 300 + radius * cos(angle * 3.14159 / 180);
        var y = 200 + radius * sin(angle * 3.14159 / 180);
        
        // Change colour based on position
        var r = (point_i * 2) % 256;
        var g = (point_i * 3) % 256;  
        var b = (point_i * 5) % 256;
        
        sdl_set_colour(r, g, b);
        sdl_draw_point(x, y);
    }
    
    // Draw some reference lines
    sdl_set_colour(255, 255, 255); // White
    sdl_draw_line(0, 200, 600, 200); // Horizontal center line
    sdl_draw_line(300, 0, 300, 400); // Vertical center line
    
    // Draw timing information using points/lines
    var current_time = sdl_get_ticks();
    var elapsed = current_time - start_time;
    var seconds = elapsed / 1000;
    
    // Draw a simple "timer bar" using points
    sdl_set_colour(0, 255, 0); // Green
    var bar_length = (seconds * 10) % 500; // Bar that grows and resets
    for (var bar_x = 50; bar_x < 50 + bar_length; bar_x = bar_x + 2) {
        sdl_draw_point(bar_x, 50);
        sdl_draw_point(bar_x, 51);
        sdl_draw_point(bar_x, 52);
    }
    
    // Update window title with frame count every 60 frames
    frame_count = frame_count + 1;
    if (frame_count % 60 == 0) {
        var fps = frame_count * 1000 / (current_time - start_time + 1);
        sdl_set_window_title("SDL2 Extended Demo - Frame: " + str(frame_count) + " FPS: " + str(fps));
    }
    
    // Test window resizing every 5 seconds
    if ((seconds % 10) < 5) {
        // Normal size
        if (frame_count % 300 == 100) {
            sdl_set_window_size(600, 400);
        }
    } else {
        // Larger size  
        if (frame_count % 300 == 200) {
            sdl_set_window_size(800, 600);
        }
    }
    
    // Present the frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Control frame rate
    sdl_delay(16); // ~60 FPS
}

// Final timing report
var final_time = sdl_get_ticks();
var total_time = final_time - start_time;
var final_fps = frame_count * 1000 / total_time;

print("Demo completed!");
print("Total frames: " + str(frame_count));
print("Total time: " + str(total_time) + " ms");
print("Average FPS: " + str(final_fps));

// Cleanup
sdl_cleanup();