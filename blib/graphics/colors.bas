// Graphics Colors Module - Color utilities and management
// Provides color constants, mixing, and conversion functions

import "blib/graphics/core.bas";

// ========================================
// Color Constants
// ========================================

// Basic colors (RGB values)
var COLOR_BLACK = 0x000000;
var COLOR_WHITE = 0xFFFFFF;
var COLOR_RED = 0xFF0000;
var COLOR_GREEN = 0x00FF00;
var COLOR_BLUE = 0x0000FF;
var COLOR_YELLOW = 0xFFFF00;
var COLOR_CYAN = 0x00FFFF;
var COLOR_MAGENTA = 0xFF00FF;

// Extended color palette
var COLOR_DARK_RED = 0x800000;
var COLOR_DARK_GREEN = 0x008000;
var COLOR_DARK_BLUE = 0x000080;
var COLOR_ORANGE = 0xFFA500;
var COLOR_PURPLE = 0x800080;
var COLOR_BROWN = 0xA52A2A;
var COLOR_GRAY = 0x808080;
var COLOR_LIGHT_GRAY = 0xC0C0C0;
var COLOR_DARK_GRAY = 0x404040;

// Transparent color
var COLOR_TRANSPARENT = 0x00000000;

// ========================================
// Color Utility Functions
// ========================================

// Create RGB color value
function rgb(r as integer, g as integer, b as integer) as integer {
    // Clamp values to 0-255 range
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    
    return (r * 65536) + (g * 256) + b;
}

// Create RGBA color value with alpha
function rgba(r as integer, g as integer, b as integer, a as integer) as integer {
    // Clamp values to 0-255 range
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    if (a < 0) a = 0; if (a > 255) a = 255;
    
    return (a * 16777216) + (r * 65536) + (g * 256) + b;
}

// Extract red component from color
function get_red(color as integer) as integer {
    return (color / 65536) % 256;
}

// Extract green component from color
function get_green(color as integer) as integer {
    return (color / 256) % 256;
}

// Extract blue component from color
function get_blue(color as integer) as integer {
    return color % 256;
}

// Extract alpha component from RGBA color
function get_alpha(color as integer) as integer {
    return (color / 16777216) % 256;
}

// ========================================
// Color Setting Functions
// ========================================

// Set current drawing color using RGB values
function set_color(r as integer, g as integer, b as integer) {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    // Clamp values
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    
    SDL_SetRenderDrawColor(graphics_get_renderer(), r, g, b, 255);
}

// Set current drawing color using RGBA values
function set_color_rgba(r as integer, g as integer, b as integer, a as integer) {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    // Clamp values
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    if (a < 0) a = 0; if (a > 255) a = 255;
    
    SDL_SetRenderDrawColor(graphics_get_renderer(), r, g, b, a);
}

// Set color using packed color value
function set_color_packed(color as integer) {
    var r = get_red(color);
    var g = get_green(color);
    var b = get_blue(color);
    set_color(r, g, b);
}

// Set color using packed RGBA value
function set_color_packed_rgba(color as integer) {
    var r = get_red(color);
    var g = get_green(color);
    var b = get_blue(color);
    var a = get_alpha(color);
    set_color_rgba(r, g, b, a);
}

// ========================================
// Color Mixing and Manipulation
// ========================================

// Mix two colors with specified ratio (0.0 = first color, 1.0 = second color)
function mix_colors(color1 as integer, color2 as integer, ratio as double) as integer {
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;
    
    var r1 = get_red(color1);
    var g1 = get_green(color1);
    var b1 = get_blue(color1);
    
    var r2 = get_red(color2);
    var g2 = get_green(color2);
    var b2 = get_blue(color2);
    
    var r = r1 + (r2 - r1) * ratio;
    var g = g1 + (g2 - g1) * ratio;
    var b = b1 + (b2 - b1) * ratio;
    
    return rgb(r, g, b);
}

// Lighten a color by specified amount (0.0 = no change, 1.0 = white)
function lighten_color(color as integer, amount as double) as integer {
    return mix_colors(color, COLOR_WHITE, amount);
}

// Darken a color by specified amount (0.0 = no change, 1.0 = black)
function darken_color(color as integer, amount as double) as integer {
    return mix_colors(color, COLOR_BLACK, amount);
}

// Adjust color brightness (-1.0 = black, 0.0 = no change, 1.0 = white)
function adjust_brightness(color as integer, brightness as double) as integer {
    if (brightness < 0.0) {
        return darken_color(color, -brightness);
    } else {
        return lighten_color(color, brightness);
    }
}

// ========================================
// Color Conversion Functions
// ========================================

// Convert HSV to RGB (Hue 0-360, Saturation 0-1, Value 0-1)
function hsv_to_rgb(h as double, s as double, v as double) as integer {
    // Clamp values
    if (h < 0.0) h = 0.0; if (h >= 360.0) h = h - 360.0;
    if (s < 0.0) s = 0.0; if (s > 1.0) s = 1.0;
    if (v < 0.0) v = 0.0; if (v > 1.0) v = 1.0;
    
    var c = v * s;
    var x = c * (1.0 - abs((h / 60.0) % 2.0 - 1.0));
    var m = v - c;
    
    var r = 0.0;
    var g = 0.0;
    var b = 0.0;
    
    if (h < 60.0) {
        r = c; g = x; b = 0.0;
    } else if (h < 120.0) {
        r = x; g = c; b = 0.0;
    } else if (h < 180.0) {
        r = 0.0; g = c; b = x;
    } else if (h < 240.0) {
        r = 0.0; g = x; b = c;
    } else if (h < 300.0) {
        r = x; g = 0.0; b = c;
    } else {
        r = c; g = 0.0; b = x;
    }
    
    var red = (r + m) * 255.0;
    var green = (g + m) * 255.0;
    var blue = (b + m) * 255.0;
    
    return rgb(red, green, blue);
}

// ========================================
// Predefined Color Functions
// ========================================

// Set common colors quickly
function set_black() { set_color(0, 0, 0); }
function set_white() { set_color(255, 255, 255); }
function set_red() { set_color(255, 0, 0); }
function set_green() { set_color(0, 255, 0); }
function set_blue() { set_color(0, 0, 255); }
function set_yellow() { set_color(255, 255, 0); }
function set_cyan() { set_color(0, 255, 255); }
function set_magenta() { set_color(255, 0, 255); }
function set_gray() { set_color(128, 128, 128); }

// ========================================
// Module Information
// ========================================

function graphics_colors_info() {
    print("=== Graphics Colors Module ===");
    print("Features:");
    print("- RGB and RGBA color creation");
    print("- Color component extraction");
    print("- Color mixing and brightness adjustment");
    print("- HSV to RGB conversion");
    print("- Predefined color constants and functions");
    print("");
    print("Color constants: COLOR_BLACK, COLOR_WHITE, COLOR_RED, etc.");
    print("Functions: rgb(), rgba(), set_color(), mix_colors()");
    print("Quick colors: set_red(), set_blue(), set_green(), etc.");
}