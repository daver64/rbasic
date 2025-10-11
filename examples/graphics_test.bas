// Graphics Module Test - Demonstrates the new graphics system
// Tests texture loading, coordinate systems, and color management

import "blib/sdl2.bas";
import "blib/graphics/core.bas";
import "blib/graphics/textures.bas";
import "blib/graphics/colors.bas";

print("=== Graphics Module Test ===");

// Initialize SDL2 first
if (sdl_init("Graphics Module Test", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Initialize graphics module
if (graphics_init(sdl_renderer, 800, 600) == 0) {
    print("Failed to initialize graphics module");
    sdl_cleanup();
    return;
}

// Show module information
graphics_core_info();
graphics_textures_info();
graphics_colors_info();

var frame_count = 0;
var test_phase = 0; // 0 = coordinate system demo, 1 = color demo

print("Starting graphics test...");
print("Press ESCAPE to exit, any other key to switch test phase");

// Main test loop
while (sdl_is_running()) {
    // Clear screen
    set_black();
    SDL_RenderClear(sdl_renderer);
    
    if (test_phase == 0) {
        // Test coordinate systems and basic drawing
        
        // Draw coordinate system indicators
        set_red();
        graphics_draw_point(0, 0); // Origin
        
        set_green();
        graphics_draw_line(0, 0, 100, 0); // X axis
        
        set_blue();
        graphics_draw_line(0, 0, 0, 100); // Y axis
        
        // Draw some test points
        set_yellow();
        graphics_draw_point(100, 100);
        graphics_draw_point(200, 150);
        graphics_draw_point(300, 200);
        
        // Draw test lines
        set_cyan();
        graphics_draw_line(50, 50, 150, 100);
        graphics_draw_line(200, 50, 300, 150);
        
        // Draw coordinate system info
        set_white();
        var coord_text = "Coordinate System: " + (graphics_get_coordinate_system() == 0 ? "Screen" : "Cartesian");
        // Note: Text rendering will be added in fonts module
        
    } else if (test_phase == 1) {
        // Test color system
        
        // Draw rainbow gradient using HSV
        var x = 0;
        while (x < 800) {
            var hue = (x / 800.0) * 360.0;
            var color = hsv_to_rgb(hue, 1.0, 1.0);
            set_color_packed(color);
            graphics_draw_line(x, 100, x, 200);
            x = x + 2;
        }
        
        // Draw color mixing demo
        var time = sdl_get_ticks() / 1000.0;
        var mix_ratio = (sin(time) + 1.0) / 2.0; // 0.0 to 1.0
        var mixed_color = mix_colors(COLOR_RED, COLOR_BLUE, mix_ratio);
        set_color_packed(mixed_color);
        
        // Draw some shapes with mixed color
        var y = 250;
        while (y < 350) {
            graphics_draw_line(200, y, 600, y);
            y = y + 5;
        }
        
        // Draw brightness demo
        var brightness_x = 100;
        var brightness_val = -1.0;
        while (brightness_x < 700) {
            var bright_color = adjust_brightness(COLOR_GREEN, brightness_val);
            set_color_packed(bright_color);
            graphics_draw_line(brightness_x, 400, brightness_x, 500);
            brightness_x = brightness_x + 2;
            brightness_val = brightness_val + (2.0 / 600.0); // -1.0 to 1.0
        }
    }
    
    // Present frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Simple frame rate control
    sdl_delay(16); // ~60 FPS
    
    frame_count = frame_count + 1;
    
    // Switch test phase every 300 frames (5 seconds)
    if (frame_count % 300 == 0) {
        test_phase = 1 - test_phase; // Toggle between 0 and 1
        print("Switching to test phase: " + str(test_phase));
        
        // Switch coordinate system for demo
        graphics_set_coordinate_system(1 - graphics_get_coordinate_system());
    }
}

print("Graphics test completed!");
print("Total frames rendered: " + str(frame_count));

// Cleanup
graphics_cleanup();
sdl_cleanup();
print("Graphics Module Test finished successfully!");