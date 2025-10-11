// Graphics Colors Module - Rebuilt working version
// Color utilities and management

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
    if (r < 0) {
        r = 0;
    }
    if (r > 255) {
        r = 255;
    }
    if (g < 0) {
        g = 0;
    }
    if (g > 255) {
        g = 255;
    }
    if (b < 0) {
        b = 0;
    }
    if (b > 255) {
        b = 255;
    }
    
    return (r * 65536) + (g * 256) + b;
}

// Create RGBA color value with alpha
function rgba(r as integer, g as integer, b as integer, a as integer) as integer {
    // Clamp values to 0-255 range
    if (r < 0) {
        r = 0;
    }
    if (r > 255) {
        r = 255;
    }
    if (g < 0) {
        g = 0;
    }
    if (g > 255) {
        g = 255;
    }
    if (b < 0) {
        b = 0;
    }
    if (b > 255) {
        b = 255;
    }
    if (a < 0) {
        a = 0;
    }
    if (a > 255) {
        a = 255;
    }
    
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

// Mix two colors with a ratio (0.0 to 1.0)
function mix_colors(color1 as integer, color2 as integer, ratio as double) as integer {
    // Clamp ratio
    if (ratio < 0.0) {
        ratio = 0.0;
    }
    if (ratio > 1.0) {
        ratio = 1.0;
    }
    
    var r1 = get_red(color1);
    var g1 = get_green(color1);
    var b1 = get_blue(color1);
    
    var r2 = get_red(color2);
    var g2 = get_green(color2);
    var b2 = get_blue(color2);
    
    var r = r1 + ((r2 - r1) * ratio);
    var g = g1 + ((g2 - g1) * ratio);
    var b = b1 + ((b2 - b1) * ratio);
    
    return rgb(r, g, b);
}

// Set the current drawing color using RGB
function set_color_rgb(r as integer, g as integer, b as integer) {
    // Clamp values to 0-255 range
    if (r < 0) {
        r = 0;
    }
    if (r > 255) {
        r = 255;
    }
    if (g < 0) {
        g = 0;
    }
    if (g > 255) {
        g = 255;
    }
    if (b < 0) {
        b = 0;
    }
    if (b > 255) {
        b = 255;
    }
    
    SDL_SetRenderDrawColor(graphics_get_renderer(), r, g, b, 255);
}

// Set the current drawing color using a color value
function set_color(color as integer) {
    var r = get_red(color);
    var g = get_green(color);
    var b = get_blue(color);
    var a = get_alpha(color);
    set_color_rgb(r, g, b);
}

// Module information
function graphics_colors_info() {
    print("=== Graphics Colors Module ===");
    print("Features:");
    print("- Color constants (RGB and extended palette)");
    print("- Color utility functions (rgb, rgba, mixing)");
    print("- Component extraction (get_red, get_green, get_blue, get_alpha)");
    print("- Drawing color management");
    print("- Color mixing and blending");
    print("Functions: rgb(), rgba(), mix_colors(), set_color(), set_color_rgb()");
    print("Constants: COLOR_RED, COLOR_GREEN, COLOR_BLUE, etc.");
}

print("Graphics Colors Module loaded successfully!");