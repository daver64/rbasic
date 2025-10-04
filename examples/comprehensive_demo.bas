// Comprehensive rbasic Demo - All Features and Functions
// Run with: rbasic -i comprehensive_demo.bas -sdl

print("=== rbasic Comprehensive Demo ===");

// Variables and I/O
var name = "rbasic";
var version = 2.0;
print("Welcome to", name, "version", version);

// String Functions
var greeting = "Hello World!";
print("\nString Functions:");
print("Text:", greeting);
print("Length:", len(greeting));
print("First 5 chars:", left(greeting, 5));
print("Last 6 chars:", right(greeting, 6));
print("Middle part:", mid(greeting, 7, 5));

// Math Functions - Single Argument
print("\nMath Functions:");
var x = 16.5;
print("Number:", x);
print("Square root:", sqrt(x));
print("Absolute value:", abs(-x));
print("Floor:", floor(x));
print("Ceiling:", ceil(x));
print("Rounded:", round(x));

// Trigonometric Functions
var angle = 1.0;
print("sin(1):", sin(angle));
print("cos(1):", cos(angle));
print("tan(1):", tan(angle));

// Logarithmic Functions
print("log(10):", log(10));
print("log10(100):", log10(100));
print("exp(1):", exp(1));

// Math Functions - Two Arguments
print("pow(2, 8):", pow(2, 8));
print("min(10, 20):", min(10, 20));
print("max(10, 20):", max(10, 20));
print("mod(17, 5):", mod(17, 5));

// Math Constants and Random
print("Pi:", pi());
print("Random 1-10:", rnd(10));

// Control Structures
print("\nControl Structures:");

// If-Then-Else
if (version >= 2.0) {
    print("Using modern version");
} else {
    print("Using older version");
}

// For Loop (Modern C-style)
print("Counting 1 to 5:");
for(var i = 1; i <= 5; i = i + 1) {
    print("Count:", i);
}

// While Loop
print("Countdown from 3:");
var countdown = 3;
while(countdown > 0) {
    print(countdown);
    countdown = countdown - 1;
}

// Note: User-defined functions are planned for future implementation

// Graphics Demo (if SDL mode is available)
print("\nInitializing graphics...");
graphics_mode(400, 300);
clear_screen();

// Draw some graphics
print("Drawing graphics...");
set_color(255, 0, 0);                     // Set red color
draw_pixel(50, 50);                       // Red pixel
draw_line(0, 0, 100, 100);               // Line with current color
set_color(0, 0, 255);                     // Set blue color  
draw_rect(200, 200, 80, 60);             // Rectangle outline

refresh_screen();

// Interactive demo
print("\nInteractive demo - press 'A' key or close window to continue...");
var keep_running = 1;
while(keep_running > 0) {
    // Small delay to prevent busy-waiting and allow SDL to process events
    sleep_ms(10);
    
    if (key_pressed(65)) {             // 'A' key
        print("A key detected!");
        keep_running = 0;
    }
    if (quit_requested()) {            // Window close or ESC
        print("Quit requested");
        keep_running = 0;
    }
}

// System info
print("\nSystem Information:");
print("Program runtime:", get_ticks(), "ms");

// Switch back to text mode
text_mode();
print("\nSwitched back to text mode");

print("\n=== Demo Complete ===");
print("All", len("rbasic"), "features demonstrated successfully!");