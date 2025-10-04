print("Starting SDL graphics test...");
graphics_mode(800, 600);
clear_screen();

// Draw something visible
set_color(255, 0, 0);
draw_rect(100, 100, 200, 150, false);

set_color(0, 255, 0);
draw_rect(150, 150, 100, 100, true);

refresh_screen();

print("Graphics drawn. Running for 100 iterations without quit checks...");

// Fixed iteration loop without quit checks
var loop_count = 0;

while (loop_count < 100) {
    loop_count = loop_count + 1;
    
    if (loop_count % 20 == 0) {
        print("Loop iteration: " + loop_count);
    }
    
    // Just refresh and sleep - no quit checks
    refresh_screen();
    sleep_ms(100);
}

print("Loop completed. Closing...");
text_mode();
print("Exiting graphics mode.");