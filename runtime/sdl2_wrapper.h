#ifndef SDL2_WRAPPER_H
#define SDL2_WRAPPER_H

#ifdef SDL2_SUPPORT_ENABLED

#include <SDL.h>
#include <string>
#include <map>
#include <memory>

namespace basic_runtime {

// SDL2 wrapper functions
// Handle management structures
struct WindowHandle {
    SDL_Window* window;
    int id;
};

struct RendererHandle {
    SDL_Renderer* renderer;
    int id;
};

struct TextureHandle {
    SDL_Texture* texture;
    int id;
};

struct SurfaceHandle {
    SDL_Surface* surface;
    int id;
};

// Resource managers
class SDL2_ResourceManager {
public:
    static SDL2_ResourceManager& instance();
    
    // Window management
    int addWindow(SDL_Window* window);
    SDL_Window* getWindow(int handle);
    void removeWindow(int handle);
    
    // Renderer management
    int addRenderer(SDL_Renderer* renderer);
    SDL_Renderer* getRenderer(int handle);
    void removeRenderer(int handle);
    
    // Texture management
    int addTexture(SDL_Texture* texture);
    SDL_Texture* getTexture(int handle);
    void removeTexture(int handle);
    
    // Surface management
    int addSurface(SDL_Surface* surface);
    SDL_Surface* getSurface(int handle);
    void removeSurface(int handle);
    
    // Cleanup
    void cleanup();
    
private:
    SDL2_ResourceManager() : nextWindowId(1), nextRendererId(1), nextTextureId(1), nextSurfaceId(1) {}
    
    std::map<int, SDL_Window*> windows;
    std::map<int, SDL_Renderer*> renderers;
    std::map<int, SDL_Texture*> textures;
    std::map<int, SDL_Surface*> surfaces;
    
    int nextWindowId;
    int nextRendererId;
    int nextTextureId;
    int nextSurfaceId;
};

// Core SDL functions
int sdl_init(int flags);
void sdl_quit();
std::string sdl_get_error();

// Window functions
int sdl_create_window(const std::string& title, int x, int y, int w, int h, int flags);
void sdl_destroy_window(int window_handle);
void sdl_set_window_title(int window_handle, const std::string& title);
void sdl_set_window_size(int window_handle, int w, int h);
void sdl_set_window_position(int window_handle, int x, int y);
void sdl_show_window(int window_handle);
void sdl_hide_window(int window_handle);

// Renderer functions
int sdl_create_renderer(int window_handle, int index, int flags);
void sdl_destroy_renderer(int renderer_handle);
void sdl_render_clear(int renderer_handle);
void sdl_render_present(int renderer_handle);
void sdl_set_render_draw_color(int renderer_handle, int r, int g, int b, int a);
void sdl_get_render_draw_color(int renderer_handle, int* r, int* g, int* b, int* a);

// Drawing primitives
void sdl_render_draw_point(int renderer_handle, int x, int y);
void sdl_render_draw_line(int renderer_handle, int x1, int y1, int x2, int y2);
void sdl_render_draw_rect(int renderer_handle, int x, int y, int w, int h);
void sdl_render_fill_rect(int renderer_handle, int x, int y, int w, int h);

// Advanced drawing (requires SDL2_gfx)
#ifdef SDL2_GFX_AVAILABLE
void sdl_render_draw_circle(int renderer_handle, int x, int y, int radius);
void sdl_render_fill_circle(int renderer_handle, int x, int y, int radius);
void sdl_render_draw_ellipse(int renderer_handle, int x, int y, int rx, int ry);
void sdl_render_fill_ellipse(int renderer_handle, int x, int y, int rx, int ry);
void sdl_render_draw_triangle(int renderer_handle, int x1, int y1, int x2, int y2, int x3, int y3);
void sdl_render_fill_triangle(int renderer_handle, int x1, int y1, int x2, int y2, int x3, int y3);
#endif

// Texture functions
int sdl_create_texture_from_surface(int renderer_handle, int surface_handle);
int sdl_load_texture(int renderer_handle, const std::string& filename);
void sdl_destroy_texture(int texture_handle);
void sdl_render_copy(int renderer_handle, int texture_handle, int src_x, int src_y, int src_w, int src_h, 
                     int dst_x, int dst_y, int dst_w, int dst_h);
void sdl_query_texture(int texture_handle, int* format, int* access, int* w, int* h);

// Surface functions
int sdl_load_bmp(const std::string& filename);
void sdl_free_surface(int surface_handle);

// Event functions
int sdl_poll_event();
int sdl_get_event_type();
int sdl_get_key_scancode();
int sdl_get_key_keycode();
int sdl_get_mouse_x();
int sdl_get_mouse_y();
int sdl_get_mouse_button();
int sdl_get_mouse_state(int* x, int* y);

// Delay function
void sdl_delay(int ms);

// SDL constants
namespace constants {
    // Init flags
    const int INIT_VIDEO = SDL_INIT_VIDEO;
    const int INIT_AUDIO = SDL_INIT_AUDIO;
    const int INIT_TIMER = SDL_INIT_TIMER;
    const int INIT_JOYSTICK = SDL_INIT_JOYSTICK;
    const int INIT_HAPTIC = SDL_INIT_HAPTIC;
    const int INIT_GAMECONTROLLER = SDL_INIT_GAMECONTROLLER;
    const int INIT_EVENTS = SDL_INIT_EVENTS;
    const int INIT_EVERYTHING = SDL_INIT_EVERYTHING;
    
    // Window flags
    const int WINDOW_FULLSCREEN = SDL_WINDOW_FULLSCREEN;
    const int WINDOW_OPENGL = SDL_WINDOW_OPENGL;
    const int WINDOW_SHOWN = SDL_WINDOW_SHOWN;
    const int WINDOW_HIDDEN = SDL_WINDOW_HIDDEN;
    const int WINDOW_BORDERLESS = SDL_WINDOW_BORDERLESS;
    const int WINDOW_RESIZABLE = SDL_WINDOW_RESIZABLE;
    const int WINDOW_MINIMIZED = SDL_WINDOW_MINIMIZED;
    const int WINDOW_MAXIMIZED = SDL_WINDOW_MAXIMIZED;
    
    // Renderer flags
    const int RENDERER_SOFTWARE = SDL_RENDERER_SOFTWARE;
    const int RENDERER_ACCELERATED = SDL_RENDERER_ACCELERATED;
    const int RENDERER_PRESENTVSYNC = SDL_RENDERER_PRESENTVSYNC;
    const int RENDERER_TARGETTEXTURE = SDL_RENDERER_TARGETTEXTURE;
    
    // Event types
    const int EVENT_QUIT = SDL_QUIT;
    const int EVENT_KEYDOWN = SDL_KEYDOWN;
    const int EVENT_KEYUP = SDL_KEYUP;
    const int EVENT_MOUSEMOTION = SDL_MOUSEMOTION;
    const int EVENT_MOUSEBUTTONDOWN = SDL_MOUSEBUTTONDOWN;
    const int EVENT_MOUSEBUTTONUP = SDL_MOUSEBUTTONUP;
    const int EVENT_MOUSEWHEEL = SDL_MOUSEWHEEL;
    const int EVENT_WINDOWEVENT = SDL_WINDOWEVENT;
    
    // Window position
    const int WINDOWPOS_UNDEFINED = SDL_WINDOWPOS_UNDEFINED;
    const int WINDOWPOS_CENTERED = SDL_WINDOWPOS_CENTERED;
}

} // namespace basic_runtime

#endif // SDL2_SUPPORT_ENABLED

#endif // SDL2_WRAPPER_H
