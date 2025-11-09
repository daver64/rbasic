// Direct SDL2 Texture Test - Bypasses graphics module for debugging
// Tests texture rendering directly with SDL2 calls

import "blib/sdl2.bas";

// Add IMG_LoadTexture if not available
ffi pointer IMG_LoadTexture(renderer as pointer, file as string) from "SDL2_image";

print("=== Direct SDL2 Texture Test ===");

// Initialize SDL2 
if (sdl_init("Direct Texture Test", 800, 600) == 0) {
    print("Failed to initialize SDL");
    return;
}

// Initialize SDL2_image
var img_init_result = get_constant("IMG_INIT_PNG");
print("Initializing SDL2_image...");

// Load texture using IMG_LoadTexture directly
print("Loading texture: test_pattern24.bmp");
var test_texture = IMG_LoadTexture(sdl_renderer, "test_pattern24.bmp");

if (is_null(test_texture)) {
    print("ERROR: Could not load test_pattern24.bmp");
    sdl_cleanup();
    return;
}

print("Texture loaded successfully!");
print("Drawing 128x128 texture at (0,0)");
print("Press ESCAPE to exit");

var frame_count = 0;

// Main demo loop
while (sdl_running == 1) {
    // Clear screen to dark blue
    SDL_SetRenderDrawColor(sdl_renderer, 0, 32, 64, 255);
    SDL_RenderClear(sdl_renderer);
    
    // Create a simple 128x128 rectangle at (0,0)
    var dest_rect = create_sdl_rect(0, 0, 128, 128);
    
    // Draw texture with explicit destination rectangle
    SDL_RenderCopy(sdl_renderer, test_texture, 0, dest_rect);
    
    // Present the frame
    SDL_RenderPresent(sdl_renderer);
    
    // Process events
    sdl_process_events();
    
    // Frame rate control
    SDL_Delay(16);
    
    frame_count = frame_count + 1;
    
    // Exit after 300 frames (5 seconds) to avoid infinite loop
    if (frame_count > 300) {
        print("Auto-exit after 5 seconds");
        sdl_running = 0;
    }
}

print("Direct texture test completed!");
print("Frames rendered: " + str(frame_count));

// Cleanup
if (not_null(test_texture)) {
    SDL_DestroyTexture(test_texture);
}
sdl_cleanup();
print("Direct Texture Test finished!");