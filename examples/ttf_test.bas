print("Testing SDL TTF text rendering...");
graphics_mode(800, 600);
clear_screen();

// Set white color for text
set_color(255, 255, 255);

// The print() calls should now render as proper TTF text in the SDL window
print("Hello from SDL TTF!");
print("This should appear as proper text rendering.");
print("No more white rectangles!");

// Draw some graphics too
set_color(255, 0, 0);
draw_rect(100, 200, 200, 100, false);

set_color(0, 255, 0);
draw_rect(350, 200, 100, 100, true);

refresh_screen();

// Sleep for 5 seconds to see the result
print("Window will stay open for 5 seconds...");
sleep_ms(5000);

text_mode();
print("TTF test completed!");