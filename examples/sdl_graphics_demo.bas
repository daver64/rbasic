// Simple SDL2 Graphics Demo
// Demonstrates basic SDL2 functionality using the blib wrapper

import "blib/sdl2.bas";

function main() {
    print("SDL2 Graphics Demo");
    print("==================");
    
    // Initialize SDL with 800x600 window
    if (!sdl_init("SDL2 Graphics Demo", 800, 600)) {
        print("Failed to initialize SDL2");
        return;
    }
    
    print("Controls:");
    print("- Arrow keys: Move the bouncing ball");
    print("- SPACE: Change ball color");
    print("- ESC: Exit");
    
    // Ball properties
    var ball_x = 400;
    var ball_y = 300;
    var ball_dx = 3;
    var ball_dy = 2;
    var ball_radius = 20;
    var ball_color = 0; // 0=red, 1=green, 2=blue, 3=yellow
    
    // Animation variables
    var time = 0;
    
    // Main loop
    while (sdl_is_running()) {
        var frame_start = sdl_frame_start();
        
        // Handle input
        var key = sdl_get_key();
        if (key == key_escape()) {
            break;
        } else if (key == key_space()) {
            ball_color = (ball_color + 1) % 4;
        } else if (key == key_up()) {
            ball_dy = ball_dy - 1;
        } else if (key == key_down()) {
            ball_dy = ball_dy + 1;
        } else if (key == key_left()) {
            ball_dx = ball_dx - 1;
        } else if (key == key_right()) {
            ball_dx = ball_dx + 1;
        }
        
        // Update ball position
        ball_x = ball_x + ball_dx;
        ball_y = ball_y + ball_dy;
        
        // Bounce off walls
        if (ball_x - ball_radius <= 0 || ball_x + ball_radius >= 800) {
            ball_dx = -ball_dx;
        }
        if (ball_y - ball_radius <= 0 || ball_y + ball_radius >= 600) {
            ball_dy = -ball_dy;
        }
        
        // Keep ball in bounds
        if (ball_x - ball_radius < 0) ball_x = ball_radius;
        if (ball_x + ball_radius > 800) ball_x = 800 - ball_radius;
        if (ball_y - ball_radius < 0) ball_y = ball_radius;
        if (ball_y + ball_radius > 600) ball_y = 600 - ball_radius;
        
        // Clear screen with dark blue background
        sdl_set_color(0, 0, 50);
        sdl_clear();
        
        // Draw border
        sdl_color_white();
        sdl_draw_rect(0, 0, 800, 600);
        
        // Draw grid pattern
        sdl_set_color(20, 20, 70);
        for (var x = 0; x < 800; x = x + 50) {
            sdl_draw_line(x, 0, x, 600);
        }
        for (var y = 0; y < 600; y = y + 50) {
            sdl_draw_line(0, y, 800, y);
        }
        
        // Set ball color
        if (ball_color == 0) {
            sdl_color_red();
        } else if (ball_color == 1) {
            sdl_color_green();
        } else if (ball_color == 2) {
            sdl_color_blue();
        } else {
            sdl_color_yellow();
        }
        
        // Draw ball
        sdl_fill_circle(ball_x, ball_y, ball_radius);
        
        // Draw ball outline
        sdl_color_white();
        sdl_draw_circle(ball_x, ball_y, ball_radius);
        
        // Draw some animated decorations
        time = time + 1;
        sdl_set_color(int_val(sin_val(time * 0.1) * 127) + 128, 
                      int_val(cos_val(time * 0.15) * 127) + 128, 
                      int_val(sin_val(time * 0.2) * 127) + 128);
        
        // Draw rotating lines from corners
        var center_x = 400;
        var center_y = 300;
        var line_length = 100;
        
        for (var i = 0; i < 8; i = i + 1) {
            var angle = (time + i * 45) * 0.01745; // Convert to radians
            var end_x = center_x + int_val(cos_val(angle) * line_length);
            var end_y = center_y + int_val(sin_val(angle) * line_length);
            sdl_draw_line(center_x, center_y, end_x, end_y);
        }
        
        // Draw FPS indicator (simple)
        sdl_color_yellow();
        sdl_fill_rect(10, 10, 5, 5);
        
        // Present the frame
        sdl_present();
        sdl_frame_end(frame_start, 60); // Target 60 FPS
    }
    
    // Cleanup
    sdl_cleanup();
    print("SDL2 demo completed!");
}

main();