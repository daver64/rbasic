#pragma once

#include <string>

namespace rbasic {

// Terminal colour constants
enum class Colour {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7,
    BRIGHT_BLACK = 8,
    BRIGHT_RED = 9,
    BRIGHT_GREEN = 10,
    BRIGHT_YELLOW = 11,
    BRIGHT_BLUE = 12,
    BRIGHT_MAGENTA = 13,
    BRIGHT_CYAN = 14,
    BRIGHT_WHITE = 15,
    DEFAULT = -1
};

// Terminal interface for cross-platform coloured output and input
class Terminal {
public:
    // Initialize terminal (setup colours, etc.)
    static bool initialize();
    
    // Cleanup terminal (restore original state)
    static void cleanup();
    
    // Check if terminal supports colours
    static bool supportsColour();
    
    // Clear screen
    static void clear();
    
    // Move cursor to position (0,0 is top-left)
    static void setCursor(int row, int col);
    
    // Get current cursor position
    static void getCursor(int& row, int& col);
    
    // Save/restore cursor position for mixing with regular output
    static void saveCursor();
    static void restoreCursor();
    
    // Set text colours
    static void setColour(Colour foreground, Colour background = Colour::DEFAULT);
    
    // Reset colours to default
    static void resetColour();
    
    // Output coloured text
    static void print(const std::string& text, Colour foreground = Colour::DEFAULT, Colour background = Colour::DEFAULT);
    
    // Print with newline
    static void println(const std::string& text = "", Colour foreground = Colour::DEFAULT, Colour background = Colour::DEFAULT);
    
    // Get terminal size
    static void getSize(int& rows, int& cols);
    
    // Check if a key is pressed (non-blocking)
    static bool kbhit();
    
    // Get a single character (blocking)
    static int getch();
    
    // Get a string with optional prompt and colour
    static std::string getline(const std::string& prompt = "", Colour promptColour = Colour::DEFAULT);
    
    // Enable/disable cursor visibility
    static void showCursor(bool visible);
    
    // Enable/disable echo (for password input)
    static void setEcho(bool enabled);

private:
    // All state is now encapsulated in TerminalState (see terminal.cpp)
    // No static members needed - using thread-safe singleton pattern
};

} // namespace rbasic