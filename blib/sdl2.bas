// SDL2 Graphics Library for rbasic
// High-level wrapper around SDL2 using rbasic's FFI system
// Provides easy-to-use graphics, input, and window management functions

// SDL2 Library Loading and Initialization
ffi integer SDL_Init(flags as integer) from "SDL2.dll";
ffi integer SDL_Quit() from "SDL2.dll";
ffi pointer SDL_CreateWindow(title as string, x as integer, y as integer, w as integer, h as integer, flags as integer) from "SDL2.dll";
ffi integer SDL_DestroyWindow(window as pointer) from "SDL2.dll";
ffi pointer SDL_CreateRenderer(window as pointer, index as integer, flags as integer) from "SDL2.dll";
ffi integer SDL_DestroyRenderer(renderer as pointer) from "SDL2.dll";
ffi integer SDL_SetRenderDrawColor(renderer as pointer, r as integer, g as integer, b as integer, a as integer) from "SDL2.dll";
ffi integer SDL_RenderClear(renderer as pointer) from "SDL2.dll";
ffi integer SDL_RenderPresent(renderer as pointer) from "SDL2.dll";
ffi integer SDL_PollEvent(event as pointer) from "SDL2.dll";
ffi integer SDL_Delay(ms as integer) from "SDL2.dll";
ffi integer SDL_GetTicks() from "SDL2.dll";

// Drawing Functions
ffi integer SDL_RenderDrawPoint(renderer as pointer, x as integer, y as integer) from "SDL2.dll";
ffi integer SDL_RenderDrawLine(renderer as pointer, x1 as integer, y1 as integer, x2 as integer, y2 as integer) from "SDL2.dll";
ffi integer SDL_RenderDrawRect(renderer as pointer, rect as pointer) from "SDL2.dll";
ffi integer SDL_RenderFillRect(renderer as pointer, rect as pointer) from "SDL2.dll";

// Global SDL state
var sdl_window = NULL;
var sdl_renderer = NULL;
var sdl_running = false;
var sdl_event = NULL;

// Initialize SDL graphics system
function sdl_init(title as string, width as integer, height as integer) as integer {
    var result = SDL_Init(get_constant("SDL_INIT_VIDEO"));
    if (result != 0) {
        print("Failed to initialize SDL");
        return 0;
    }
    
    sdl_window = SDL_CreateWindow(title, 
                                 get_constant("SDL_WINDOWPOS_CENTERED"), 
                                 get_constant("SDL_WINDOWPOS_CENTERED"), 
                                 width, height, 
                                 get_constant("SDL_WINDOW_SHOWN"));
    
    if (is_null(sdl_window)) {
        print("Failed to create SDL window");
        SDL_Quit();
        return 0;
    }
    
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 
                                     get_constant("SDL_RENDERER_ACCELERATED"));
    
    if (is_null(sdl_renderer)) {
        print("Failed to create SDL renderer");
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return 0;
    }
    
    sdl_event = create_sdl_event();
    sdl_running = true;
    
    print("SDL initialized successfully");
    return 1;
}

// Cleanup SDL resources
function sdl_cleanup() {
    if (not_null(sdl_renderer)) {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = NULL;
    }
    
    if (not_null(sdl_window)) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = NULL;
    }
    
    SDL_Quit();
    sdl_running = false;
    print("SDL cleaned up");
}

// Check if SDL is still running (no quit event)
function sdl_is_running() as integer {
    return sdl_running;
}

// Process SDL events
function sdl_poll_events() as integer {
    var events_processed = 0;
    
    while (SDL_PollEvent(sdl_event) == 1) {
        events_processed = events_processed + 1;
        var event_type = get_event_type(sdl_event);
        
        if (event_type == get_constant("SDL_QUIT")) {
            sdl_running = false;
        }
    }
    
    return events_processed;
}

// Get last key pressed (returns key code or 0 if no key event)
function sdl_get_key() as integer {
    if (SDL_PollEvent(sdl_event) == 1) {
        var event_type = get_event_type(sdl_event);
        
        if (event_type == get_constant("SDL_KEYDOWN")) {
            return get_key_code(sdl_event);
        }
    }
    
    return 0;
}

// Check if a specific key was pressed
function sdl_key_pressed(key_code as integer) as integer {
    var key = sdl_get_key();
    return (key == key_code);
}

// Set drawing color
function sdl_set_color(r as integer, g as integer, b as integer) {
    SDL_SetRenderDrawColor(sdl_renderer, r, g, b, 255);
}

// Clear screen with current color
function sdl_clear() {
    SDL_RenderClear(sdl_renderer);
}

// Present/display the rendered frame
function sdl_present() {
    SDL_RenderPresent(sdl_renderer);
}

// Draw a pixel
function sdl_draw_pixel(x as integer, y as integer) {
    SDL_RenderDrawPoint(sdl_renderer, x, y);
}

// Draw a line
function sdl_draw_line(x1 as integer, y1 as integer, x2 as integer, y2 as integer) {
    SDL_RenderDrawLine(sdl_renderer, x1, y1, x2, y2);
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

// Delay for specified milliseconds
function sdl_delay(ms as integer) {
    SDL_Delay(ms);
}

// Get milliseconds since SDL initialization
function sdl_get_ticks() as integer {
    return SDL_GetTicks();
}

// High-level convenience functions

// Simple animation loop with FPS control
function sdl_frame_start() as integer {
    var start_time = sdl_get_ticks();
    sdl_poll_events();
    return start_time;
}

function sdl_frame_end(start_time as integer, target_fps as integer) {
    var frame_time = 1000 / target_fps;
    var elapsed = sdl_get_ticks() - start_time;
    
    if (elapsed < frame_time) {
        sdl_delay(frame_time - elapsed);
    }
}

// Draw a circle (using line approximation)
function sdl_draw_circle(center_x as integer, center_y as integer, radius as integer) {
    var segments = 32;
    var angle_step = 6.283185 / segments; // 2 * PI / segments
    
    var x1 = center_x + radius;
    var y1 = center_y;
    
    for (var i = 1; i <= segments; i = i + 1) {
        var angle = i * angle_step;
        var x2 = center_x + int(cos(angle) * radius);
        var y2 = center_y + int(sin(angle) * radius);
        
        sdl_draw_line(x1, y1, x2, y2);
        
        x1 = x2;
        y1 = y2;
    }
}

// Draw a filled circle (using horizontal lines)
function sdl_fill_circle(center_x as integer, center_y as integer, radius as integer) {
    for (var y = -radius; y <= radius; y = y + 1) {
        var half_width = int(sqrt(radius * radius - y * y));
        var line_y = center_y + y;
        
        sdl_draw_line(center_x - half_width, line_y, center_x + half_width, line_y);
    }
}

// Convenience color constants
function color_red() as integer { return 255; }
function color_green() as integer { return 255; }
function color_blue() as integer { return 255; }
function color_black() as integer { return 0; }
function color_white() as integer { return 255; }

// Predefined color setters
function sdl_color_red() { sdl_set_color(255, 0, 0); }
function sdl_color_green() { sdl_set_color(0, 255, 0); }
function sdl_color_blue() { sdl_set_color(0, 0, 255); }
function sdl_color_white() { sdl_set_color(255, 255, 255); }
function sdl_color_black() { sdl_set_color(0, 0, 0); }
function sdl_color_yellow() { sdl_set_color(255, 255, 0); }
function sdl_color_magenta() { sdl_set_color(255, 0, 255); }
function sdl_color_cyan() { sdl_set_color(0, 255, 255); }

// Key code constants for common keys
function key_escape() as integer { return get_constant("SDLK_ESCAPE"); }
function key_space() as integer { return get_constant("SDLK_SPACE"); }
function key_enter() as integer { return get_constant("SDLK_RETURN"); }
function key_up() as integer { return get_constant("SDLK_UP"); }
function key_down() as integer { return get_constant("SDLK_DOWN"); }
function key_left() as integer { return get_constant("SDLK_LEFT"); }
function key_right() as integer { return get_constant("SDLK_RIGHT"); }

print("SDL2 library loaded successfully");