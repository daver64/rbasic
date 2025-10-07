// Simple mouse test - just verify deref_int_offset works

print("=== Simple Mouse Test ===");

// Initialize SDL
print("Initializing SDL...");
ffi "SDL2.dll" SDL_Init(flags as integer) as integer;
var result = SDL_Init(32);  // SDL_INIT_VIDEO
if (result != 0) {
    print("SDL_Init failed");
    exit(1);
}
print("SDL initialized successfully");

// Create window
ffi "SDL2.dll" SDL_CreateWindow(title as string, x as integer, y as integer, w as integer, h as integer, flags as integer) as pointer;
var window = SDL_CreateWindow("Simple Mouse Test", 100, 100, 400, 300, 4);  // SDL_WINDOW_SHOWN
if (is_null(window)) {
    print("Failed to create window");
    SDL_Quit();
    exit(1);
}
print("Window created successfully");

// Create event buffer
ffi "SDL2.dll" SDL_PollEvent(event as pointer) as integer;
var event_buffer = alloc_buffer(64);
print("Event buffer created");

// Test deref_int_offset basic functionality
print("Testing deref_int_offset...");
var test_buffer = alloc_buffer(16);
// We can't easily test without actual data, so let's test in the main loop

print("Starting simple event loop...");
print("Move mouse and click. Press any key to quit.");

var running = 1;
while (running == 1) {
    var has_event = SDL_PollEvent(event_buffer);
    if (has_event == 1) {
        var event_type = deref_int_offset(event_buffer, 0);
        print("Event type: " + event_type);
        
        if (event_type == 256) {  // SDL_QUIT
            print("Quit event received");
            running = 0;
        } else if (event_type == 768) {  // SDL_KEYDOWN
            print("Key pressed - exiting");
            running = 0;
        } else if (event_type == 1025) {  // SDL_MOUSEBUTTONDOWN
            var button = deref_int_offset(event_buffer, 16);  // button field
            print("Mouse button pressed: " + button);
            if (button == 1) {  // SDL_BUTTON_LEFT
                print("Left mouse button");
            } else if (button == 3) {  // SDL_BUTTON_RIGHT
                print("Right mouse button");
            } else if (button == 2) {  // SDL_BUTTON_MIDDLE
                print("Middle mouse button");
            }
        } else if (event_type == 1024) {  // SDL_MOUSEMOTION
            var x = deref_int_offset(event_buffer, 16);  // x coordinate
            var y = deref_int_offset(event_buffer, 20);  // y coordinate
            print("Mouse moved to: " + x + ", " + y);
        }
    }
}

// Cleanup
SDL_DestroyWindow(window);
SDL_Quit();
print("Simple Mouse Test completed");