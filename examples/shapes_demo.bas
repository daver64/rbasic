// SDL2_gfx Shapes Demo
// Demonstrates various colored drawing primitives

import "blib/sdl2.bas";

print("SDL2_gfx Shapes Demo");
print("Press ESCAPE to quit");

// Initialize SDL with a larger window for more shapes
if (sdl_init("SDL2_gfx Shapes Demo", 900, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Main rendering loop
while (sdl_is_running()) {
    // Clear to dark blue background
    sdl_clear_blue();
    
    // Draw colored lines in different colors
    sdl_draw_colored_line(50, 50, 150, 100, 255, 0, 0);     // Red line
    sdl_draw_colored_line(50, 100, 150, 150, 0, 255, 0);    // Green line  
    sdl_draw_colored_line(50, 150, 150, 200, 255, 255, 0);  // Yellow line
    
    // Draw colored rectangle outlines
    sdl_draw_colored_rect(200, 50, 350, 150, 255, 128, 0);  // Orange rectangle
    sdl_draw_colored_rect(200, 200, 350, 300, 128, 255, 128); // Light green rectangle
    
    // Draw filled colored rectangles
    sdl_fill_colored_rect(400, 50, 500, 120, 255, 0, 255);  // Magenta box
    sdl_fill_colored_rect(400, 150, 500, 220, 0, 255, 255); // Cyan box
    sdl_fill_colored_rect(400, 250, 500, 320, 255, 128, 128); // Pink box
    
    // Draw existing shapes for comparison
    sdl_fill_circle(600, 100, 30);        // Green circle
    sdl_fill_ellipse(700, 100, 40, 25);   // Purple ellipse
    sdl_fill_triangle(750, 50, 800, 150, 850, 50); // Yellow triangle
    
    // Draw a pattern of small colored rectangles
    var x = 50;
    var y = 350;
    var colorIndex = 0;
    
    for (var i = 0; i < 10; i = i + 1) {
        var r = 255;
        var g = 0; 
        var b = 0;
        
        if (colorIndex == 1) {
            r = 0; g = 255; b = 0;
        } else if (colorIndex == 2) {
            r = 0; g = 0; b = 255;
        } else if (colorIndex == 3) {
            r = 255; g = 255; b = 0;
        } else if (colorIndex == 4) {
            r = 255; g = 0; b = 255;
        } else if (colorIndex == 5) {
            r = 0; g = 255; b = 255;
        }
        
        sdl_fill_colored_rect(x, y, x + 60, y + 40, r, g, b);
        
        x = x + 80;
        colorIndex = colorIndex + 1;
        if (colorIndex > 5) {
            colorIndex = 0;
        }
    }
    
    // Present the frame
    sdl_present();
    
    // Process events
    sdl_process_events();
    
    // Small delay to control frame rate
    sdl_delay(16); // ~60 FPS
}

// Cleanup
sdl_cleanup();
print("Shapes demo complete!");