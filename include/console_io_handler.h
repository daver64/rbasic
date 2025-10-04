#pragma once

#include "io_handler.h"
#include <iostream>

namespace rbasic {

// Console-based I/O handler (traditional behavior)
class ConsoleIOHandler : public IOHandler {
private:
    bool graphics_mode_active = false;
    
public:
    ConsoleIOHandler() = default;
    ~ConsoleIOHandler() override = default;
    
    // Text output methods
    void print(const std::string& text) override;
    void println(const std::string& text) override;
    void newline() override;
    
    // Text input methods
    std::string input() override;
    std::string input(const std::string& prompt) override;
    
    // Graphics methods (no-op for console)
    void graphics_mode(int width, int height) override;
    void text_mode() override;
    void clear_screen() override;
    void set_color(int r, int g, int b) override;
    void draw_pixel(int x, int y) override;
    void draw_line(int x1, int y1, int x2, int y2) override;
    void draw_rect(int x, int y, int width, int height, bool filled = false) override;
    void draw_circle(int x, int y, int radius, bool filled = false) override;
    void draw_text(int x, int y, const std::string& text) override;
    void print_at(int x, int y, const std::string& text) override;
    void refresh_screen() override;
    
    // Input handling methods (limited for console)
    bool key_pressed(const std::string& key) override;
    bool mouse_clicked() override;
    void get_mouse_pos(int& x, int& y) override;
    bool quit_requested() override;
    
    // Audio methods (no-op for console)
    void play_sound(const std::string& filename) override;
    void play_tone(int frequency, int duration_ms) override;
    
    // Utility methods
    void sleep_ms(int milliseconds) override;
    int get_ticks() override;
};

} // namespace rbasic