var demo_running = 1;
var current_demo = 0;
var frame_count = 0;

function draw_banner(title) {
    // Clear screen with black background
    clear_screen();
    
    // Draw title banner
    set_color(255, 255, 255);
    var x = 0;
    while (x < 800) {
        draw_pixel(x, 10);
        draw_pixel(x, 30);
        x = x + 1;
    }
    
    // Simple text effect - draw title with colored pixels
    var start_x = 50;
    if (title == 1) {
        // "LINES"
        draw_letter_l(start_x, 15);
        draw_letter_i(start_x + 40, 15);
        draw_letter_n(start_x + 60, 15);
        draw_letter_e(start_x + 100, 15);
        draw_letter_s(start_x + 140, 15);
    }
    if (title == 2) {
        // "RECTS" 
        draw_letter_r(start_x, 15);
        draw_letter_e(start_x + 40, 15);
        draw_letter_c(start_x + 80, 15);
        draw_letter_t(start_x + 120, 15);
        draw_letter_s(start_x + 140, 15);
    }
    if (title == 3) {
        // "PIXELS"
        draw_letter_p(start_x, 15);
        draw_letter_i(start_x + 40, 15);
        draw_letter_x(start_x + 60, 15);
        draw_letter_e(start_x + 100, 15);
        draw_letter_l(start_x + 140, 15);
        draw_letter_s(start_x + 160, 15);
    }
}

function draw_letter_l(x, y) {
    set_color(255, 100, 100);
    var i = 0;
    while (i < 10) {
        draw_pixel(x, y + i);
        i = i + 1;
    }
    i = 0;
    while (i < 8) {
        draw_pixel(x + i, y + 9);
        i = i + 1;
    }
}

function draw_letter_i(x, y) {
    set_color(100, 255, 100);
    var i = 0;
    while (i < 10) {
        draw_pixel(x, y + i);
        i = i + 1;
    }
}

function draw_letter_n(x, y) {
    set_color(100, 100, 255);
    var i = 0;
    while (i < 10) {
        draw_pixel(x, y + i);
        draw_pixel(x + 8, y + i);
        if (i > 2 && i < 8) {
            draw_pixel(x + i - 2, y + i);
        }
        i = i + 1;
    }
}

function draw_letter_e(x, y) {
    set_color(255, 255, 100);
    var i = 0;
    while (i < 10) {
        draw_pixel(x, y + i);
        i = i + 1;
    }
    i = 0;
    while (i < 6) {
        draw_pixel(x + i, y);
        draw_pixel(x + i, y + 5);
        draw_pixel(x + i, y + 9);
        i = i + 1;
    }
}

function draw_letter_s(x, y) {
    set_color(255, 100, 255);
    var i = 0;
    while (i < 6) {
        draw_pixel(x + i, y);
        draw_pixel(x + i, y + 5);
        draw_pixel(x + i, y + 9);
        i = i + 1;
    }
    draw_pixel(x, y + 1);
    draw_pixel(x, y + 2);
    draw_pixel(x + 5, y + 6);
    draw_pixel(x + 5, y + 7);
    draw_pixel(x + 5, y + 8);
}

function draw_letter_r(x, y) {
    set_color(255, 200, 100);
    var i = 0;
    while (i < 10) {
        draw_pixel(x, y + i);
        i = i + 1;
    }
    i = 0;
    while (i < 5) {
        draw_pixel(x + i, y);
        draw_pixel(x + i, y + 5);
        i = i + 1;
    }
    draw_pixel(x + 5, y + 1);
    draw_pixel(x + 5, y + 2);
    draw_pixel(x + 5, y + 3);
    draw_pixel(x + 5, y + 4);
    i = 3;
    while (i < 6) {
        draw_pixel(x + i, y + 6 + i - 3);
        i = i + 1;
    }
}

function draw_letter_c(x, y) {
    set_color(100, 255, 200);
    var i = 1;
    while (i < 9) {
        draw_pixel(x, y + i);
        i = i + 1;
    }
    i = 1;
    while (i < 6) {
        draw_pixel(x + i, y);
        draw_pixel(x + i, y + 9);
        i = i + 1;
    }
}

function draw_letter_t(x, y) {
    set_color(200, 100, 255);
    var i = 0;
    while (i < 8) {
        draw_pixel(x + i, y);
        i = i + 1;
    }
    i = 1;
    while (i < 10) {
        draw_pixel(x + 4, y + i);
        i = i + 1;
    }
}

function draw_letter_p(x, y) {
    set_color(255, 150, 150);
    var i = 0;
    while (i < 10) {
        draw_pixel(x, y + i);
        i = i + 1;
    }
    i = 0;
    while (i < 5) {
        draw_pixel(x + i, y);
        draw_pixel(x + i, y + 5);
        i = i + 1;
    }
    draw_pixel(x + 5, y + 1);
    draw_pixel(x + 5, y + 2);
    draw_pixel(x + 5, y + 3);
    draw_pixel(x + 5, y + 4);
}

function draw_letter_x(x, y) {
    set_color(150, 255, 150);
    var i = 0;
    while (i < 10) {
        draw_pixel(x + i / 2, y + i);
        draw_pixel(x + 4 - i / 2, y + i);
        i = i + 2;
    }
}

function demo_lines() {
    draw_banner(1);
    
    // Draw colorful lines
    var angle = frame_count * 5;
    var i = 0;
    while (i < 12) {
        var x1 = 400 + 150 * cos(angle + i * 30) / 100;
        var y1 = 300 + 150 * sin(angle + i * 30) / 100;
        var x2 = 400 + 100 * cos(angle + i * 30 + 180) / 100;
        var y2 = 300 + 100 * sin(angle + i * 30 + 180) / 100;
        
        var r = (i * 20 + frame_count * 2) % 256;
        var g = (i * 30 + frame_count * 3) % 256;
        var b = (i * 40 + frame_count * 4) % 256;
        
        set_color(r, g, b);
        draw_line(x1, y1, x2, y2);
        i = i + 1;
    }
    
    // Draw grid pattern
    i = 0;
    while (i < 20) {
        var x = 50 + i * 35;
        var y = 450;
        var height = 100 + 50 * sin(frame_count * 3 + i * 20) / 100;
        set_color(255 - i * 12, i * 12, 128);
        draw_line(x, y, x, y + height);
        i = i + 1;
    }
}

function demo_rectangles() {
    draw_banner(2);
    
    // Animated rectangles
    var i = 0;
    while (i < 8) {
        var size = 20 + 30 * sin(frame_count * 2 + i * 45) / 100;
        var x = 100 + i * 80;
        var y = 200 + 50 * cos(frame_count * 3 + i * 30) / 100;
        
        var r = (i * 32 + frame_count) % 256;
        var g = (i * 48 + frame_count * 2) % 256;
        var b = (i * 64 + frame_count * 3) % 256;
        
        set_color(r, g, b);
        draw_rect(x - size / 2, y - size / 2, size, size, true);
        i = i + 1;
    }
    
    // Concentric rectangles
    i = 1;
    while (i < 8) {
        var size = i * 25;
        var alpha = (i + frame_count / 5) % 8;
        var intensity = 255 * alpha / 8;
        set_color(intensity, intensity / 2, 255 - intensity);
        draw_rect(400 - size / 2, 400 - size / 2, size, size, false);
        i = i + 1;
    }
}

function demo_pixels() {
    draw_banner(3);
    
    // Pixel art mandala
    var y = 100;
    while (y < 500) {
        var x = 100;
        while (x < 700) {
            var dx = x - 400;
            var dy = y - 300;
            var dist = sqrt(dx * dx + dy * dy);
            var angle = atan2(dy, dx) * 180 / 314;
            
            var pattern = sin(dist / 5 + frame_count) * sin(angle * 8 + frame_count * 2);
            if (pattern > 0) {
                var r = (dist + frame_count) % 256;
                var g = (angle + frame_count * 2) % 256;
                var b = (pattern * 128 + 128) % 256;
                set_color(r, g, b);
                draw_pixel(x, y);
            }
            x = x + 4;
        }
        y = y + 4;
    }
}

// Simple math functions (approximations)
function sin(degrees) {
    // Convert to approximate radians and use Taylor series approximation
    var rad = degrees * 314 / 18000; // degrees * pi / 180, scaled
    var x = rad % 628; // mod 2*pi approximation
    if (x > 314) {
        x = x - 628;
    }
    
    // Taylor series: sin(x) ≈ x - x³/6 + x⁵/120
    var x2 = x * x / 10000;
    var x3 = x2 * x / 100;
    var x5 = x3 * x2 / 100;
    
    return x - x3 / 6 + x5 / 120;
}

function cos(degrees) {
    return sin(degrees + 90);
}

function sqrt(x) {
    if (x <= 0) return 0;
    var guess = x / 2;
    var i = 0;
    while (i < 10) {
        guess = (guess + x / guess) / 2;
        i = i + 1;
    }
    return guess;
}

function atan2(y, x) {
    if (x == 0) {
        if (y > 0) return 90;
        if (y < 0) return -90;
        return 0;
    }
    
    var ratio = y * 100 / x;
    var result = ratio / (1 + ratio * ratio / 10000) * 45; // rough approximation
    
    if (x < 0) {
        if (y >= 0) result = result + 180;
        else result = result - 180;
    }
    
    return result;
}

// Main program
print("=== RBASIC Graphics Showcase ===");
print("This demo shows lines, rectangles, and pixel effects.");
print("Press ESC to exit or wait for automatic progression.");
print("");

graphics_mode(800, 600);

while (demo_running) {
    if (current_demo == 0) {
        demo_lines();
    } else if (current_demo == 1) {
        demo_rectangles();
    } else {
        demo_pixels();
    }
    
    refresh_screen();
    sleep_ms(50);
    frame_count = frame_count + 1;
    
    // Auto-progress every 200 frames (10 seconds)
    if (frame_count % 200 == 0) {
        current_demo = current_demo + 1;
        if (current_demo > 2) {
            current_demo = 0;
        }
        frame_count = 0;
    }
    
    // Check for exit
    if (key_pressed("ESC") || quit_requested()) {
        demo_running = 0;
    }
}

text_mode();
print("Graphics showcase completed!");