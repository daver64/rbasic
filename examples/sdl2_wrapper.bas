// sdl2_wrapper.bas - SDL2 wrapper library for rbasic (simplified for demo)

// High-level wrapper functions (simplified - no actual FFI calls for now)
function init_graphics() as int {
    print("Initializing graphics...");
    return 1; // Success
}

function create_window(title as string, width as int, height as int) as int {
    print("Creating window: ");
    print(title);
    print(" (");
    print(width);
    print("x");
    print(height);
    print(")");
    return 1; // Window handle placeholder
}

function quit_graphics() as int {
    print("Shutting down graphics...");
    return 0;
}

// Helper functions for common SDL operations
function create_basic_window(title as string) as int {
    return create_window(title, 800, 600);
}

function show_message_box(text as string, title as string) as int {
    print("Message Box - ");
    print(title);
    print(": ");
    print(text);
    return 1; // OK button pressed
}

// Library constants
var SDL_GRAPHICS_LIBRARY_VERSION = "1.0";

print("SDL2 wrapper library loaded successfully!");