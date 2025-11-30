#ifdef SDL2_SUPPORT_ENABLED

#include "sdl2_wrapper.h"
#include <stdexcept>
#include <SDL_image.h>

#ifdef SDL2_GFX_AVAILABLE
#include <SDL2_gfxPrimitives.h>
#endif

namespace basic_runtime {


// Static storage for last event
static SDL_Event last_event;
static bool event_available = false;

// ResourceManager implementation
SDL2_ResourceManager& SDL2_ResourceManager::instance() {
    static SDL2_ResourceManager instance;
    return instance;
}

int SDL2_ResourceManager::addWindow(SDL_Window* window) {
    int id = nextWindowId++;
    windows[id] = window;
    return id;
}

SDL_Window* SDL2_ResourceManager::getWindow(int handle) {
    auto it = windows.find(handle);
    return (it != windows.end()) ? it->second : nullptr;
}

void SDL2_ResourceManager::removeWindow(int handle) {
    windows.erase(handle);
}

int SDL2_ResourceManager::addRenderer(SDL_Renderer* renderer) {
    int id = nextRendererId++;
    renderers[id] = renderer;
    return id;
}

SDL_Renderer* SDL2_ResourceManager::getRenderer(int handle) {
    auto it = renderers.find(handle);
    return (it != renderers.end()) ? it->second : nullptr;
}

void SDL2_ResourceManager::removeRenderer(int handle) {
    renderers.erase(handle);
}

int SDL2_ResourceManager::addTexture(SDL_Texture* texture) {
    int id = nextTextureId++;
    textures[id] = texture;
    return id;
}

SDL_Texture* SDL2_ResourceManager::getTexture(int handle) {
    auto it = textures.find(handle);
    return (it != textures.end()) ? it->second : nullptr;
}

void SDL2_ResourceManager::removeTexture(int handle) {
    textures.erase(handle);
}

int SDL2_ResourceManager::addSurface(SDL_Surface* surface) {
    int id = nextSurfaceId++;
    surfaces[id] = surface;
    return id;
}

SDL_Surface* SDL2_ResourceManager::getSurface(int handle) {
    auto it = surfaces.find(handle);
    return (it != surfaces.end()) ? it->second : nullptr;
}

void SDL2_ResourceManager::removeSurface(int handle) {
    surfaces.erase(handle);
}

void SDL2_ResourceManager::cleanup() {
    // Clean up all textures
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
    
    // Clean up all surfaces
    for (auto& pair : surfaces) {
        if (pair.second) {
            SDL_FreeSurface(pair.second);
        }
    }
    surfaces.clear();
    
    // Clean up all renderers
    for (auto& pair : renderers) {
        if (pair.second) {
            SDL_DestroyRenderer(pair.second);
        }
    }
    renderers.clear();
    
    // Clean up all windows
    for (auto& pair : windows) {
        if (pair.second) {
            SDL_DestroyWindow(pair.second);
        }
    }
    windows.clear();
}

// Core SDL functions
int sdl_init(int flags) {
    return SDL_Init(flags);
}

void sdl_quit() {
    SDL2_ResourceManager::instance().cleanup();
    SDL_Quit();
}

std::string sdl_get_error() {
    const char* err = SDL_GetError();
    return err ? std::string(err) : std::string();
}

// Window functions
int sdl_create_window(const std::string& title, int x, int y, int w, int h, int flags) {
    SDL_Window* window = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    if (!window) {
        return -1;
    }
    return SDL2_ResourceManager::instance().addWindow(window);
}

void sdl_destroy_window(int window_handle) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (window) {
        SDL_DestroyWindow(window);
        SDL2_ResourceManager::instance().removeWindow(window_handle);
    }
}

void sdl_set_window_title(int window_handle, const std::string& title) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (window) {
        SDL_SetWindowTitle(window, title.c_str());
    }
}

void sdl_set_window_size(int window_handle, int w, int h) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (window) {
        SDL_SetWindowSize(window, w, h);
    }
}

void sdl_set_window_position(int window_handle, int x, int y) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (window) {
        SDL_SetWindowPosition(window, x, y);
    }
}

void sdl_show_window(int window_handle) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (window) {
        SDL_ShowWindow(window);
    }
}

void sdl_hide_window(int window_handle) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (window) {
        SDL_HideWindow(window);
    }
}

// Renderer functions
int sdl_create_renderer(int window_handle, int index, int flags) {
    SDL_Window* window = SDL2_ResourceManager::instance().getWindow(window_handle);
    if (!window) {
        return -1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, index, flags);
    if (!renderer) {
        return -1;
    }
    return SDL2_ResourceManager::instance().addRenderer(renderer);
}

void sdl_destroy_renderer(int renderer_handle) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        SDL2_ResourceManager::instance().removeRenderer(renderer_handle);
    }
}

void sdl_render_clear(int renderer_handle) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_RenderClear(renderer);
    }
}

void sdl_render_present(int renderer_handle) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_RenderPresent(renderer);
    }
}

void sdl_set_render_draw_color(int renderer_handle, int r, int g, int b, int a) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }
}

void sdl_get_render_draw_color(int renderer_handle, int* r, int* g, int* b, int* a) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 ur, ug, ub, ua;
        SDL_GetRenderDrawColor(renderer, &ur, &ug, &ub, &ua);
        if (r) *r = ur;
        if (g) *g = ug;
        if (b) *b = ub;
        if (a) *a = ua;
    }
}

// Drawing primitives
void sdl_render_draw_point(int renderer_handle, int x, int y) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void sdl_render_draw_line(int renderer_handle, int x1, int y1, int x2, int y2) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void sdl_render_draw_rect(int renderer_handle, int x, int y, int w, int h) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void sdl_render_fill_rect(int renderer_handle, int x, int y, int w, int h) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(renderer, &rect);
    }
}

// Advanced drawing (requires SDL2_gfx)
#ifdef SDL2_GFX_AVAILABLE
void sdl_render_draw_circle(int renderer_handle, int x, int y, int radius) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        circleRGBA(renderer, x, y, radius, r, g, b, a);
    }
}

void sdl_render_fill_circle(int renderer_handle, int x, int y, int radius) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        filledCircleRGBA(renderer, x, y, radius, r, g, b, a);
    }
}

void sdl_render_draw_ellipse(int renderer_handle, int x, int y, int rx, int ry) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        ellipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
    }
}

void sdl_render_fill_ellipse(int renderer_handle, int x, int y, int rx, int ry) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        filledEllipseRGBA(renderer, x, y, rx, ry, r, g, b, a);
    }
}

void sdl_render_draw_triangle(int renderer_handle, int x1, int y1, int x2, int y2, int x3, int y3) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        trigonRGBA(renderer, x1, y1, x2, y2, x3, y3, r, g, b, a);
    }
}

void sdl_render_fill_triangle(int renderer_handle, int x1, int y1, int x2, int y2, int x3, int y3) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (renderer) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
        filledTrigonRGBA(renderer, x1, y1, x2, y2, x3, y3, r, g, b, a);
    }
}
#endif

// Texture functions
int sdl_create_texture_from_surface(int renderer_handle, int surface_handle) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    SDL_Surface* surface = SDL2_ResourceManager::instance().getSurface(surface_handle);
    
    if (!renderer || !surface) {
        return -1;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        return -1;
    }
    
    return SDL2_ResourceManager::instance().addTexture(texture);
}

int sdl_load_texture(int renderer_handle, const std::string& filename) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    if (!renderer) {
        return -1;
    }
    
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        return -1;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        return -1;
    }
    
    return SDL2_ResourceManager::instance().addTexture(texture);
}

void sdl_destroy_texture(int texture_handle) {
    SDL_Texture* texture = SDL2_ResourceManager::instance().getTexture(texture_handle);
    if (texture) {
        SDL_DestroyTexture(texture);
        SDL2_ResourceManager::instance().removeTexture(texture_handle);
    }
}

void sdl_render_copy(int renderer_handle, int texture_handle, int src_x, int src_y, int src_w, int src_h,
                     int dst_x, int dst_y, int dst_w, int dst_h) {
    SDL_Renderer* renderer = SDL2_ResourceManager::instance().getRenderer(renderer_handle);
    SDL_Texture* texture = SDL2_ResourceManager::instance().getTexture(texture_handle);
    
    if (!renderer || !texture) {
        return;
    }
    
    SDL_Rect src_rect = {src_x, src_y, src_w, src_h};
    SDL_Rect dst_rect = {dst_x, dst_y, dst_w, dst_h};
    
    SDL_Rect* src_ptr = (src_w > 0 && src_h > 0) ? &src_rect : nullptr;
    SDL_Rect* dst_ptr = (dst_w > 0 && dst_h > 0) ? &dst_rect : nullptr;
    
    SDL_RenderCopy(renderer, texture, src_ptr, dst_ptr);
}

void sdl_query_texture(int texture_handle, int* format, int* access, int* w, int* h) {
    SDL_Texture* texture = SDL2_ResourceManager::instance().getTexture(texture_handle);
    if (texture) {
        Uint32 fmt;
        int acc, width, height;
        SDL_QueryTexture(texture, &fmt, &acc, &width, &height);
        if (format) *format = fmt;
        if (access) *access = acc;
        if (w) *w = width;
        if (h) *h = height;
    }
}

// Surface functions
int sdl_load_bmp(const std::string& filename) {
    SDL_Surface* surface = SDL_LoadBMP(filename.c_str());
    if (!surface) {
        return -1;
    }
    return SDL2_ResourceManager::instance().addSurface(surface);
}

void sdl_free_surface(int surface_handle) {
    SDL_Surface* surface = SDL2_ResourceManager::instance().getSurface(surface_handle);
    if (surface) {
        SDL_FreeSurface(surface);
        SDL2_ResourceManager::instance().removeSurface(surface_handle);
    }
}

// Event functions
int sdl_poll_event() {
    event_available = (SDL_PollEvent(&last_event) == 1);
    return event_available ? 1 : 0;
}

int sdl_get_event_type() {
    return event_available ? last_event.type : 0;
}

int sdl_get_key_scancode() {
    if (event_available && (last_event.type == SDL_KEYDOWN || last_event.type == SDL_KEYUP)) {
        return last_event.key.keysym.scancode;
    }
    return 0;
}

int sdl_get_key_keycode() {
    if (event_available && (last_event.type == SDL_KEYDOWN || last_event.type == SDL_KEYUP)) {
        return last_event.key.keysym.sym;
    }
    return 0;
}

int sdl_get_mouse_x() {
    if (event_available && (last_event.type == SDL_MOUSEMOTION || 
                           last_event.type == SDL_MOUSEBUTTONDOWN || 
                           last_event.type == SDL_MOUSEBUTTONUP)) {
        if (last_event.type == SDL_MOUSEMOTION) {
            return last_event.motion.x;
        } else {
            return last_event.button.x;
        }
    }
    return 0;
}

int sdl_get_mouse_y() {
    if (event_available && (last_event.type == SDL_MOUSEMOTION || 
                           last_event.type == SDL_MOUSEBUTTONDOWN || 
                           last_event.type == SDL_MOUSEBUTTONUP)) {
        if (last_event.type == SDL_MOUSEMOTION) {
            return last_event.motion.y;
        } else {
            return last_event.button.y;
        }
    }
    return 0;
}

int sdl_get_mouse_button() {
    if (event_available && (last_event.type == SDL_MOUSEBUTTONDOWN || 
                           last_event.type == SDL_MOUSEBUTTONUP)) {
        return last_event.button.button;
    }
    return 0;
}

int sdl_get_mouse_state(int* x, int* y) {
    return SDL_GetMouseState(x, y);
}

// Delay function
void sdl_delay(int ms) {
    SDL_Delay(ms);
}

} // namespace basic_runtime

#endif // SDL2_SUPPORT_ENABLED
