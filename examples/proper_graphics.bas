print("Starting SDL graphics test...");
graphics_mode(800, 600);
clear_screen();

// Draw something visible
set_color(255, 0, 0);
draw_rect(100, 100, 200, 150, false);

set_color(0, 255, 0);
draw_rect(150, 150, 100, 100, true);

refresh_screen();

print("Graphics drawn. Use the window - press ESC or close window to exit.");

// Proper event loop
var running = true;
while (running) {
    // Check for quit events
    if (quit_requested()) {
        running = false;
    }
    if (key_pressed("ESC")) {
        running = false;
    }
    
    // Refresh the screen to process events
    refresh_screen();
    
    // Small delay to avoid consuming too much CPU
    sleep_ms(16); // ~60 FPS
}

text_mode();
print("Exiting graphics mode.");