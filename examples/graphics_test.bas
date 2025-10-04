// Test graphics functionality
var x = 0;
var y = 0;

print("Testing graphics functions...");
graphics_mode(800, 600);
clear_screen();

// Draw some test graphics
set_color(255, 0, 0);
draw_pixel(100, 100);

set_color(0, 255, 0);
draw_line(0, 0, 100, 100);

set_color(0, 0, 255);
draw_rect(200, 200, 50, 30, false);

set_color(255, 255, 0);
draw_rect(300, 300, 40, 40, true);

refresh_screen();

print("Graphics drawn! Check the display.");
print("Press any key to continue...");

// Wait for user input
var dummy = "";
input(dummy);

text_mode();
print("Returned to text mode.");