// Graphics demo for rbasic
// Run with: rbasic -i graphics_demo.bas --io sdl

print("Starting graphics demo...");

// Initialize graphics mode
graphics_mode(800, 600);

// Set drawing color to red
set_color(255, 0, 0);

// Draw some pixels
for(var x = 100; x <= 200; x = x + 1) {
    draw_pixel(x, 100);
}

// Set color to green and draw a line
set_color(0, 255, 0);
draw_line(50, 50, 300, 200);

// Set color to blue and draw some rectangles
set_color(0, 0, 255);
draw_rect(400, 100, 100, 80, false);
draw_rect(500, 200, 60, 60, true);

refresh_screen();

print("Graphics drawn! Press ESC to exit.");

// Simple game loop
while(not quit_requested()) {
    if (key_pressed("ESC")) {
        break;
    }
    
    refresh_screen();
    sleep_ms(16); // ~60 FPS
}

text_mode();
print("Graphics demo completed.");