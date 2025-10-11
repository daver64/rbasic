// Minimal graphics core test - no functions
// SDL2_image Constants (must be first)
var IMG_INIT_JPG = 1;
var IMG_INIT_PNG = 2;
var IMG_INIT_TIF = 4;
var IMG_INIT_WEBP = 8;

// SDL2_image FFI Declarations
ffi integer IMG_Init(flags as integer) from "SDL2_image.dll";
ffi integer IMG_Quit() from "SDL2_image.dll";

// Graphics Core State
var graphics_initialized = 0;
var graphics_coordinate_system = 0; // 0 = screen coordinates (default), 1 = cartesian
var graphics_screen_width = 800;
var graphics_screen_height = 600;
var graphics_current_renderer = 0;

// Initialize the graphics system
function graphics_init(renderer as pointer, screen_width as integer, screen_height as integer) as integer {
    print("Initializing Graphics Module...");
    
    // Store parameters in local variables first (workaround for parser issue)
    var local_renderer = renderer;
    var local_width = screen_width;
    var local_height = screen_height;
    
    // Declare other variables
    var img_flags = 0;
    var img_result = 0;
    
    // Initialize SDL2_image with PNG, JPG, and WEBP support
    // Using literal values: IMG_INIT_PNG=2, IMG_INIT_JPG=1, IMG_INIT_WEBP=8
    img_flags = 2 + 1 + 8;  // PNG + JPG + WEBP = 11
    img_result = IMG_Init(img_flags);
    
    if (img_result == 0) {
        print("ERROR: Failed to initialize SDL2_image");
        return 0;
    }
    
    // Store graphics state
    graphics_current_renderer = local_renderer;
    graphics_screen_width = local_width;
    graphics_screen_height = local_height;
    graphics_initialized = 1;
    
    print("Graphics Module initialized successfully!");
    return 1;
}

print("Minimal graphics core loaded successfully!");