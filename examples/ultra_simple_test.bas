// Ultra Simple SDL2 Test
import "blib/sdl2.bas";

print("Ultra Simple SDL2 Test");

if (sdl_init("Test", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

print("Drawing 5 frames...");

// Use simple for loop instead of while
var i = 0;
for (i = 0; i < 5; i = i + 1) {
    print("Drawing frame " + str(i));
    
    // Clear and draw
    sdl_clear_blue();
    sdl_set_color(255, 0, 0);
    sdl_draw_point(100 + i * 20, 100 + i * 20);
    sdl_present();
    
    // Process events quickly
    sdl_process_events();
    sdl_delay(1000);  // 1 second delay
}

print("Test completed");
sdl_cleanup();