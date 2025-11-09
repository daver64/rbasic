// Graphics Textures Module - Rebuilt from working parts
// Image loading, texture management, and basic texture operations

import "blib/graphics/core.bas";

// ========================================
// Texture Management State
// ========================================

var loaded_textures = 0;
var texture_count = 0;

// ========================================
// Texture Loading Functions
// ========================================

// Load a texture from file (PNG, JPG, WEBP, BMP supported)
function load_texture(filename as string) as pointer {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics system not initialized. Call graphics_init() first.");
        return 0;
    }
    
    print("Loading texture: " + filename);
    
    // Try IMG_LoadTexture first (supports BMP, PNG, JPG, etc.)
    var texture = IMG_LoadTexture(graphics_get_renderer(), filename);

    if (is_null(texture)) {
        // Fallback: try loading as surface and create texture from surface
        print("IMG_LoadTexture failed, attempting surface fallback for: " + filename);
        var surface = IMG_Load(filename);
        if (is_null(surface)) {
            print("ERROR: Failed to load texture: " + filename);
            print("Make sure the file exists and is a supported format (BMP, PNG, JPG, WEBP)");
            return 0;
        }

        // Create texture from surface
        texture = SDL_CreateTextureFromSurface(graphics_get_renderer(), surface);
        // Free the surface regardless
        SDL_FreeSurface(surface);

        if (is_null(texture)) {
            print("ERROR: Failed to create texture from surface for: " + filename);
            return 0;
        }
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
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET,
                                   width, height);
    
    if (is_null(texture)) {
        print("ERROR: Failed to create blank texture");
        return 0;
    }
    
    texture_count = texture_count + 1;
    return texture;
}

// ========================================
// Basic Texture Rendering
// ========================================

// Draw entire texture at world coordinates (simplified version)
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
    var query_result = SDL_QueryTexture(texture, null, null, width_buffer, height_buffer);
    
    if (query_result != 0) {
        print("ERROR: SDL_QueryTexture failed - using fallback size 128x128");
        // Print SDL error to help debugging
        var err_ptr = SDL_GetError();
        var err_msg = deref_string(err_ptr);
        print("SDL_Error: " + err_msg);
        // Fallback to a reasonable default size
        draw_texture_sized(texture, x, y, 128, 128);
        return;
    }
    
    var width = deref_int(width_buffer);
    var height = deref_int(height_buffer);
    
    // Convert to screen coordinates
    var screen_x_float = graphics_world_to_screen_x(x);
    var screen_y_float = graphics_world_to_screen_y(y);
    
    // Convert to integers for SDL rectangle
    var screen_x = int(screen_x_float);
    var screen_y = int(screen_y_float);
    var width_int = int(width);
    var height_int = int(height);
    
    // In cartesian mode, adjust for texture height
    if (graphics_get_coordinate_system() == 1) {
        screen_y = screen_y - height_int;
    }
    
    print("Drawing texture at: " + str(screen_x) + ", " + str(screen_y) + " size: " + str(width_int) + "x" + str(height_int));
    
    // Create destination rectangle for proper positioning and sizing
    var dest_rect = create_sdl_rect(screen_x, screen_y, width_int, height_int);
    SDL_RenderCopy(graphics_get_renderer(), texture, null, dest_rect);
}

// Draw texture with custom width and height at world coordinates
function draw_texture_sized(texture as pointer, x as double, y as double, width as integer, height as integer) {
    if (graphics_is_initialized() == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    if (is_null(texture)) {
        print("ERROR: Cannot draw null texture");
        return;
    }
    
    // Convert to screen coordinates
    var screen_x_float = graphics_world_to_screen_x(x);
    var screen_y_float = graphics_world_to_screen_y(y);
    
    // Convert to integers for SDL rectangle
    var screen_x = int(screen_x_float);
    var screen_y = int(screen_y_float);
    
    // In cartesian mode, adjust for texture height
    if (graphics_get_coordinate_system() == 1) {
        screen_y = screen_y - height;
    }
    
    // Create destination rectangle with custom size
    var dest_rect = create_sdl_rect(screen_x, screen_y, width, height);
    
    if (is_null(dest_rect)) {
        print("ERROR: Failed to create SDL rectangle!");
        return 0;
    }
    
    SDL_RenderCopy(graphics_get_renderer(), texture, null, dest_rect);
    
    return 0;
}

// Set texture alpha (transparency)
function set_texture_alpha(texture as pointer, alpha as integer) {
    if (is_null(texture)) {
        print("ERROR: Cannot set alpha on null texture");
        return;
    }
    
    // Clamp alpha to 0-255 range
    if (alpha < 0) {
        alpha = 0;
    }
    if (alpha > 255) {
        alpha = 255;
    }
    
    SDL_SetTextureAlphaMod(texture, alpha);
}

// Destroy a texture and free memory
function destroy_texture(texture as pointer) {
    if (not_null(texture)) {
        SDL_DestroyTexture(texture);
        texture_count = texture_count - 1;
        print("Texture destroyed. Remaining textures: " + texture_count);
    }
}

// Module information
function graphics_textures_info() {
    print("=== Graphics Textures Module ===");
    print("Features:");
    print("- PNG, JPG, WEBP texture loading");
    print("- Basic texture rendering");
    print("- Alpha blending support");
    print("- Coordinate system integration");
    print("- Memory management");
    print("Loaded textures: " + texture_count);
    print("Functions: load_texture(), draw_texture(), draw_texture_sized(), set_texture_alpha(), destroy_texture()");
}

print("Graphics Textures Module loaded successfully!");