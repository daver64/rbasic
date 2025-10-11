// Minimal SDL2 Graphics Library for rbasic
// Simple wrapper for basic window creation, clearing, and event handling
// Just enough functionality to create a window, clear to blue, and handle escape key

// ========================================
// Core SDL2 FFI Declarations
// ========================================

// Essential SDL2 functions for basic window and rendering
ffi integer SDL_Init(flags as integer) from "SDL2.dll";
ffi integer SDL_Quit() from "SDL2.dll";
ffi pointer SDL_CreateWindow(title as string, x as integer, y as integer, w as integer, h as integer, flags as integer) from "SDL2.dll";
ffi integer SDL_DestroyWindow(window as pointer) from "SDL2.dll";
ffi pointer SDL_CreateRenderer(window as pointer, index as integer, flags as integer) from "SDL2.dll";
ffi integer SDL_DestroyRenderer(renderer as pointer) from "SDL2.dll";

// Rendering functions
ffi integer SDL_SetRenderDrawColor(renderer as pointer, r as integer, g as integer, b as integer, a as integer) from "SDL2.dll";
ffi integer SDL_RenderClear(renderer as pointer) from "SDL2.dll";
ffi integer SDL_RenderPresent(renderer as pointer) from "SDL2.dll";

// SDL2_gfx functions for filled shapes (with correct signatures from SDL2_gfxPrimitives.h)
ffi integer filledTrigonRGBA(renderer as pointer, x1 as integer, y1 as integer, x2 as integer, y2 as integer, x3 as integer, y3 as integer, r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";
ffi integer filledCircleRGBA(renderer as pointer, x as integer, y as integer, rad as integer, r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";
ffi integer filledEllipseRGBA(renderer as pointer, x as integer, y as integer, rx as integer, ry as integer, r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";

// SDL2_gfx functions for lines and rectangles  
ffi integer lineRGBA(renderer as pointer, x1 as integer, y1 as integer, x2 as integer, y2 as integer, r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";
ffi integer rectangleRGBA(renderer as pointer, x1 as integer, y1 as integer, x2 as integer, y2 as integer, r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";
ffi integer boxRGBA(renderer as pointer, x1 as integer, y1 as integer, x2 as integer, y2 as integer, r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";

// Drawing functions
ffi integer SDL_RenderDrawPoint(renderer as pointer, x as integer, y as integer) from "SDL2.dll";
ffi integer SDL_RenderDrawLine(renderer as pointer, x1 as integer, y1 as integer, x2 as integer, y2 as integer) from "SDL2.dll";
ffi integer SDL_RenderDrawRect(renderer as pointer, rect as pointer) from "SDL2.dll";
ffi integer SDL_RenderFillRect(renderer as pointer, rect as pointer) from "SDL2.dll";

// Window management functions
ffi integer SDL_SetWindowTitle(window as pointer, title as string) from "SDL2.dll";
ffi integer SDL_GetWindowSize(window as pointer, w as pointer, h as pointer) from "SDL2.dll";
ffi integer SDL_SetWindowSize(window as pointer, w as integer, h as integer) from "SDL2.dll";

// Timing functions  
ffi integer SDL_GetTicks() from "SDL2.dll";
ffi integer SDL_GetPerformanceCounter() from "SDL2.dll";
ffi integer SDL_GetPerformanceFrequency() from "SDL2.dll";

// Event handling
ffi integer SDL_PollEvent(event as pointer) from "SDL2.dll";

// Texture functions
ffi pointer SDL_CreateTexture(renderer as pointer, format as integer, access as integer, w as integer, h as integer) from "SDL2.dll";
ffi integer SDL_DestroyTexture(texture as pointer) from "SDL2.dll";
ffi integer SDL_QueryTexture(texture as pointer, format as pointer, access as pointer, w as pointer, h as pointer) from "SDL2.dll";
ffi integer SDL_RenderCopy(renderer as pointer, texture as pointer, src as pointer, dest as pointer) from "SDL2.dll";
ffi integer SDL_SetTextureBlendMode(texture as pointer, blend_mode as integer) from "SDL2.dll";
ffi integer SDL_SetTextureAlphaMod(texture as pointer, alpha as integer) from "SDL2.dll";
ffi integer SDL_SetTextureColorMod(texture as pointer, r as integer, g as integer, b as integer) from "SDL2.dll";

// Texture constants
var SDL_PIXELFORMAT_RGBA8888 = 373694468;
var SDL_TEXTUREACCESS_STATIC = 0;
var SDL_TEXTUREACCESS_STREAMING = 1;
var SDL_TEXTUREACCESS_TARGET = 2;

// Blend modes
var SDL_BLENDMODE_NONE = 0;
var SDL_BLENDMODE_BLEND = 1;
var SDL_BLENDMODE_ADD = 2;
var SDL_BLENDMODE_MOD = 4;

// Timing
ffi integer SDL_Delay(ms as integer) from "SDL2.dll";

// ========================================
// Global State
// ========================================

var sdl_window = 0;
var sdl_renderer = 0;
var sdl_running = 0;
var sdl_event_buffer = 0;

// ========================================
// Core Functions
// ========================================

// Initialize SDL and create a window with renderer
function sdl_init(title as string, width as integer, height as integer) as integer {
    print("Initializing SDL2...");
    
    // Initialize SDL video subsystem
    var init_result = SDL_Init(get_constant("SDL_INIT_VIDEO"));
    if (init_result != 0) {
        print("ERROR: Failed to initialize SDL2");
        return 0;
    }
    
    // Create window (positioned at 100, 100)
    sdl_window = SDL_CreateWindow(title, 100, 100, width, height, get_constant("SDL_WINDOW_SHOWN"));
    if (is_null(sdl_window)) {
        print("ERROR: Failed to create SDL window");
        SDL_Quit();
        return 0;
    }
    
    // Create hardware-accelerated renderer
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, get_constant("SDL_RENDERER_ACCELERATED"));
    if (is_null(sdl_renderer)) {
        print("ERROR: Failed to create SDL renderer");
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return 0;
    }
    
    // Create event buffer
    sdl_event_buffer = create_sdl_event();
    sdl_running = 1;
    
    print("SDL2 initialized successfully!");
    return 1;
}

// Cleanup SDL resources
function sdl_cleanup() {
    if (not_null(sdl_renderer)) {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = 0;
    }
    
    if (not_null(sdl_window)) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = 0;
    }
    
    SDL_Quit();
    sdl_running = 0;
    print("SDL2 cleanup complete");
}

// Check if application should keep running
function sdl_is_running() as integer {
    return sdl_running;
}

// Get the SDL renderer
function sdl_get_renderer() as pointer {
    return sdl_renderer;
}

// Clear screen to blue colour
function sdl_clear_blue() {
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 255, 255); // Blue background
    SDL_RenderClear(sdl_renderer);
}

// Set drawing colour
function sdl_set_colour(r as integer, g as integer, b as integer) {
    SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
}

// Draw a single point
function sdl_draw_point(x as integer, y as integer) {
    SDL_RenderDrawPoint(sdl_renderer, x, y);
}

// Draw a line
function sdl_draw_line(x1 as integer, y1 as integer, x2 as integer, y2 as integer) {
    SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
}

// Window management functions
function sdl_set_window_title(title as string) {
    SDL_SetWindowTitle(sdl_window, title);
}

function sdl_set_window_size(width as integer, height as integer) {
    SDL_SetWindowSize(sdl_window, width, height);
}

// Timing functions
function sdl_get_ticks() as integer {
    return SDL_GetTicks();
}

function sdl_get_performance_time() as double {
    var counter = SDL_GetPerformanceCounter();
    var frequency = SDL_GetPerformanceFrequency();
    return counter / frequency;
}

// Draw a rectangle outline
function sdl_draw_rect(x as integer, y as integer, w as integer, h as integer) {
    var rect = create_sdl_rect(x, y, w, h);
    SDL_RenderDrawRect(sdl_renderer, rect);
}

// Draw a filled rectangle
function sdl_fill_rect(x as integer, y as integer, w as integer, h as integer) {
    var rect = create_sdl_rect(x, y, w, h);
    SDL_RenderFillRect(sdl_renderer, rect);
}

// Draw a filled triangle using three points with SDL2_gfx
function sdl_fill_triangle(x1 as integer, y1 as integer, x2 as integer, y2 as integer, x3 as integer, y3 as integer) {
    // Use SDL2_gfx for proper filled triangle rendering
    // filledTrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, r, g, b, a)
    // Yellow colour: r=255, g=255, b=0, a=255
    filledTrigonRGBA(sdl_renderer, x1, y1, x2, y2, x3, y3, 255, 255, 0, 255);
}

// Draw a filled circle using SDL2_gfx
function sdl_fill_circle(x as integer, y as integer, radius as integer) {
    // Use SDL2_gfx for filled circle rendering
    // filledCircleRGBA(renderer, x, y, rad, r, g, b, a)
    // Green colour: r=0, g=255, b=0, a=255
    filledCircleRGBA(sdl_renderer, x, y, radius, 0, 255, 0, 255);
}

// Draw a filled ellipse using SDL2_gfx
function sdl_fill_ellipse(x as integer, y as integer, rx as integer, ry as integer) {
    // Use SDL2_gfx for filled ellipse rendering
    // filledEllipseRGBA(renderer, x, y, rx, ry, r, g, b, a)
    // Purple colour: r=128, g=0, b=128, a=255
    filledEllipseRGBA(sdl_renderer, x, y, rx, ry, 128, 0, 128, 255);
}

// Draw a coloured line using SDL2_gfx
function sdl_draw_coloured_line(x1 as integer, y1 as integer, x2 as integer, y2 as integer, r as integer, g as integer, b as integer) {
    // Use SDL2_gfx for coloured line rendering
    // lineRGBA(renderer, x1, y1, x2, y2, r, g, b, a)
    lineRGBA(sdl_renderer, x1, y1, x2, y2, r, g, b, 255);
}

// Draw a rectangle outline using SDL2_gfx
function sdl_draw_coloured_rect(x1 as integer, y1 as integer, x2 as integer, y2 as integer, r as integer, g as integer, b as integer) {
    // Use SDL2_gfx for coloured rectangle outline
    // rectangleRGBA(renderer, x1, y1, x2, y2, r, g, b, a)
    rectangleRGBA(sdl_renderer, x1, y1, x2, y2, r, g, b, 255);
}

// Draw a filled rectangle using SDL2_gfx
function sdl_fill_coloured_rect(x1 as integer, y1 as integer, x2 as integer, y2 as integer, r as integer, g as integer, b as integer) {
    // Use SDL2_gfx for filled coloured rectangle
    // boxRGBA(renderer, x1, y1, x2, y2, r, g, b, a)
    boxRGBA(sdl_renderer, x1, y1, x2, y2, r, g, b, 255);
}

// Present the rendered frame
function sdl_present() {
    SDL_RenderPresent(sdl_renderer);
}

// Process events and check for escape key
function sdl_process_events() as integer {
    var events_processed = 0;
    
    // Create a local event buffer to avoid global variable issues
    var local_event_buffer = create_sdl_event();
    
    while (SDL_PollEvent(local_event_buffer) == 1) {
        events_processed = events_processed + 1;
        
        // Use the safer get_event_type function
        var event_type = get_event_type(local_event_buffer);
        
        // Handle quit event (window close button)
        if (event_type == get_constant("SDL_QUIT")) {
            sdl_running = 0;
        }
        
        // Handle key press events
        if (event_type == get_constant("SDL_KEYDOWN")) {
            var key_code = get_key_code(local_event_buffer);
            
            // Check for escape key
            if (key_code == get_constant("SDL_SCANCODE_ESCAPE")) {
                sdl_running = 0;
            }
        }
    }
    
    return events_processed;
}

// Simple delay function
function sdl_delay(milliseconds as integer) {
    SDL_Delay(milliseconds);
}

// ========================================
// SDL Resource Management
// ========================================

// Free a specific SDL resource (rect, event buffer, etc.)
function sdl_free_resource(ptr as pointer) as integer {
    return free_sdl_resource(ptr);
}

// Clean up all allocated SDL resources
function sdl_cleanup_resources() {
    sdl_cleanup_all();
}

// ========================================
// Library Initialization
// ========================================

print("SDL2 Minimal Library loaded");
print("Functions: sdl_init(), sdl_cleanup(), sdl_clear_blue(), sdl_present(), sdl_process_events()");
print("Drawing: sdl_set_colour(), sdl_draw_point(), sdl_draw_line(), sdl_draw_rect(), sdl_fill_rect(), sdl_fill_triangle(), sdl_fill_circle(), sdl_fill_ellipse()");
print("Coloured: sdl_draw_coloured_line(), sdl_draw_coloured_rect(), sdl_fill_coloured_rect()");
print("Window: sdl_set_window_title(), sdl_set_window_size()");
print("Timing: sdl_get_ticks(), sdl_get_performance_time()");
print("Memory: sdl_free_resource(), sdl_cleanup_resources()");