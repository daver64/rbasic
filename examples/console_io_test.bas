// Console I/O test for the new IOHandler system
print("Testing console I/O abstraction...");

var name = "rbasic";
print("Hello from", name, "console handler!");

print("Current time:", get_ticks(), "ms");

// Test some console graphics functions (will show placeholder output)
clear_screen();
set_color(255, 0, 0);
draw_pixel(10, 20);
draw_line(0, 0, 100, 100);
refresh_screen();

print("Console I/O test completed!");