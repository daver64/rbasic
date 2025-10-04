// Comprehensive graphics test with animation
print("Starting comprehensive graphics test...");
graphics_mode(800, 600);
clear_screen();

// Draw static elements first
set_color(255, 255, 255);
draw_line(0, 0, 799, 0);      // Top border
draw_line(0, 0, 0, 599);      // Left border  
draw_line(799, 0, 799, 599);  // Right border
draw_line(0, 599, 799, 599);  // Bottom border

// Draw some rectangles
set_color(255, 0, 0);
draw_rect(50, 50, 100, 80, false);  // Red outline rectangle

set_color(0, 255, 0);
draw_rect(200, 50, 100, 80, true);  // Green filled rectangle

set_color(0, 0, 255);
draw_rect(350, 50, 100, 80, false); // Blue outline rectangle

refresh_screen();

print("Graphics drawn. Press ESC to exit, any other key to animate...");

// Wait for any key
var input_str = "";
var running = true;
var animate = false;

while (running) {
    if (quit_requested()) {
        running = false;
    }
    if (key_pressed("ESC")) {
        running = false;
    }
    if (key_pressed("SPACE")) {
        animate = true;
    }
    
    if (animate) {
        // Simple animation - moving pixel
        var time = get_ticks();
        var x = 100 + (time / 10) % 600;
        var y = 200 + 50 * sin(time / 500.0);
        
        // Clear animation area
        set_color(0, 0, 0);
        draw_rect(100, 150, 600, 100, true);
        
        // Draw moving pixel
        set_color(255, 255, 0);
        draw_pixel(x, y);
        
        refresh_screen();
    }
    
    sleep_ms(16); // ~60 FPS
}

text_mode();
print("Graphics test complete.");