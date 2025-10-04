print("Interactive SDL graphics test");
graphics_mode(800, 600);
clear_screen();

// Draw initial graphics
set_color(255, 255, 255);
draw_rect(10, 10, 780, 580, false); // White border

set_color(255, 0, 0);
draw_rect(100, 100, 100, 100, false);

set_color(0, 255, 0);
draw_rect(250, 100, 100, 100, true);

set_color(0, 0, 255);
draw_rect(400, 100, 100, 100, false);

refresh_screen();

print("Graphics drawn. Press ESC to exit or close the window.");
print("The window should stay open and responsive!");

// Interactive event loop
var running = 1;
while (running) {
    if (quit_requested()) {
        print("Window close requested!");
        running = 0;
    }
    if (key_pressed("ESC")) {
        print("ESC key pressed!");
        running = 0;
    }
    
    sleep_ms(100); // Check every 100ms
}

text_mode();
print("Graphics test completed!");