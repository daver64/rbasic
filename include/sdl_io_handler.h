#pragma once

#include "io_handler.h"

#ifdef RBASIC_SDL_SUPPORT
#include <SDL.h>
#include <vector>
#include <string>
#include <queue>

namespace rbasic {

// SDL2-based I/O handler for graphics and GUI applications
class SDLIOHandler : public IOHandler {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool initialized = false;
    bool graphics_mode_active = false;
    bool should_quit = false;
    
    // Console area for text I/O within SDL window
    std::vector<std::string> console_lines;
    std::string current_input;
    bool input_active = false;
    std::queue<std::string> input_queue;
    
    // Graphics state
    SDL_Color current_color = {255, 255, 255, 255}; // White
    int window_width = 800;
    int window_height = 600;
    
    // Input state
    bool keys_pressed[SDL_NUM_SCANCODES] = {false};
    bool mouse_button_pressed = false;
    int mouse_x = 0, mouse_y = 0;
    
    // Timing
    Uint32 start_ticks;
    
    // Helper methods
    bool init_sdl();
    void cleanup_sdl();
    void handle_events();
    void render_console();
    void add_console_line(const std::string& line);
    
public:
    SDLIOHandler();
    ~SDLIOHandler() override;
    
    // Text output methods
    void print(const std::string& text) override;
    void println(const std::string& text) override;
    void newline() override;
    
    // Text input methods
    std::string input() override;
    std::string input(const std::string& prompt) override;
    
    // Graphics methods
    void graphics_mode(int width, int height) override;
    void text_mode() override;
    void clear_screen() override;
    void set_color(int r, int g, int b) override;
    void draw_pixel(int x, int y) override;
    void draw_line(int x1, int y1, int x2, int y2) override;
    void draw_rect(int x, int y, int width, int height, bool filled = false) override;
    void draw_circle(int x, int y, int radius, bool filled = false) override;
    void print_at(int x, int y, const std::string& text) override;
    void refresh_screen() override;
    
    // Input handling methods
    bool key_pressed(const std::string& key) override;
    bool mouse_clicked() override;
    void get_mouse_pos(int& x, int& y) override;
    bool quit_requested() override;
    
    // Audio methods (basic SDL audio)
    void play_sound(const std::string& filename) override;
    void play_tone(int frequency, int duration_ms) override;
    
    // Utility methods
    void sleep_ms(int milliseconds) override;
    int get_ticks() override;
};

} // namespace rbasic

#endif // RBASIC_SDL_SUPPORT