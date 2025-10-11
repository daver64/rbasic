// Graphics Colours Module - Rebuilt working version
// Colour utilities and management

import "blib/graphics/core.bas";

// ========================================
// Colour Constants
// ========================================

// Basic colours (RGB values)
var COLOUR_BLACK = 0x000000;
var COLOUR_WHITE = 0xFFFFFF;
var COLOUR_RED = 0xFF0000;
var COLOUR_GREEN = 0x00FF00;
var COLOUR_BLUE = 0x0000FF;
var COLOUR_YELLOW = 0xFFFF00;
var COLOUR_CYAN = 0x00FFFF;
var COLOUR_MAGENTA = 0xFF00FF;

// Extended colour palette
var COLOUR_DARK_RED = 0x800000;
var COLOUR_DARK_GREEN = 0x008000;
var COLOUR_DARK_BLUE = 0x000080;
var COLOUR_ORANGE = 0xFFA500;
var COLOUR_PURPLE = 0x800080;
var COLOUR_BROWN = 0xA52A2A;
var COLOUR_GRAY = 0x808080;
var COLOUR_LIGHT_GRAY = 0xC0C0C0;
var COLOUR_DARK_GRAY = 0x404040;

// Transparent colour
var COLOUR_TRANSPARENT = 0x00000000;

// ========================================
// Colour Utility Functions
// ========================================

// Create RGB colour value
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

// Create RGBA colour value with alpha
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

// Extract red component from colour
function get_red(colour as integer) as integer {
    return (colour / 65536) % 256;
}

// Extract green component from colour
function get_green(colour as integer) as integer {
    return (colour / 256) % 256;
}

// Extract blue component from colour
function get_blue(colour as integer) as integer {
    return colour % 256;
}

// Extract alpha component from RGBA colour
function get_alpha(colour as integer) as integer {
    return (colour / 16777216) % 256;
}

// Mix two colours with a ratio (0.0 to 1.0)
function mix_colours(colour1 as integer, colour2 as integer, ratio as double) as integer {
    // Clamp ratio
    if (ratio < 0.0) {
        ratio = 0.0;
    }
    if (ratio > 1.0) {
        ratio = 1.0;
    }
    
    var r1 = get_red(colour1);
    var g1 = get_green(colour1);
    var b1 = get_blue(colour1);
    
    var r2 = get_red(colour2);
    var g2 = get_green(colour2);
    var b2 = get_blue(colour2);
    
    var r = r1 + ((r2 - r1) * ratio);
    var g = g1 + ((g2 - g1) * ratio);
    var b = b1 + ((b2 - b1) * ratio);
    
    return rgb(r, g, b);
}

// Set the current drawing colour using RGB
function set_colour_rgb(r as integer, g as integer, b as integer) {
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
    
    SDL_SetRenderDrawColour(graphics_get_renderer(), r, g, b, 255);
}

// Set the current drawing colour using a colour value
function set_colour(colour as integer) {
    var r = get_red(colour);
    var g = get_green(colour);
    var b = get_blue(colour);
    var a = get_alpha(colour);
    set_colour_rgb(r, g, b);
}

// Module information
function graphics_colours_info() {
    print("=== Graphics Colours Module ===");
    print("Features:");
    print("- Colour constants (RGB and extended palette)");
    print("- Colour utility functions (rgb, rgba, mixing)");
    print("- Component extraction (get_red, get_green, get_blue, get_alpha)");
    print("- Drawing colour management");
    print("- Colour mixing and blending");
    print("Functions: rgb(), rgba(), mix_colours(), set_colour(), set_colour_rgb()");
    print("Constants: COLOUR_RED, COLOUR_GREEN, COLOUR_BLUE, etc.");
}

print("Graphics Colours Module loaded successfully!");