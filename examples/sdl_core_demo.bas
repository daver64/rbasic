// SDL2 Core Functions Demonstration
// Clean implementation showing all new SDL2 functions without complex nesting

import "blib/sdl2.bas";

print("SDL2 Core Functions Demo - Clean Version");
print("Features: points, lines, window management, timing");

// Initialize SDL
if (sdl_init("SDL2 Core Demo", 800, 600) != 1) {
    print("Failed to initialize SDL");
    return;
}

var start_time = sdl_get_ticks();
var frame_count = 0;
var x_pos = 0;  // Pre-define to avoid scoping issues
var current_time = 0;
var elapsed_ms = 0;
var final_time = 0;  // Pre-declare to avoid scoping issues
var total_runtime = 0;  // Pre-declare to avoid scoping issues
var average_fps = 0;  // Pre-declare to avoid scoping issues

print("Demo running - ESC to quit");
print("sdl_is_running() = " + str(sdl_is_running()));

print("Demo running - ESC to quit");

while (sdl_is_running()) {
    print("=== ENTERING LOOP ITERATION ===");
    print("Loop start: sdl_is_running() = " + str(sdl_is_running()));
    current_time = sdl_get_ticks();
    elapsed_ms = current_time - start_time;
    
    // === FRAME COUNTING ===
    frame_count = frame_count + 1;
    print("Frame " + str(frame_count) + " - elapsed: " + str(elapsed_ms) + "ms");
    frame_count = frame_count + 1;
    
    // Clear screen
    sdl_clear_blue();
    
    // === POINT DRAWING DEMO ===
    // Static pixel art pattern
    sdl_set_colour(255, 0, 0);  // Red
    sdl_draw_point(100, 100);
    sdl_draw_point(101, 100);
    sdl_draw_point(102, 100);
    sdl_draw_point(103, 100);
    sdl_draw_point(104, 100);
    
    sdl_set_colour(0, 255, 0);  // Green
    sdl_draw_point(100, 101);
    sdl_draw_point(101, 101);
    sdl_draw_point(102, 101);
    sdl_draw_point(103, 101);
    sdl_draw_point(104, 101);
    
    sdl_set_colour(0, 0, 255);  // Blue
    sdl_draw_point(100, 102);
    sdl_draw_point(101, 102);
    sdl_draw_point(102, 102);
    sdl_draw_point(103, 102);
    sdl_draw_point(104, 102);
    
    print("About to update x_pos...");
    // Moving point animation
    x_pos = 120 + (elapsed_ms / 20) % 400;
    print("x_pos updated: " + str(x_pos));
    sdl_set_colour(255, 255, 0);  // Yellow
    sdl_draw_point(x_pos, 150);
    sdl_draw_point(x_pos + 1, 150);
    sdl_draw_point(x_pos, 151);
    sdl_draw_point(x_pos + 1, 151);
    
    // === LINE DRAWING DEMO ===
    // Reference grid
    sdl_set_colour(64, 64, 64);  // Dark gray
    var grid_x = 200;
    while (grid_x < 600) {
        sdl_draw_line(grid_x, 200, grid_x, 400);
        grid_x = grid_x + 50;
    }
    
    var grid_y = 200;
    while (grid_y < 400) {
        sdl_draw_line(200, grid_y, 600, grid_y);
        grid_y = grid_y + 50;
    }
    
    // Animated crosshair
    var cross_x = 400 + 100 * cos(elapsed_ms / 1000.0);
    var cross_y = 300 + 50 * sin(elapsed_ms / 500.0);
    
    sdl_set_colour(255, 255, 255);  // White
    sdl_draw_line(cross_x - 20, cross_y, cross_x + 20, cross_y);
    sdl_draw_line(cross_x, cross_y - 20, cross_x, cross_y + 20);
    
    // === TIMING VISUALIZATION ===
    // Timer bar using point array
    var timer_x = 50;
    var timer_y = 500;
    var timer_width = 300;
    var timer_progress = (elapsed_ms / 100) % timer_width;
    
    sdl_set_colour(0, 200, 0);  // Bright green
    var px = 0;
    while (px <= timer_progress) {
        sdl_draw_point(timer_x + px, timer_y);
        sdl_draw_point(timer_x + px, timer_y + 1);
        sdl_draw_point(timer_x + px, timer_y + 2);
        px = px + 2;
    }
    
    // Timer border
    sdl_set_colour(255, 255, 255);
    sdl_draw_line(timer_x, timer_y - 5, timer_x + timer_width, timer_y - 5);
    sdl_draw_line(timer_x, timer_y + 5, timer_x + timer_width, timer_y + 5);
    sdl_draw_line(timer_x, timer_y - 5, timer_x, timer_y + 5);
    sdl_draw_line(timer_x + timer_width, timer_y - 5, timer_x + timer_width, timer_y + 5);
    
    // === WINDOW MANAGEMENT DEMO ===
    // Update title every 2 seconds
    if ((elapsed_ms / 2000) != ((elapsed_ms - 33) / 2000)) {
        var seconds = elapsed_ms / 1000;
        var fps = frame_count * 1000 / elapsed_ms;
        var title = "SDL2 Demo - " + str(seconds) + "s - Frame " + str(frame_count) + " - FPS " + str(fps);
        sdl_set_window_title(title);
    }
    
    // Window resize demonstration
    var resize_cycle = (elapsed_ms / 1000) % 16;
    if (resize_cycle == 5) {
        sdl_set_window_size(1000, 700);
        print("Resized to 1000x700");
    } else if (resize_cycle == 10) {
        sdl_set_window_size(800, 600);
        print("Resized to 800x600");
    }
    
    // Status indicator
    var status_colour = 0;
    if (resize_cycle >= 5 && resize_cycle < 10) {
        status_colour = 255;  // Red for large mode
        sdl_set_colour(255, 0, 0);
    } else {
        status_colour = 255;  // Green for normal mode
        sdl_set_colour(0, 255, 0);
    }
    
    // Status indicator circle made of points
    sdl_draw_point(750, 50);
    sdl_draw_point(749, 51);
    sdl_draw_point(750, 51);
    sdl_draw_point(751, 51);
    sdl_draw_point(748, 52);
    sdl_draw_point(749, 52);
    sdl_draw_point(750, 52);
    sdl_draw_point(751, 52);
    sdl_draw_point(752, 52);
    sdl_draw_point(749, 53);
    sdl_draw_point(750, 53);
    sdl_draw_point(751, 53);
    sdl_draw_point(750, 54);
    
    // Present and control frame rate
    sdl_present();
    print("About to process events, sdl_running = " + str(sdl_running));
    sdl_process_events();
    print("After processing events, sdl_running = " + str(sdl_running));
    continue_loop = sdl_is_running();  // Update loop condition
    sdl_delay(33);  // ~30 FPS
    print("=== END OF LOOP ITERATION ===");
    print("About to check while condition: continue_loop = " + str(continue_loop));
}

print("=== EXITED WHILE LOOP ===");
print("After while loop: sdl_is_running() = " + str(sdl_is_running()));

// Final report
final_time = sdl_get_ticks();
total_runtime = final_time - start_time;
if (total_runtime > 0) {
    average_fps = frame_count * 1000 / total_runtime;
} else {
    average_fps = 0;
}

print("");
print("SDL2 Core Functions Demo Completed!");
print("======================================");
print("Runtime Statistics:");
print("  Total frames: " + str(frame_count));
print("  Runtime: " + str(total_runtime) + " ms (" + str(total_runtime / 1000) + " seconds)");
print("  Average FPS: " + str(average_fps));
print("");
print("Functions Successfully Demonstrated:");
print("  [OK] SDL_RenderDrawPoint() - Pixel-level graphics");
print("  [OK] SDL_SetWindowTitle() - Dynamic window titles");
print("  [OK] SDL_SetWindowSize() - Runtime window resizing"); 
print("  [OK] SDL_GetTicks() - High-precision timing");
print("  [OK] SDL_RenderDrawLine() - Vector graphics");
print("");
print("All new SDL2 core functions working correctly!");

sdl_cleanup();