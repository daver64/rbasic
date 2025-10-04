// Basic Graphics Test for RBASIC

print("Testing basic graphics functions...");

graphics_mode(800, 600);

// Test 1: Clear and basic pixels
clear_screen();
set_color(255, 0, 0);
draw_pixel(100, 100);
draw_pixel(101, 100);
draw_pixel(102, 100);

// Test 2: Lines
set_color(0, 255, 0);
draw_line(50, 50, 200, 150);
draw_line(200, 50, 50, 150);

// Test 3: Rectangles
set_color(0, 0, 255);
draw_rect(300, 200, 100, 80, false);
draw_rect(420, 220, 60, 40, true);

// Test 4: More colors
set_color(255, 255, 0);
draw_rect(500, 100, 50, 50, true);

set_color(255, 0, 255);
draw_line(600, 300, 700, 400);

refresh_screen();

print("Graphics drawn! Press ESC to exit.");

var running = 1;
while (running) {
    if (key_pressed("ESC") or quit_requested()) {
        running = 0;
    }
    sleep_ms(16);
}

text_mode();
print("Graphics test completed!");