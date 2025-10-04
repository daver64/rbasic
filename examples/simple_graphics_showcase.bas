// Simple Graphics Showcase for RBASIC
// Demonstrates all available graphics functions

print("=== RBASIC Graphics Showcase ===");
print("This demo shows pixels, lines, and rectangles.");
print("Press ESC to exit.");
print("");

graphics_mode(800, 600);

var running = 1;
var frame = 0;

while (running) {
    // Clear screen
    clear_screen();
    
    // Demo 1: Colored pixels in a pattern
    set_color(255, 100, 100);
    var i = 0;
    while (i < 50) {
        draw_pixel(100 + i * 2, 100 + i);
        i = i + 1;
    }
    
    // Demo 2: Animated line fan
    set_color(100, 255, 100);
    var angle = 0;
    while (angle < 360) {
        var x1 = 400;
        var y1 = 300;
        var x2 = 400 + 100 * cos(angle + frame) / 100;
        var y2 = 300 + 100 * sin(angle + frame) / 100;
        draw_line(x1, y1, x2, y2);
        angle = angle + 30;
    }
    
    // Demo 3: Bouncing rectangles
    set_color(100, 100, 255);
    var rect_x = 600 + 50 * sin(frame * 2) / 100;
    var rect_y = 200 + 30 * cos(frame * 3) / 100;
    draw_rect(rect_x, rect_y, 60, 40, true);
    
    set_color(255, 255, 100);
    draw_rect(rect_x + 10, rect_y + 10, 40, 20, false);
    
    // Demo 4: Pixel art
    set_color(255, 0, 255);
    var px = 0;
    while (px < 20) {
        var py = 0;
        while (py < 20) {
            if ((px + py + frame / 10) % 3 == 0) {
                draw_pixel(50 + px * 3, 400 + py * 3);
            }
            py = py + 1;
        }
        px = px + 1;
    }
    
    // Demo 5: Color cycling lines
    var line_i = 0;
    while (line_i < 10) {
        var r = (line_i * 25 + frame * 2) % 256;
        var g = (line_i * 35 + frame * 3) % 256;
        var b = (line_i * 45 + frame * 4) % 256;
        set_color(r, g, b);
        draw_line(300, 450 + line_i * 5, 500, 450 + line_i * 5);
        line_i = line_i + 1;
    }
    
    refresh_screen();
    sleep_ms(50);
    frame = frame + 1;
    
    // Check for exit
    if (key_pressed("ESC") or quit_requested()) {
        running = 0;
    }
}

text_mode();
print("Graphics showcase completed!");

// Simple math functions
function sin(degrees) {
    var rad = degrees * 314 / 18000; // approximate conversion to radians * 100
    var x = rad % 628; // mod 2*pi approximation
    if (x > 314) {
        x = x - 628;
    }
    
    // Simple approximation: sin(x) ≈ x for small x
    return x;
}

function cos(degrees) {
    return sin(degrees + 90);
}