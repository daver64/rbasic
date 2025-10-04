// Graphics demo for rbasic
// Run with: rbasic -i graphics_demo.bas --io sdl

print("Starting graphics demo...");

// Initialize graphics mode
graphics_mode(800, 600);

// Set drawing colour to red
set_colour(255, 0, 0);

// Draw some pixels
for(var x = 100; x <= 200; x = x + 1) {
    draw_pixel(x, 100);
}

// Set colour to green and draw a line
set_colour(0, 255, 0);
draw_line(50, 50, 300, 200);

// Set colour to blue and draw some rectangles
set_colour(0, 0, 255);
draw_rect(400, 100, 100, 80, false);
draw_rect(500, 200, 60, 60, true);

refresh_screen();

print("Graphics drawn! Press ESC to exit.");

// Simple game loop
var running = true;
while(not quit_requested() and running) {
    if (key_pressed("ESC")) {
        running = false;
    }
    
    refresh_screen();
    sleep_ms(16); // ~60 FPS
}

text_mode();
print("Graphics demo completed.");