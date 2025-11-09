// Terminal Color and Cursor Control Demo
// Demonstrates rbasic's built-in cross-platform terminal features

// Colour constants (from Colour enum in terminal.h)
var COLOUR_BLACK = 0;
var COLOUR_RED = 1;
var COLOUR_GREEN = 2;
var COLOUR_YELLOW = 3;
var COLOUR_BLUE = 4;
var COLOUR_MAGENTA = 5;
var COLOUR_CYAN = 6;
var COLOUR_WHITE = 7;
var COLOUR_BRIGHT_BLACK = 8;
var COLOUR_BRIGHT_RED = 9;
var COLOUR_BRIGHT_GREEN = 10;
var COLOUR_BRIGHT_YELLOW = 11;
var COLOUR_BRIGHT_BLUE = 12;
var COLOUR_BRIGHT_MAGENTA = 13;
var COLOUR_BRIGHT_CYAN = 14;
var COLOUR_BRIGHT_WHITE = 15;
var COLOUR_DEFAULT = -1;

// Initialize terminal
terminal_init();

// Check if colors are supported
if (terminal_supports_colour()) {
    terminal_println("Terminal colour support: ENABLED", COLOUR_BRIGHT_GREEN, COLOUR_DEFAULT);
} else {
    print("Terminal colour support: DISABLED");
}

print("");

// Demonstrate basic colors
terminal_println("=== Basic Color Palette ===", COLOUR_BRIGHT_WHITE, COLOUR_DEFAULT);
terminal_print("BLACK ", COLOUR_BLACK, COLOUR_WHITE);
terminal_print("RED ", COLOUR_RED, COLOUR_DEFAULT);
terminal_print("GREEN ", COLOUR_GREEN, COLOUR_DEFAULT);
terminal_print("YELLOW ", COLOUR_YELLOW, COLOUR_DEFAULT);
terminal_print("BLUE ", COLOUR_BLUE, COLOUR_DEFAULT);
terminal_print("MAGENTA ", COLOUR_MAGENTA, COLOUR_DEFAULT);
terminal_print("CYAN ", COLOUR_CYAN, COLOUR_DEFAULT);
terminal_println("WHITE", COLOUR_WHITE, COLOUR_DEFAULT);

print("");

// Demonstrate bright colors
terminal_println("=== Bright Color Palette ===", COLOUR_BRIGHT_WHITE, COLOUR_DEFAULT);
terminal_print("BRIGHT_BLACK ", COLOUR_BRIGHT_BLACK, COLOUR_DEFAULT);
terminal_print("BRIGHT_RED ", COLOUR_BRIGHT_RED, COLOUR_DEFAULT);
terminal_print("BRIGHT_GREEN ", COLOUR_BRIGHT_GREEN, COLOUR_DEFAULT);
terminal_print("BRIGHT_YELLOW ", COLOUR_BRIGHT_YELLOW, COLOUR_DEFAULT);
terminal_println("", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("BRIGHT_BLUE ", COLOUR_BRIGHT_BLUE, COLOUR_DEFAULT);
terminal_print("BRIGHT_MAGENTA ", COLOUR_BRIGHT_MAGENTA, COLOUR_DEFAULT);
terminal_print("BRIGHT_CYAN ", COLOUR_BRIGHT_CYAN, COLOUR_DEFAULT);
terminal_println("BRIGHT_WHITE", COLOUR_BRIGHT_WHITE, COLOUR_DEFAULT);

print("");

// Demonstrate background colors
terminal_println("=== Background Colors ===", COLOUR_BRIGHT_WHITE, COLOUR_DEFAULT);
terminal_print("  Red BG  ", COLOUR_WHITE, COLOUR_RED);
terminal_print("  Green BG  ", COLOUR_BLACK, COLOUR_GREEN);
terminal_print("  Blue BG  ", COLOUR_WHITE, COLOUR_BLUE);
terminal_println("  Yellow BG  ", COLOUR_BLACK, COLOUR_YELLOW);

print("");

// Get terminal size
var rows = terminal_get_rows();
var cols = terminal_get_cols();
terminal_print("Terminal size: ", COLOUR_CYAN, COLOUR_DEFAULT);
print(str(rows) + " rows x " + str(cols) + " columns");

print("");

// Demonstrate cursor positioning
terminal_println("=== Cursor Control Demo ===", COLOUR_BRIGHT_YELLOW, COLOUR_DEFAULT);
print("Moving cursor to position (5, 10) and back...");

// Save current cursor position
terminal_save_cursor();

// Move cursor and draw
terminal_set_cursor(5, 10);
terminal_print("* MOVED *", COLOUR_BRIGHT_MAGENTA, COLOUR_DEFAULT);

// Restore cursor position
terminal_restore_cursor();
print("Cursor restored!");

print("");

// Create a colorful box
terminal_println("=== Drawing a Color Box ===", COLOUR_BRIGHT_YELLOW, COLOUR_DEFAULT);
print("");

for (var row = 0; row < 3; row = row + 1) {
    for (var col = 0; col < 8; col = col + 1) {
        var color = col + 1; // Colors 1-8
        terminal_print("  ", COLOUR_WHITE, color);
    }
    print("");
}

print("");

// Color fade demonstration
terminal_println("=== Color Intensity Demo ===", COLOUR_BRIGHT_YELLOW, COLOUR_DEFAULT);
terminal_print("Normal:  ", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("LOW", COLOUR_RED, COLOUR_DEFAULT);
terminal_print(" -> ", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_println("HIGH", COLOUR_BRIGHT_RED, COLOUR_DEFAULT);

terminal_print("Normal:  ", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("LOW", COLOUR_GREEN, COLOUR_DEFAULT);
terminal_print(" -> ", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_println("HIGH", COLOUR_BRIGHT_GREEN, COLOUR_DEFAULT);

terminal_print("Normal:  ", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("LOW", COLOUR_BLUE, COLOUR_DEFAULT);
terminal_print(" -> ", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_println("HIGH", COLOUR_BRIGHT_BLUE, COLOUR_DEFAULT);

print("");

// Status message example
terminal_print("[", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("SUCCESS", COLOUR_BRIGHT_GREEN, COLOUR_DEFAULT);
terminal_println("] Operation completed!", COLOUR_DEFAULT, COLOUR_DEFAULT);

terminal_print("[", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("WARNING", COLOUR_BRIGHT_YELLOW, COLOUR_DEFAULT);
terminal_println("] Check configuration!", COLOUR_DEFAULT, COLOUR_DEFAULT);

terminal_print("[", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("ERROR", COLOUR_BRIGHT_RED, COLOUR_DEFAULT);
terminal_println("] Something went wrong!", COLOUR_DEFAULT, COLOUR_DEFAULT);

terminal_print("[", COLOUR_DEFAULT, COLOUR_DEFAULT);
terminal_print("INFO", COLOUR_BRIGHT_CYAN, COLOUR_DEFAULT);
terminal_println("] Processing data...", COLOUR_DEFAULT, COLOUR_DEFAULT);

print("");

// Reset colors to default
terminal_reset_colour();
terminal_println("=== Demo Complete ===", COLOUR_BRIGHT_WHITE, COLOUR_DEFAULT);
print("All colors reset to default.");

// Cleanup terminal
terminal_cleanup();

print("");
print("Available terminal functions:");
print("  - terminal_init() / terminal_cleanup()");
print("  - terminal_supports_colour()");
print("  - terminal_clear()");
print("  - terminal_set_cursor(row, col)");
print("  - terminal_get_cursor_row() / terminal_get_cursor_col()");
print("  - terminal_save_cursor() / terminal_restore_cursor()");
print("  - terminal_set_colour(fg, bg)");
print("  - terminal_reset_colour()");
print("  - terminal_print(text, fg, bg)");
print("  - terminal_println(text, fg, bg)");
print("  - terminal_get_rows() / terminal_get_cols()");
print("  - terminal_show_cursor(visible)");
print("  - terminal_kbhit() / terminal_getch()");
print("  - terminal_getline(prompt, color)");
