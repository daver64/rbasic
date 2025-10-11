// Graphics Core Module - Fundamental graphics operations and coordinate system management
// Provides the foundation for all graphics operations with configurable coordinate systems

// ========================================
// SDL2_image Constants (must be first)
// ========================================

var IMG_INIT_JPG = 1;
var IMG_INIT_PNG = 2;
var IMG_INIT_TIF = 4;
var IMG_INIT_WEBP = 8;

// ========================================
// SDL2_image FFI Declarations
// ========================================

// Image loading functions
ffi integer IMG_Init(flags as integer) from "SDL2_image.dll";
ffi integer IMG_Quit() from "SDL2_image.dll";
ffi pointer IMG_Load(file as string) from "SDL2_image.dll";
ffi pointer IMG_LoadTexture(renderer as pointer, file as string) from "SDL2_image.dll";
ffi integer IMG_SavePNG(surface as pointer, file as string) from "SDL2_image.dll";

// ========================================
// Graphics Core State
// ========================================

var graphics_initialized = 0;
var graphics_coordinate_system = 0; // 0 = screen coordinates (default), 1 = cartesian
var graphics_screen_width = 800;
var graphics_screen_height = 600;
var graphics_current_renderer = 0;

// ========================================
// Core Initialization Functions
// ========================================

// Initialize the graphics system
function graphics_init(renderer as pointer, screen_width as integer, screen_height as integer) as integer {
    print("Initializing Graphics Module...");
    
    // Initialize SDL2_image with PNG, JPG, and WEBP support
    // Using literal values: IMG_INIT_PNG=2, IMG_INIT_JPG=1, IMG_INIT_WEBP=8
    var img_flags = 2 + 1 + 8;  // PNG + JPG + WEBP = 11
    var img_result = IMG_Init(img_flags);
    
    if (img_result == 0) {
        print("ERROR: Failed to initialize SDL2_image");
        return 0;
    }
    
    // Store graphics state
    graphics_current_renderer = renderer;
    graphics_screen_width = screen_width;
    graphics_screen_height = screen_height;
    graphics_initialized = 1;
    
    print("Graphics Module initialized successfully!");
    print("- Image formats: PNG, JPG, WEBP supported");
    print("- Coordinate system: Screen coordinates (configurable)");
    print("- Screen size: " + str(screen_width) + "x" + str(screen_height));
    
    return 1;
}

// Cleanup graphics resources
function graphics_cleanup() {
    if (graphics_initialized == 1) {
        print("Cleaning up Graphics Module...");
        IMG_Quit();
        graphics_initialized = 0;
        print("Graphics Module cleanup complete");
    }
}

// ========================================
// Coordinate System Management
// ========================================

// Set coordinate system: 0 = screen (0,0 top-left), 1 = cartesian (0,0 bottom-left)
function graphics_set_coordinate_system(system as integer) {
    if (system == 0) {
        graphics_coordinate_system = 0;
        print("Graphics: Using screen coordinates (0,0 = top-left, Y down)");
    } else if (system == 1) {
        graphics_coordinate_system = 1;
        print("Graphics: Using cartesian coordinates (0,0 = bottom-left, Y up)");
    } else {
        print("ERROR: Invalid coordinate system. Use 0 (screen) or 1 (cartesian)");
    }
}

// Get current coordinate system
function graphics_get_coordinate_system() as integer {
    return graphics_coordinate_system;
}

// Convert world coordinates to screen coordinates based on current system
function graphics_world_to_screen_x(world_x as double) as integer {
    // X coordinate is the same in both systems
    return world_x;
}

function graphics_world_to_screen_y(world_y as double) as integer {
    if (graphics_coordinate_system == 0) {
        // Screen coordinates - no conversion needed
        return world_y;
    } else {
        // Cartesian coordinates - flip Y axis
        return graphics_screen_height - world_y;
    }
}

// ========================================
// Utility Functions
// ========================================

// Check if graphics system is initialized
function graphics_is_initialized() as integer {
    return graphics_initialized;
}

// Get current renderer
function graphics_get_renderer() as pointer {
    return graphics_current_renderer;
}

// Get screen dimensions
function graphics_get_screen_width() as integer {
    return graphics_screen_width;
}

function graphics_get_screen_height() as integer {
    return graphics_screen_height;
}

// Set screen dimensions (useful for window resizing)
function graphics_set_screen_size(width as integer, height as integer) {
    graphics_screen_width = width;
    graphics_screen_height = height;
    print("Graphics: Screen size updated to " + str(width) + "x" + str(height));
}

// ========================================
// Basic Drawing Functions with Coordinate Support
// ========================================

// Draw a point at world coordinates
function graphics_draw_point(x as double, y as double) {
    if (graphics_initialized == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    var screen_x = graphics_world_to_screen_x(x);
    var screen_y = graphics_world_to_screen_y(y);
    
    SDL_RenderDrawPoint(graphics_current_renderer, screen_x, screen_y);
}

// Draw a line between two world coordinates
function graphics_draw_line(x1 as double, y1 as double, x2 as double, y2 as double) {
    if (graphics_initialized == 0) {
        print("ERROR: Graphics not initialized");
        return;
    }
    
    var screen_x1 = graphics_world_to_screen_x(x1);
    var screen_y1 = graphics_world_to_screen_y(y1);
    var screen_x2 = graphics_world_to_screen_x(x2);
    var screen_y2 = graphics_world_to_screen_y(y2);
    
    SDL_RenderDrawLine(graphics_current_renderer, screen_x1, screen_y1, screen_x2, screen_y2);
}

// ========================================
// Module Information
// ========================================

function graphics_core_info() {
    print("=== Graphics Core Module ===");
    print("Features:");
    print("- SDL2_image integration (PNG, JPG, WEBP)");
    print("- Configurable coordinate systems");
    print("- Screen and cartesian coordinate support");
    print("- Foundation for textures, sprites, and effects");
    print("");
    
    if (graphics_initialized == 1) {
        print("Status: INITIALIZED");
        print("Renderer: " + str(graphics_current_renderer));
        print("Screen: " + str(graphics_screen_width) + "x" + str(graphics_screen_height));
        print("Coordinate system: " + (graphics_coordinate_system == 0 ? "Screen" : "Cartesian"));
    } else {
        print("Status: NOT INITIALIZED");
        print("Call graphics_init() first");
    }
}