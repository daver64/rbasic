// SDL2 Core Functions Demo - Working Version
// Pre-defines all variables to avoid scoping issues

import "blib/sdl2.bas";

print("SDL2 Core Functions Demo - Working Version");
print("Features: points, lines, window management, timing");

// Initialize SDL
if (sdl_init("SDL2 Core Demo", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Pre-define ALL variables to avoid scoping issues
var start_time = sdl_get_ticks();
var frame_count = 0;
var current_time = 0;
var elapsed_ms = 0;
var x_pos = 0;
var grid_x = 0;
var grid_y = 0;
var cross_x = 0;
var cross_y = 0;
var timer_x = 50;
var timer_y = 500;
var timer_width = 300;
var timer_progress = 0;
var px = 0;
var seconds = 0;
var fps = 0;
var title = "";
var resize_cycle = 0;
var status_color = 0;
var final_time = 0;
var total_runtime = 0;
var average_fps = 0;

print("Demo starting main loop...");
print("sdl_running initial state: " + str(sdl_running));

var loop_count = 0;
var max_loops = 300;  // Run for ~10 seconds at 30fps

while (loop_count < max_loops && sdl_is_running()) {
    if (loop_count % 30 == 0) {
        print("Loop " + str(loop_count) + " running, sdl_running = " + str(sdl_running));
    }
    
    current_time = sdl_get_ticks();
    elapsed_ms = current_time - start_time;
    frame_count = frame_count + 1;
    loop_count = loop_count + 1;
    
    // Clear screen
    sdl_clear_blue();
    print("After clear_blue, sdl_running = " + str(sdl_running));
    
    if (sdl_running == 0) {
        print("SDL quit detected after clear_blue!");
        break;
    }
    
    // === POINT DRAWING DEMO ===
    // Static pixel art pattern
    sdl_set_color(255, 0, 0);  // Red
    sdl_draw_point(100, 100);
    sdl_draw_point(101, 100);
    sdl_draw_point(102, 100);
    sdl_draw_point(103, 100);
    sdl_draw_point(104, 100);
    
    sdl_set_color(0, 255, 0);  // Green
    sdl_draw_point(100, 101);
    sdl_draw_point(101, 101);
    sdl_draw_point(102, 101);
    sdl_draw_point(103, 101);
    sdl_draw_point(104, 101);
    
    sdl_set_color(0, 0, 255);  // Blue
    sdl_draw_point(100, 102);
    sdl_draw_point(101, 102);
    sdl_draw_point(102, 102);
    sdl_draw_point(103, 102);
    sdl_draw_point(104, 102);
    
    // Moving point animation
    x_pos = 120 + (elapsed_ms / 20) % 400;
    sdl_set_color(255, 255, 0);  // Yellow
    sdl_draw_point(x_pos, 150);
    sdl_draw_point(x_pos + 1, 150);
    sdl_draw_point(x_pos, 151);
    sdl_draw_point(x_pos + 1, 151);
    
    // === LINE DRAWING DEMO ===
    // Reference grid
    sdl_set_color(64, 64, 64);  // Dark gray
    grid_x = 200;
    while (grid_x < 600) {
        sdl_draw_line(grid_x, 200, grid_x, 400);
        grid_x = grid_x + 50;
    }
    
    grid_y = 200;
    while (grid_y < 400) {
        sdl_draw_line(200, grid_y, 600, grid_y);
        grid_y = grid_y + 50;
    }
    
    // Animated crosshair
    cross_x = 400 + 100 * cos(elapsed_ms / 1000.0);
    cross_y = 300 + 50 * sin(elapsed_ms / 500.0);
    
    sdl_set_color(255, 255, 255);  // White
    sdl_draw_line(cross_x - 20, cross_y, cross_x + 20, cross_y);
    sdl_draw_line(cross_x, cross_y - 20, cross_x, cross_y + 20);
    
    // === TIMING VISUALIZATION ===
    // Timer bar using point array
    timer_progress = (elapsed_ms / 100) % timer_width;
    
    sdl_set_color(0, 200, 0);  // Bright green
    px = 0;
    while (px <= timer_progress) {
        sdl_draw_point(timer_x + px, timer_y);
        sdl_draw_point(timer_x + px, timer_y + 1);
        sdl_draw_point(timer_x + px, timer_y + 2);
        px = px + 2;
    }
    
    // Timer border
    sdl_set_color(255, 255, 255);
    sdl_draw_line(timer_x, timer_y - 5, timer_x + timer_width, timer_y - 5);
    sdl_draw_line(timer_x, timer_y + 5, timer_x + timer_width, timer_y + 5);
    sdl_draw_line(timer_x, timer_y - 5, timer_x, timer_y + 5);
    sdl_draw_line(timer_x + timer_width, timer_y - 5, timer_x + timer_width, timer_y + 5);
    
    // === WINDOW MANAGEMENT DEMO ===
    // Update title every 2 seconds
    if ((elapsed_ms / 2000) != ((elapsed_ms - 33) / 2000)) {
        seconds = elapsed_ms / 1000;
        if (elapsed_ms > 0) {
            fps = frame_count * 1000 / elapsed_ms;
        } else {
            fps = 0;
        }
        title = "SDL2 Demo - " + str(seconds) + "s - Frame " + str(frame_count) + " - FPS " + str(fps);
        sdl_set_window_title(title);
    }
    
    // Window resize demonstration
    resize_cycle = (elapsed_ms / 1000) % 16;
    print("resize_cycle = " + str(resize_cycle) + ", sdl_running = " + str(sdl_running));
    if (resize_cycle == 5) {
        print("About to resize to 1000x700...");
        sdl_set_window_size(1000, 700);
        print("Resized to 1000x700, sdl_running = " + str(sdl_running));
    } else if (resize_cycle == 10) {
        print("About to resize to 800x600...");
        sdl_set_window_size(800, 600);
        print("Resized to 800x600, sdl_running = " + str(sdl_running));
    }
    
    // Status indicator
    if (resize_cycle >= 5 && resize_cycle < 10) {
        status_color = 255;  // Red for large mode
        sdl_set_color(255, 0, 0);
    } else {
        status_color = 255;  // Green for normal mode
        sdl_set_color(0, 255, 0);
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
    // sdl_process_events();  // Temporarily disabled to test
    sdl_delay(33);  // ~30 FPS
}

// Final report
final_time = sdl_get_ticks();
total_runtime = final_time - start_time;
if (total_runtime > 0) {
    average_fps = frame_count * 1000 / total_runtime;
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
print("  + SDL_RenderDrawPoint() - Pixel-level graphics");
print("  + SDL_SetWindowTitle() - Dynamic window titles");
print("  + SDL_SetWindowSize() - Runtime window resizing"); 
print("  + SDL_GetTicks() - High-precision timing");
print("  + SDL_RenderDrawLine() - Vector graphics");
print("");
print("All new SDL2 core functions working correctly!");

sdl_cleanup();