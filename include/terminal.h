#pragma once

#include <string>

namespace rbasic {

// Terminal color constants
enum class Color {
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

// Terminal interface for cross-platform colored output and input
class Terminal {
public:
    // Initialize terminal (setup colors, etc.)
    static bool initialize();
    
    // Cleanup terminal (restore original state)
    static void cleanup();
    
    // Check if terminal supports colors
    static bool supportsColor();
    
    // Clear screen
    static void clear();
    
    // Move cursor to position (0,0 is top-left)
    static void setCursor(int row, int col);
    
    // Get current cursor position
    static void getCursor(int& row, int& col);
    
    // Save/restore cursor position for mixing with regular output
    static void saveCursor();
    static void restoreCursor();
    
    // Set text colors
    static void setColor(Color foreground, Color background = Color::DEFAULT);
    
    // Reset colors to default
    static void resetColor();
    
    // Output colored text
    static void print(const std::string& text, Color foreground = Color::DEFAULT, Color background = Color::DEFAULT);
    
    // Print with newline
    static void println(const std::string& text = "", Color foreground = Color::DEFAULT, Color background = Color::DEFAULT);
    
    // Get terminal size
    static void getSize(int& rows, int& cols);
    
    // Check if a key is pressed (non-blocking)
    static bool kbhit();
    
    // Get a single character (blocking)
    static int getch();
    
    // Get a string with optional prompt and color
    static std::string getline(const std::string& prompt = "", Color promptColor = Color::DEFAULT);
    
    // Enable/disable cursor visibility
    static void showCursor(bool visible);
    
    // Enable/disable echo (for password input)
    static void setEcho(bool enabled);

private:
    static bool initialized;
    static bool colorSupported;
    
#ifdef _WIN32
    static void* hConsole;
    static void* hStdin;
    static unsigned long originalConsoleMode;
#endif
};

} // namespace rbasic