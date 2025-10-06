// sdl2.bas - SDL2 wrapper library for rbasic

// SDL2 wrapper functions using FFI
declare function SDL_Init from "SDL2.dll" (int flags) as int;
declare function SDL_CreateWindow from "SDL2.dll" (string title, int x, int y, int w, int h, int flags) as pointer;
declare function SDL_Quit from "SDL2.dll" () as void;

// High-level wrapper functions
function init_graphics() as int {
    var sdl_init_video = get_constant("SDL_INIT_VIDEO");
    return call_ffi_function("SDL2.dll", "SDL_Init", sdl_init_video);
}

function create_window(string title, int width, int height) as pointer {
    var pos_centered = get_constant("SDL_WINDOWPOS_CENTERED");
    var window_shown = get_constant("SDL_WINDOW_SHOWN");
    
    return call_ffi_function("SDL2.dll", "SDL_CreateWindow", 
                            title, pos_centered, pos_centered, 
                            width, height, window_shown);
}

function quit_graphics() as void {
    call_ffi_function("SDL2.dll", "SDL_Quit");
}

// Library constants
var SDL_GRAPHICS_LIBRARY_VERSION = "1.0";

print("SDL2 wrapper library loaded successfully!");