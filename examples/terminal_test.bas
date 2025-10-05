// Terminal support test for rbasic
print("Testing terminal support...");

// Test terminal initialization
var supported = terminal_supports_color();
print("Color support: " + str(supported));

// Test terminal size
var rows = terminal_get_rows();
var cols = terminal_get_cols();
print("Terminal size: " + str(cols) + "x" + str(rows));

// Test colored output
print("Testing colored output:");
terminal_print("Red text", 1, -1);
terminal_print(" ");
terminal_print("Green text", 2, -1);
terminal_print(" ");
terminal_print("Blue text", 4, -1);
terminal_println("");

// Test bright colors
terminal_print("Bright Red", 9, -1);
terminal_print(" ");
terminal_print("Bright Green", 10, -1);
terminal_print(" ");
terminal_print("Bright Blue", 12, -1);
terminal_println("");

// Test background colors
terminal_print("White on Red", 7, 1);
terminal_println("");
terminal_print("Yellow on Blue", 11, 4);
terminal_println("");

// Reset colors
terminal_reset_color();
print("Colors reset to default.");

// Test cursor positioning
terminal_save_cursor();  // Save current position before moving
terminal_set_cursor(5, 10);
terminal_print("Text at row 5, col 10", 3, -1);

// Get cursor position while at the moved location
var crow = terminal_get_cursor_row();
var ccol = terminal_get_cursor_col();

terminal_restore_cursor();  // Restore to normal output position
print("Current cursor position: row " + str(crow) + ", col " + str(ccol));

print("Terminal test complete!");