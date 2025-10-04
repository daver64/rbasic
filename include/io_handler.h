#pragma once

#include <string>
#include <memory>

namespace rbasic {

// Abstract interface for I/O operations
// Allows different implementations (console, SDL, web, etc.)
class IOHandler {
public:
    virtual ~IOHandler() = default;
    
    // Text output methods
    virtual void print(const std::string& text) = 0;
    virtual void println(const std::string& text) = 0;
    virtual void newline() = 0;
    
    // Text input methods
    virtual std::string input() = 0;
    virtual std::string input(const std::string& prompt) = 0;
    
    // Graphics methods (for SDL/graphics implementations)
    virtual void graphics_mode(int width, int height) = 0;
    virtual void text_mode() = 0;
    virtual void clear_screen() = 0;
    virtual void set_colour(int r, int g, int b) = 0;
    virtual void draw_pixel(int x, int y) = 0;
    virtual void draw_line(int x1, int y1, int x2, int y2) = 0;
    virtual void draw_rect(int x, int y, int width, int height, bool filled = false) = 0;
    virtual void draw_circle(int x, int y, int radius, bool filled = false) = 0;
    virtual void draw_text(int x, int y, const std::string& text) = 0;
    virtual void print_at(int x, int y, const std::string& text) = 0;
    virtual void refresh_screen() = 0;
    
    // Input handling methods
    virtual bool key_pressed(const std::string& key) = 0;
    virtual bool mouse_clicked() = 0;
    virtual void get_mouse_pos(int& x, int& y) = 0;
    virtual bool quit_requested() = 0;
    
    // Audio methods (for future expansion)
    virtual void play_sound(const std::string& filename) = 0;
    virtual void play_tone(int frequency, int duration_ms) = 0;
    
    // Utility methods
    virtual void sleep_ms(int milliseconds) = 0;
    virtual int get_ticks() = 0; // Get time in milliseconds since start
};

// Factory function to create appropriate IOHandler
std::unique_ptr<IOHandler> createIOHandler(const std::string& type = "console");

} // namespace rbasic