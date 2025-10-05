#include "terminal.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#endif

namespace rbasic {

bool Terminal::initialized = false;
bool Terminal::colorSupported = false;

#ifdef _WIN32
void* Terminal::hConsole = nullptr;
void* Terminal::hStdin = nullptr;
unsigned long Terminal::originalConsoleMode = 0;
#endif

bool Terminal::initialize() {
    if (initialized) {
        return true;
    }
    
#ifdef _WIN32
    // Windows Console API initialization
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    
    if (hConsole == INVALID_HANDLE_VALUE || hStdin == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    // Enable virtual terminal processing for ANSI escape sequences
    DWORD consoleMode = 0;
    if (GetConsoleMode(hConsole, &consoleMode)) {
        if (SetConsoleMode(hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            colorSupported = true;
        } else {
            // If ANSI escape sequences fail, we can still use Windows Console API
            colorSupported = true;
        }
    } else {
        // Even if we can't get/set console mode, we can still try Windows Console API
        colorSupported = true;
    }
    
    // Save original input mode
    GetConsoleMode(hStdin, &originalConsoleMode);
    
#else
    // Linux/macOS: Check for color support via environment
    const char* term = getenv("TERM");
    if (term && (strstr(term, "color") || strstr(term, "xterm") || strstr(term, "screen"))) {
        colorSupported = true;
    }
    
    // Check if stdout is a terminal
    if (isatty(STDOUT_FILENO)) {
        colorSupported = true;
    }
#endif

    initialized = true;
    return true;
}

void Terminal::cleanup() {
    if (!initialized) {
        return;
    }
    
    resetColor();
    showCursor(true);
    setEcho(true);
    
#ifdef _WIN32
    // Restore original console mode
    if (hStdin && originalConsoleMode != 0) {
        SetConsoleMode(hStdin, originalConsoleMode);
    }
#else
    // Reset terminal state
    std::cout << "\033[0m" << std::flush; // Reset all attributes
#endif

    initialized = false;
}

bool Terminal::supportsColor() {
    return colorSupported;
}

void Terminal::clear() {
#ifdef _WIN32
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }
    
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    
    if (!FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }
    
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    
    SetConsoleCursorPosition(hConsole, coordScreen);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

void Terminal::setCursor(int row, int col) {
#ifdef _WIN32
    COORD coord = {static_cast<SHORT>(col), static_cast<SHORT>(row)};
    SetConsoleCursorPosition(hConsole, coord);
#else
    std::cout << "\033[" << (row + 1) << ";" << (col + 1) << "H" << std::flush;
#endif
}

void Terminal::getCursor(int& row, int& col) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        row = csbi.dwCursorPosition.Y;
        col = csbi.dwCursorPosition.X;
    } else {
        row = col = 0;
    }
#else
    // ANSI escape sequence to get cursor position
    std::cout << "\033[6n" << std::flush;
    
    // Read the response: ESC[row;colR
    char ch;
    std::string response;
    while (std::cin.get(ch) && ch != 'R') {
        response += ch;
    }
    
    // Parse the response
    size_t semicolon = response.find(';');
    if (semicolon != std::string::npos) {
        row = std::stoi(response.substr(2, semicolon - 2)) - 1; // Convert to 0-based
        col = std::stoi(response.substr(semicolon + 1)) - 1;
    } else {
        row = col = 0;
    }
#endif
}

void Terminal::setColor(Color foreground, Color background) {
    if (!colorSupported) {
        return;
    }
    
#ifdef _WIN32
    WORD attributes = 0;
    
    // Set foreground color
    if (foreground != Color::DEFAULT) {
        int fg = static_cast<int>(foreground);
        if (fg >= 8) {
            attributes |= FOREGROUND_INTENSITY;
            fg -= 8;
        }
        if (fg & 1) attributes |= FOREGROUND_RED;
        if (fg & 2) attributes |= FOREGROUND_GREEN;
        if (fg & 4) attributes |= FOREGROUND_BLUE;
    } else {
        attributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // White
    }
    
    // Set background color
    if (background != Color::DEFAULT) {
        int bg = static_cast<int>(background);
        if (bg >= 8) {
            attributes |= BACKGROUND_INTENSITY;
            bg -= 8;
        }
        if (bg & 1) attributes |= BACKGROUND_RED;
        if (bg & 2) attributes |= BACKGROUND_GREEN;
        if (bg & 4) attributes |= BACKGROUND_BLUE;
    }
    
    SetConsoleTextAttribute(hConsole, attributes);
#else
    std::ostringstream oss;
    oss << "\033[";
    
    if (foreground != Color::DEFAULT) {
        int fg = static_cast<int>(foreground);
        if (fg >= 8) {
            oss << "1;"; // Bold/bright
            fg -= 8;
        }
        oss << (30 + fg);
    }
    
    if (background != Color::DEFAULT) {
        if (foreground != Color::DEFAULT) oss << ";";
        int bg = static_cast<int>(background);
        if (bg >= 8) bg -= 8; // Background doesn't have bright variants in basic ANSI
        oss << (40 + bg);
    }
    
    oss << "m";
    std::cout << oss.str() << std::flush;
#endif
}

void Terminal::resetColor() {
    if (!colorSupported) {
        return;
    }
    
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    std::cout << "\033[0m" << std::flush;
#endif
}

void Terminal::print(const std::string& text, Color foreground, Color background) {
    if (foreground != Color::DEFAULT || background != Color::DEFAULT) {
        setColor(foreground, background);
        std::cout << text << std::flush;
        resetColor();
    } else {
        std::cout << text << std::flush;
    }
}

void Terminal::println(const std::string& text, Color foreground, Color background) {
    print(text + "\n", foreground, background);
}

void Terminal::getSize(int& rows, int& cols) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    } else {
        rows = 24;
        cols = 80;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        rows = w.ws_row;
        cols = w.ws_col;
    } else {
        rows = 24;
        cols = 80;
    }
#endif
}

bool Terminal::kbhit() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    // Linux implementation using select()
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    return select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &tv) > 0;
#endif
}

int Terminal::getch() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

std::string Terminal::getline(const std::string& prompt, Color promptColor) {
    if (!prompt.empty()) {
        print(prompt, promptColor);
    }
    
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void Terminal::showCursor(bool visible) {
#ifdef _WIN32
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(hConsole, &cursorInfo)) {
        cursorInfo.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }
#else
    if (visible) {
        std::cout << "\033[?25h" << std::flush; // Show cursor
    } else {
        std::cout << "\033[?25l" << std::flush; // Hide cursor
    }
#endif
}

void Terminal::setEcho(bool enabled) {
#ifdef _WIN32
    DWORD mode;
    if (GetConsoleMode(hStdin, &mode)) {
        if (enabled) {
            mode |= ENABLE_ECHO_INPUT;
        } else {
            mode &= ~ENABLE_ECHO_INPUT;
        }
        SetConsoleMode(hStdin, mode);
    }
#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    
    if (enabled) {
        tty.c_lflag |= ECHO;
    } else {
        tty.c_lflag &= ~ECHO;
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

} // namespace rbasic