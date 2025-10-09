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

// Terminal state structure for proper encapsulation
struct TerminalState {
    bool initialized = false;
    bool colorSupported = false;
    
#ifdef _WIN32
    void* hConsole = nullptr;
    void* hStdin = nullptr;
    unsigned long originalConsoleMode = 0;
    int savedCursorRow = 0;
    int savedCursorCol = 0;
#else
    struct termios originalTermios;
    bool termiosWasSaved = false;
    int savedCursorRow = 0;
    int savedCursorCol = 0;
#endif
};

// Thread-safe singleton for terminal state
TerminalState& getTerminalState() {
    static TerminalState state;
    return state;
}

bool Terminal::initialize() {
    TerminalState& state = getTerminalState();
    
    if (state.initialized) {
        return true;
    }
    
#ifdef _WIN32
    // Windows Console API initialization
    state.hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    state.hStdin = GetStdHandle(STD_INPUT_HANDLE);
    
    if (state.hConsole == INVALID_HANDLE_VALUE || state.hStdin == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    // Enable virtual terminal processing for ANSI escape sequences
    DWORD consoleMode = 0;
    if (GetConsoleMode(state.hConsole, &consoleMode)) {
        if (SetConsoleMode(state.hConsole, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
            state.colorSupported = true;
        } else {
            // If ANSI escape sequences fail, we can still use Windows Console API
            state.colorSupported = true;
        }
    } else {
        // Even if we can't get/set console mode, we can still try Windows Console API
        state.colorSupported = true;
    }
    
    // Save original input mode
    GetConsoleMode(state.hStdin, &state.originalConsoleMode);
    
#else
    // Linux/macOS: Check for color support via environment
    const char* term = getenv("TERM");
    if (term && (strstr(term, "color") || strstr(term, "xterm") || strstr(term, "screen"))) {
        state.colorSupported = true;
    }
    
    // Check if stdout is a terminal
    if (isatty(STDOUT_FILENO)) {
        state.colorSupported = true;
    }
    
    // Save original terminal attributes for proper cleanup
    if (tcgetattr(STDIN_FILENO, &state.originalTermios) == 0) {
        state.termiosWasSaved = true;
    }
#endif

    state.initialized = true;
    return true;
}

void Terminal::cleanup() {
    TerminalState& state = getTerminalState();
    
    if (!state.initialized) {
        return;
    }
    
    resetColor();
    showCursor(true);
    setEcho(true);
    
#ifdef _WIN32
    // Restore original console mode
    if (state.hStdin && state.originalConsoleMode != 0) {
        SetConsoleMode(state.hStdin, state.originalConsoleMode);
    }
#else
    // Reset terminal state and restore original attributes
    std::cout << "\033[0m" << std::flush; // Reset all attributes
    
    // Restore original terminal attributes if we saved them
    if (state.termiosWasSaved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &state.originalTermios);
        state.termiosWasSaved = false;
    }
#endif

    state.initialized = false;
}

bool Terminal::supportsColor() {
    TerminalState& state = getTerminalState();
    return state.colorSupported;
}

void Terminal::clear() {
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    
    if (!GetConsoleScreenBufferInfo(state.hConsole, &csbi)) {
        return;
    }
    
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    
    if (!FillConsoleOutputCharacter(state.hConsole, ' ', dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    
    if (!GetConsoleScreenBufferInfo(state.hConsole, &csbi)) {
        return;
    }
    
    if (!FillConsoleOutputAttribute(state.hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }
    
    SetConsoleCursorPosition(state.hConsole, coordScreen);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

void Terminal::setCursor(int row, int col) {
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    COORD coord = {static_cast<SHORT>(col), static_cast<SHORT>(row)};
    SetConsoleCursorPosition(state.hConsole, coord);
#else
    std::cout << "\033[" << (row + 1) << ";" << (col + 1) << "H" << std::flush;
#endif
}

void Terminal::getCursor(int& row, int& col) {
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(state.hConsole, &csbi)) {
        row = csbi.dwCursorPosition.Y;
        col = csbi.dwCursorPosition.X;
    } else {
        row = col = 0;
    }
#else
    // For Linux, we need to use raw terminal I/O to avoid interfering with stdout
    // Save terminal settings
    struct termios oldTermios;
    if (tcgetattr(STDIN_FILENO, &oldTermios) != 0) {
        row = col = 0;
        return;
    }
    
    // Set terminal to raw mode for reading response
    struct termios newTermios = oldTermios;
    newTermios.c_lflag &= ~(ICANON | ECHO);
    newTermios.c_cc[VMIN] = 0;
    newTermios.c_cc[VTIME] = 1; // 100ms timeout
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newTermios) != 0) {
        row = col = 0;
        return;
    }
    
    // Request cursor position using write() to avoid stdout interference
    if (write(STDOUT_FILENO, "\033[6n", 4) != 4) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
        row = col = 0;
        return;
    }
    
    // Read response character by character using read()
    char buffer[32];
    int bufferPos = 0;
    char ch;
    
    // Look for ESC[ sequence start
    while (bufferPos < 31) {
        if (read(STDIN_FILENO, &ch, 1) != 1) break;
        buffer[bufferPos++] = ch;
        
        // Stop when we get the 'R' terminator
        if (ch == 'R') break;
    }
    
    // Restore terminal settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
    
    buffer[bufferPos] = '\0';
    
    // Parse response: ESC[row;colR
    if (bufferPos >= 6 && buffer[0] == '\033' && buffer[1] == '[') {
        char* semicolon = strchr(buffer + 2, ';');
        if (semicolon) {
            *semicolon = '\0';
            row = atoi(buffer + 2) - 1; // Convert to 0-based
            col = atoi(semicolon + 1) - 1;
        } else {
            row = col = 0;
        }
    } else {
        row = col = 0;
    }
#endif
}

void Terminal::saveCursor() {
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(state.hConsole, &csbi)) {
        state.savedCursorRow = csbi.dwCursorPosition.Y;
        state.savedCursorCol = csbi.dwCursorPosition.X;
    }
#else
    // For Linux, get the current cursor position and save it
    getCursor(state.savedCursorRow, state.savedCursorCol);
#endif
}

void Terminal::restoreCursor() {
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    COORD coord = {static_cast<SHORT>(state.savedCursorCol), static_cast<SHORT>(state.savedCursorRow)};
    SetConsoleCursorPosition(state.hConsole, coord);
#else
    // For Linux, set cursor to saved position
    setCursor(state.savedCursorRow, state.savedCursorCol);
#endif
}

void Terminal::setColor(Color foreground, Color background) {
    TerminalState& state = getTerminalState();
    
    if (!state.colorSupported) {
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
    
    SetConsoleTextAttribute(state.hConsole, attributes);
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
    TerminalState& state = getTerminalState();
    
    if (!state.colorSupported) {
        return;
    }
    
#ifdef _WIN32
    SetConsoleTextAttribute(state.hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(state.hConsole, &csbi)) {
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
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(state.hConsole, &cursorInfo)) {
        cursorInfo.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(state.hConsole, &cursorInfo);
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
    TerminalState& state = getTerminalState();
    
#ifdef _WIN32
    DWORD mode;
    if (GetConsoleMode(state.hStdin, &mode)) {
        if (enabled) {
            mode |= ENABLE_ECHO_INPUT;
        } else {
            mode &= ~ENABLE_ECHO_INPUT;
        }
        SetConsoleMode(state.hStdin, mode);
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