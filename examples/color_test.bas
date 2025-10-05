// Simple color test
var supported = terminal_supports_color();
print("Color support: " + str(supported));

if (supported) {
    terminal_print("This should be RED", 1, -1);
    terminal_println("");
    terminal_print("This should be GREEN", 2, -1);
    terminal_println("");
} else {
    print("Colors not supported - text will be plain");
}