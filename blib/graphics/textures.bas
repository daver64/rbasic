// Graphics Textures Module - Image loading, texture management, and basic texture operations
// Provides texture loading from files and texture rendering capabilities

import "blib/graphics/core.bas";

// ========================================
// Texture Management State
// ========================================

var loaded_textures = 0; // Array to track loaded textures (will need array support)
var texture_count = 0;

// ========================================
// Texture Loading Functions
// ========================================

// Load a texture from file (PNG, JPG, WEBP supported)
function load_texture(filename as string) as pointer {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics system not initialized. Call graphics_init() first.");
        return 0;
    }
    
    print("Loading texture: " + filename);
    
    // Use SDL2_image to load texture directly
    var texture = IMG_LoadTexture(graphics_get_renderer(), filename);
    
    if (is_null(texture)) {
        print("ERROR: Failed to load texture: " + filename);
        return 0;
    }
    
    print("Texture loaded successfully: " + filename);
    texture_count = texture_count + 1;
    
    return texture;
}

// Load texture and get its dimensions
function load_texture_with_info(filename as string, width_ptr as pointer, height_ptr as pointer) as pointer {
    var texture = load_texture(filename);
    
    if (not_null(texture)) {
        // Query texture dimensions
        SDL_QueryTexture(texture, 0, 0, width_ptr, height_ptr);
    }
    
    return texture;
}

// Create a blank texture for rendering targets
function create_blank_texture(width as integer, height as integer) as pointer {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics system not initialized");
        return 0;
    }
    
    var texture = SDL_CreateTexture(graphics_get_renderer(), 
                                   get_constant("SDL_PIXELFORMAT_RGBA8888"),
                                   get_constant("SDL_TEXTUREACCESS_TARGET"),
                                   width, height);
    
    if (is_null(texture)) {
        print("ERROR: Failed to create blank texture");
        return 0;
    }
    
    texture_count = texture_count + 1;
    return texture;
}

// ========================================
// Texture Rendering Functions
// ========================================

// Draw entire texture at world coordinates
function draw_texture(texture as pointer, x as double, y as double) {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    if (is_null(texture)) {
        print("ERROR: Cannot draw null texture");
        return;
    }
    
    // Get texture dimensions
    var width_buffer = alloc_int_buffer();
    var height_buffer = alloc_int_buffer();
    SDL_QueryTexture(texture, 0, 0, width_buffer, height_buffer);
    
    var width = deref_int(width_buffer);
    var height = deref_int(height_buffer);
    
    // Convert to screen coordinates
    var screen_x = graphics_world_to_screen_x(x);
    var screen_y = graphics_world_to_screen_y(y);
    
    // In cartesian mode, adjust for texture height
    if (graphics_get_coordinate_system() == 1) {
        screen_y = screen_y - height;
    }
    
    // Create destination rectangle
    var dest_rect = create_sdl_rect();
    set_rect(dest_rect, screen_x, screen_y, width, height);
    
    // Render texture
    SDL_RenderCopy(graphics_get_renderer(), texture, 0, dest_rect);
}

// Draw texture with custom size at world coordinates
function draw_texture_scaled(texture as pointer, x as double, y as double, width as integer, height as integer) {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    if (is_null(texture)) {
        print("ERROR: Cannot draw null texture");
        return;
    }
    
    // Convert to screen coordinates
    var screen_x = graphics_world_to_screen_x(x);
    var screen_y = graphics_world_to_screen_y(y);
    
    // In cartesian mode, adjust for texture height
    if (graphics_get_coordinate_system() == 1) {
        screen_y = screen_y - height;
    }
    
    // Create destination rectangle
    var dest_rect = create_sdl_rect();
    set_rect(dest_rect, screen_x, screen_y, width, height);
    
    // Render scaled texture
    SDL_RenderCopy(graphics_get_renderer(), texture, 0, dest_rect);
}

// Draw portion of texture (texture atlas support)
function draw_texture_region(texture as pointer, 
                            src_x as integer, src_y as integer, src_w as integer, src_h as integer,
                            dest_x as double, dest_y as double, dest_w as integer, dest_h as integer) {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    if (is_null(texture)) {
        print("ERROR: Cannot draw null texture");
        return;
    }
    
    // Create source rectangle
    var src_rect = create_sdl_rect();
    set_rect(src_rect, src_x, src_y, src_w, src_h);
    
    // Convert destination to screen coordinates
    var screen_x = graphics_world_to_screen_x(dest_x);
    var screen_y = graphics_world_to_screen_y(dest_y);
    
    // In cartesian mode, adjust for texture height
    if (graphics_get_coordinate_system() == 1) {
        screen_y = screen_y - dest_h;
    }
    
    // Create destination rectangle
    var dest_rect = create_sdl_rect();
    set_rect(dest_rect, screen_x, screen_y, dest_w, dest_h);
    
    // Render texture region
    SDL_RenderCopy(graphics_get_renderer(), texture, src_rect, dest_rect);
}

// ========================================
// Advanced Texture Operations
// ========================================

// Set texture blend mode (for transparency)
function set_texture_blend_mode(texture as pointer, blend_mode as integer) {
    if (is_null(texture)) {
        print("ERROR: Cannot set blend mode on null texture");
        return;
    }
    
    SDL_SetTextureBlendMode(texture, blend_mode);
}

// Set texture alpha (transparency)
function set_texture_alpha(texture as pointer, alpha as integer) {
    if (is_null(texture)) {
        print("ERROR: Cannot set alpha on null texture");
        return;
    }
    
    // Clamp alpha to 0-255 range
    if (alpha < 0) alpha = 0;
    if (alpha > 255) alpha = 255;
    
    SDL_SetTextureAlphaMod(texture, alpha);
}

// Set texture color modulation (tinting)
function set_texture_color_mod(texture as pointer, r as integer, g as integer, b as integer) {
    if (is_null(texture)) {
        print("ERROR: Cannot set color mod on null texture");
        return;
    }
    
    // Clamp values to 0-255 range
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    
    SDL_SetTextureColorMod(texture, r, g, b);
}

// ========================================
// Texture Resource Management
// ========================================

// Destroy a texture and free memory
function destroy_texture(texture as pointer) {
    if (not_null(texture)) {
        SDL_DestroyTexture(texture);
        texture_count = texture_count - 1;
        print("Texture destroyed. Remaining textures: " + str(texture_count));
    }
}

// Get texture information
function get_texture_info(texture as pointer) {
    if (is_null(texture)) {
        print("ERROR: Cannot get info from null texture");
        return;
    }
    
    var width_buffer = alloc_int_buffer();
    var height_buffer = alloc_int_buffer();
    var format_buffer = alloc_int_buffer();
    var access_buffer = alloc_int_buffer();
    
    SDL_QueryTexture(texture, format_buffer, access_buffer, width_buffer, height_buffer);
    
    var width = deref_int(width_buffer);
    var height = deref_int(height_buffer);
    var format = deref_int(format_buffer);
    var access = deref_int(access_buffer);
    
    print("Texture Info:");
    print("- Size: " + str(width) + "x" + str(height));
    print("- Format: " + str(format));
    print("- Access: " + str(access));
}

// ========================================
// Module Information
// ========================================

function graphics_textures_info() {
    print("=== Graphics Textures Module ===");
    print("Features:");
    print("- PNG, JPG, WEBP texture loading");
    print("- Scaled and region-based rendering");
    print("- Alpha blending and color modulation");
    print("- Coordinate system integration");
    print("- Memory management");
    print("");
    print("Loaded textures: " + str(texture_count));
    print("Functions: load_texture(), draw_texture(), draw_texture_scaled()");
    print("Advanced: draw_texture_region(), set_texture_alpha(), set_texture_color_mod()");
}