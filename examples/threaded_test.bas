print("Starting threaded SDL test...");
graphics_mode(800, 600);
clear_screen();

// Draw something
set_color(255, 0, 0);
draw_rect(100, 100, 200, 150, false);

set_color(0, 255, 0);
draw_rect(150, 150, 100, 100, true);

refresh_screen();

print("Graphics drawn. Sleeping for 5 seconds...");
sleep_ms(5000);

print("Sleep finished. Closing graphics...");
text_mode();
print("Done!");