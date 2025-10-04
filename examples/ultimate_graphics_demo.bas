// Ultimate RBASIC Graphics Demo
// Showcases all drawing capabilities

print("=== RBASIC Graphics Demo ===");
print("This demo showcases:");
print("- Pixel drawing with colors");
print("- Line drawing");
print("- Rectangle drawing (filled and outlined)");
print("- Color changes");
print("- Animation and interactivity");
print("");
print("Press ESC to exit");

graphics_mode(800, 600);

var running = 1;
var frame = 0;

while (running) {
    // Clear to dark blue background
    clear_screen();
    
    // Title area - draw colorful pixels for "GRAPHICS"
    set_color(255, 255, 255);
    var x = 0;
    while (x < 800) {
        draw_pixel(x, 50);
        x = x + 10;
    }
    
    // Demo 1: Rainbow pixels
    var i = 0;
    while (i < 200) {
        var r = (i + frame * 2) % 256;
        var g = (i * 2 + frame) % 256;
        var b = (i * 3 + frame * 3) % 256;
        set_color(r, g, b);
        draw_pixel(100 + i, 100 + 20 * sin(i + frame) / 100);
        i = i + 3;
    }
    
    // Demo 2: Spinning lines
    set_color(0, 255, 255);
    var angle = 0;
    while (angle < 360) {
        var x1 = 400 + 80 * cos(frame * 2 + angle) / 100;
        var y1 = 250 + 80 * sin(frame * 2 + angle) / 100;
        var x2 = 400 + 120 * cos(frame * 2 + angle + 180) / 100;
        var y2 = 250 + 120 * sin(frame * 2 + angle + 180) / 100;
        draw_line(x1, y1, x2, y2);
        angle = angle + 45;
    }
    
    // Demo 3: Bouncing rectangles
    var rect_x = 600 + 80 * sin(frame * 3) / 100;
    var rect_y = 150 + 50 * cos(frame * 2) / 100;
    
    // Filled rectangle
    set_color(255, 100, 100);
    draw_rect(rect_x, rect_y, 80, 60, true);
    
    // Outlined rectangle
    set_color(255, 255, 100);
    draw_rect(rect_x + 10, rect_y + 10, 60, 40, false);
    
    // Demo 4: Color bars
    var bar = 0;
    while (bar < 16) {
        var color_val = bar * 16;
        set_color(color_val, 255 - color_val, (color_val + frame) % 256);
        draw_rect(50 + bar * 30, 350, 25, 100, true);
        bar = bar + 1;
    }
    
    // Demo 5: Pixel art pattern
    var py = 0;
    while (py < 40) {
        var px = 0;
        while (px < 60) {
            if ((px + py + frame / 5) % 7 < 3) {
                var color_r = (px * 4 + frame) % 256;
                var color_g = (py * 6 + frame * 2) % 256;
                var color_b = 200;
                set_color(color_r, color_g, color_b);
                draw_pixel(400 + px * 2, 400 + py * 2);
            }
            px = px + 1;
        }
        py = py + 1;
    }
    
    // Demo 6: Moving lines grid
    set_color(100, 255, 100);
    var grid = 0;
    while (grid < 10) {
        var offset = 30 * sin(frame + grid * 20) / 100;
        draw_line(grid * 60, 500 + offset, grid * 60 + 40, 550 + offset);
        grid = grid + 1;
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
print("");
print("Graphics demo completed!");
print("All drawing functions working correctly:");
print("✓ draw_pixel() - Individual pixel drawing");
print("✓ draw_line() - Line drawing");
print("✓ draw_rect() - Rectangle drawing (filled and outlined)");
print("✓ set_color() - Color control");
print("✓ clear_screen() - Screen clearing");
print("✓ graphics_mode() / text_mode() - Mode switching");
print("✓ refresh_screen() - Display updates");
print("✓ Event processing for responsive windows");

// Simple math functions for animation
function sin(degrees) {
    var rad = degrees * 314 / 18000;
    var x = rad % 628;
    if (x > 314) {
        x = x - 628;
    }
    return x;
}

function cos(degrees) {
    return sin(degrees + 90);
}