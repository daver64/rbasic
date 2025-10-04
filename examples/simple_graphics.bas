print("Starting graphics test...");
graphics_mode(800, 600);
print("Graphics mode set.");
clear_screen();
print("Screen cleared.");

// Draw some test graphics
set_color(255, 0, 0);
draw_rect(100, 100, 50, 30, false);
refresh_screen();

print("Graphics drawn. Press ESC to exit...");

// Event loop to keep window open
var running = true;
while (running) {
    if (quit_requested()) {
        running = false;
    }
    if (key_pressed("ESC")) {
        running = false;
    }
    sleep_ms(16); // ~60 FPS
    refresh_screen();
}

text_mode();
print("Test complete.");