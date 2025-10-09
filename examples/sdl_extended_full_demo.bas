// Comprehensive SDL2 Extended Functions Demo
// Showcases all new SDL2 functions: points, window management, and timing

import "blib/sdl2.bas";

print("SDL2 Extended Functions Demo");
print("Demonstrating points, lines, window management, and timing");

// Initialize SDL
if (sdl_init("SDL2 Extended Demo", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

var start_time = sdl_get_ticks();
var frame_count = 0;
var animation_phase = 0;

print("Demo started. Window will resize and title will update dynamically.");

// Main rendering loop
while (sdl_is_running()) {
    // Clear to dark blue background
    sdl_clear_blue();
    
    // Get current time for animations
    var current_time = sdl_get_ticks();
    var elapsed_seconds = (current_time - start_time) / 1000;
    frame_count = frame_count + 1;
    
    // === POINT DRAWING DEMONSTRATION ===
    // Create a colorful pixel art pattern
    sdl_set_color(255, 0, 0); // Red points
    sdl_draw_point(100, 100);
    sdl_draw_point(101, 100);
    sdl_draw_point(102, 100);
    sdl_draw_point(100, 101);
    sdl_draw_point(102, 101);
    sdl_draw_point(100, 102);
    sdl_draw_point(101, 102);
    sdl_draw_point(102, 102);
    
    // Green cross pattern
    sdl_set_color(0, 255, 0);
    sdl_draw_point(121, 100);
    sdl_draw_point(120, 101);
    sdl_draw_point(121, 101);
    sdl_draw_point(122, 101);
    sdl_draw_point(121, 102);
    
    // Blue diamond pattern
    sdl_set_color(0, 0, 255);
    sdl_draw_point(141, 100);
    sdl_draw_point(140, 101);
    sdl_draw_point(141, 101);
    sdl_draw_point(142, 101);
    sdl_draw_point(141, 102);
    
    // === ANIMATED POINT GRID ===
    // Create an animated grid of points that change color over time
    var grid_start_x = 200;
    var grid_start_y = 200;
    var grid_size = 10;
    
    var row = 0;
    while (row < grid_size) {
        var col = 0;
        while (col < grid_size) {
            // Calculate color based on position and time
            var time_offset = (current_time / 100) % 256;
            var r = (row * 25 + time_offset) % 256;
            var g = (col * 25 + time_offset) % 256;
            var b = ((row + col) * 12 + time_offset) % 256;
            
            sdl_set_color(r, g, b);
            sdl_draw_point(grid_start_x + col * 3, grid_start_y + row * 3);
            
            col = col + 1;
        }
        row = row + 1;
    }
    
    // === LINE DRAWING DEMONSTRATION ===
    // Draw a moving crosshair
    var center_x = 500;
    var center_y = 300;
    var crosshair_size = 30;
    
    sdl_set_color(255, 255, 0); // Yellow crosshair
    sdl_draw_line(center_x - crosshair_size, center_y, center_x + crosshair_size, center_y);
    sdl_draw_line(center_x, center_y - crosshair_size, center_x, center_y + crosshair_size);
    
    // Draw animated radiating lines
    var line_angle = 0;
    while (line_angle < 360) {
        var line_length = 50 + (elapsed_seconds * 10) % 50;
        var end_x = center_x + line_length * cos(line_angle * 3.14159 / 180);
        var end_y = center_y + line_length * sin(line_angle * 3.14159 / 180);
        
        // Color based on angle
        var angle_color = (line_angle + current_time / 10) % 360;
        if (angle_color < 120) {
            sdl_set_color(255, 0, 0); // Red
        } else if (angle_color < 240) {
            sdl_set_color(0, 255, 0); // Green
        } else {
            sdl_set_color(0, 0, 255); // Blue
        }
        
        sdl_draw_line(center_x, center_y, end_x, end_y);
        line_angle = line_angle + 45;
    }
    
    // === TIMING BAR VISUALIZATION ===
    // Create a visual timer using points
    var timer_x = 50;
    var timer_y = 450;
    var timer_width = 300;
    var timer_progress = (elapsed_seconds * 20) % timer_width;
    
    sdl_set_color(0, 255, 0); // Green timer bar
    var bar_x = 0;
    while (bar_x < timer_progress) {
        sdl_draw_point(timer_x + bar_x, timer_y);
        sdl_draw_point(timer_x + bar_x, timer_y + 1);
        sdl_draw_point(timer_x + bar_x, timer_y + 2);
        bar_x = bar_x + 2;
    }
    
    // Timer border using lines
    sdl_set_color(255, 255, 255); // White border
    sdl_draw_line(timer_x, timer_y - 5, timer_x + timer_width, timer_y - 5);
    sdl_draw_line(timer_x, timer_y + 5, timer_x + timer_width, timer_y + 5);
    sdl_draw_line(timer_x, timer_y - 5, timer_x, timer_y + 5);
    sdl_draw_line(timer_x + timer_width, timer_y - 5, timer_x + timer_width, timer_y + 5);
    
    // === WINDOW MANAGEMENT DEMONSTRATION ===
    // Update window title with live statistics every 30 frames
    if (frame_count % 30 == 0) {
        var fps = frame_count * 1000 / (current_time - start_time);
        var title = "SDL2 Extended Demo - Frame: " + str(frame_count) + " FPS: " + str(fps) + " Time: " + str(elapsed_seconds) + "s";
        sdl_set_window_title(title);
    }
    
    // Window resize demonstration every 10 seconds
    var resize_cycle = elapsed_seconds % 20;
    if (resize_cycle < 10) {
        // Normal size phase
        if (frame_count % 180 == 90) {
            sdl_set_window_size(800, 600);
            print("Window resized to normal size (800x600)");
        }
    } else {
        // Large size phase
        if (frame_count % 180 == 0) {
            sdl_set_window_size(1000, 700);
            print("Window resized to large size (1000x700)");
        }
    }
    
    // === STATUS DISPLAY ===
    // Use colored points to show current status
    var status_x = 600;
    var status_y = 450;
    
    // Status indicator using colored points
    if (resize_cycle < 10) {
        sdl_set_color(0, 255, 0); // Green - normal mode
    } else {
        sdl_set_color(255, 165, 0); // Orange - resize mode
    }
    
    // Draw status circle using points
    var status_radius = 0;
    while (status_radius < 8) {
        var status_points = 0;
        while (status_points < 16) {
            var point_angle = status_points * 22.5;
            var px = status_x + status_radius * cos(point_angle * 3.14159 / 180);
            var py = status_y + status_radius * sin(point_angle * 3.14159 / 180);
            sdl_draw_point(px, py);
            status_points = status_points + 1;
        }
        status_radius = status_radius + 1;
    }
    
    // Present the frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Control frame rate
    sdl_delay(33); // ~30 FPS for smooth animation
}

// Final statistics
var final_time = sdl_get_ticks();
var total_time = final_time - start_time;
var final_fps = frame_count * 1000 / total_time;

print("SDL2 Extended Demo completed!");
print("Statistics:");
print("  Total frames: " + str(frame_count));
print("  Total time: " + str(total_time) + " milliseconds");
print("  Average FPS: " + str(final_fps));
print("  New SDL2 functions demonstrated:");
print("    - SDL_RenderDrawPoint: Pixel-level drawing");
print("    - SDL_SetWindowTitle: Dynamic window titles");
print("    - SDL_SetWindowSize: Runtime window resizing");
print("    - SDL_GetTicks: High-precision timing");

// Cleanup
sdl_cleanup();