// SDL2_gfx Shapes Demo
// Demonstrates various coloured drawing primitives

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
    
    // Draw coloured lines in different colours
    sdl_draw_coloured_line(50, 50, 150, 100, 255, 0, 0);     // Red line
    sdl_draw_coloured_line(50, 100, 150, 150, 0, 255, 0);    // Green line  
    sdl_draw_coloured_line(50, 150, 150, 200, 255, 255, 0);  // Yellow line
    
    // Draw coloured rectangle outlines
    sdl_draw_coloured_rect(200, 50, 350, 150, 255, 128, 0);  // Orange rectangle
    sdl_draw_coloured_rect(200, 200, 350, 300, 128, 255, 128); // Light green rectangle
    
    // Draw filled coloured rectangles
    sdl_fill_coloured_rect(400, 50, 500, 120, 255, 0, 255);  // Magenta box
    sdl_fill_coloured_rect(400, 150, 500, 220, 0, 255, 255); // Cyan box
    sdl_fill_coloured_rect(400, 250, 500, 320, 255, 128, 128); // Pink box
    
    // Draw existing shapes for comparison
    sdl_fill_circle(600, 100, 30);        // Green circle
    sdl_fill_ellipse(700, 100, 40, 25);   // Purple ellipse
    sdl_fill_triangle(750, 50, 800, 150, 850, 50); // Yellow triangle
    
    // Draw a pattern of small coloured rectangles
    var x = 50;
    var y = 350;
    var colourIndex = 0;
    
    for (var i = 0; i < 10; i = i + 1) {
        var r = 255;
        var g = 0; 
        var b = 0;
        
        if (colourIndex == 1) {
            r = 0; g = 255; b = 0;
        } else if (colourIndex == 2) {
            r = 0; g = 0; b = 255;
        } else if (colourIndex == 3) {
            r = 255; g = 255; b = 0;
        } else if (colourIndex == 4) {
            r = 255; g = 0; b = 255;
        } else if (colourIndex == 5) {
            r = 0; g = 255; b = 255;
        }
        
        sdl_fill_coloured_rect(x, y, x + 60, y + 40, r, g, b);
        
        x = x + 80;
        colourIndex = colourIndex + 1;
        if (colourIndex > 5) {
            colourIndex = 0;
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