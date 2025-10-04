#include "console_io_handler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

namespace rbasic {

// Text output methods
void ConsoleIOHandler::print(const std::string& text) {
    std::cout << text;
    std::cout.flush();
}

void ConsoleIOHandler::println(const std::string& text) {
    std::cout << text << std::endl;
}

void ConsoleIOHandler::newline() {
    std::cout << std::endl;
}

// Text input methods
std::string ConsoleIOHandler::input() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::string ConsoleIOHandler::input(const std::string& prompt) {
    std::cout << prompt;
    std::cout.flush();
    return input();
}

// Graphics methods (console implementations where possible)
void ConsoleIOHandler::graphics_mode(int width, int height) {
    graphics_mode_active = true;
    std::cout << "Graphics mode " << width << "x" << height << " (console simulation)" << std::endl;
}

void ConsoleIOHandler::text_mode() {
    graphics_mode_active = false;
    std::cout << "Switched to text mode" << std::endl;
}

void ConsoleIOHandler::clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleIOHandler::set_colour(int r, int g, int b) {
    // Console colour support is limited, could use ANSI codes
    std::cout << "[Colour: RGB(" << r << "," << g << "," << b << ")]";
}

void ConsoleIOHandler::draw_pixel(int x, int y) {
    std::cout << "[Pixel at (" << x << "," << y << ")]";
}

void ConsoleIOHandler::draw_line(int x1, int y1, int x2, int y2) {
    std::cout << "[Line from (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ")]";
}

void ConsoleIOHandler::draw_rect(int x, int y, int width, int height, bool filled) {
    std::cout << "[" << (filled ? "Filled " : "") << "Rectangle at (" << x << "," << y 
              << ") size " << width << "x" << height << "]";
}

void ConsoleIOHandler::draw_circle(int x, int y, int radius, bool filled) {
    std::cout << "[" << (filled ? "Filled " : "") << "Circle at (" << x << "," << y 
              << ") radius " << radius << "]";
}

void ConsoleIOHandler::print_at(int x, int y, const std::string& text) {
    std::cout << "[Text at (" << x << "," << y << "): " << text << "]";
}

void ConsoleIOHandler::draw_text(int x, int y, const std::string& text) {
    // draw_text is just an alias for print_at in console mode
    print_at(x, y, text);
}

void ConsoleIOHandler::refresh_screen() {
    std::cout.flush();
}

// Input handling methods
bool ConsoleIOHandler::key_pressed(const std::string& key) {
#ifdef _WIN32
    if (key == "SPACE") return GetAsyncKeyState(VK_SPACE) & 0x8000;
    if (key == "ENTER") return GetAsyncKeyState(VK_RETURN) & 0x8000;
    if (key == "ESC") return GetAsyncKeyState(VK_ESCAPE) & 0x8000;
    if (key.length() == 1) {
        char c = key[0];
        if (c >= 'A' && c <= 'Z') return GetAsyncKeyState(c) & 0x8000;
        if (c >= 'a' && c <= 'z') return GetAsyncKeyState(c - 32) & 0x8000;
    }
#endif
    return false; // Limited console key detection
}

bool ConsoleIOHandler::mouse_clicked() {
    return false; // No mouse support in console
}

void ConsoleIOHandler::get_mouse_pos(int& x, int& y) {
    x = y = 0; // No mouse support in console
}

bool ConsoleIOHandler::quit_requested() {
    return false; // Console mode doesn't track quit requests
}

// Audio methods
void ConsoleIOHandler::play_sound(const std::string& filename) {
    std::cout << "[Playing sound: " << filename << "]" << std::endl;
}

void ConsoleIOHandler::play_tone(int frequency, int duration_ms) {
    std::cout << "[Playing tone: " << frequency << "Hz for " << duration_ms << "ms]" << std::endl;
#ifdef _WIN32
    Beep(frequency, duration_ms);
#endif
}

// Utility methods
void ConsoleIOHandler::sleep_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int ConsoleIOHandler::get_ticks() {
    static auto start_time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return static_cast<int>(duration.count());
}

} // namespace rbasic