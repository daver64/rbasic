// stdlib_demo.bas - Demonstration of standard library concept

import "math_lib.bas";
import "sdl2_wrapper.bas";

print("=== rbasic Standard Library Demo ===\n");

// Math library demonstration
print("Math Library Functions:\n");
var math_result1 = add_numbers(10, 20);
print("add_numbers(10, 20) = ");
print(math_result1);
print("\n");

var math_result2 = multiply_numbers(6, 7);
print("multiply_numbers(6, 7) = ");
print(math_result2);
print("\n");

var math_result3 = square(8);
print("square(8) = ");
print(math_result3);
print("\n");

// SDL2 wrapper demonstration (showing the concept)
print("\nSDL2 Wrapper Functions:\n");
print("SDL2 library version: ");
print(SDL_GRAPHICS_LIBRARY_VERSION);
print("\n");

// Example of how you would use the SDL2 wrapper
print("SDL2 wrapper demonstration:\n");
var init_result = init_graphics();
print("Graphics initialized: ");
print(init_result);
print("\n");

var window_handle = create_basic_window("My Game Window");
print("Window created: ");
print(window_handle);
print("\n");

var quit_result = quit_graphics();
print("Graphics shut down: ");
print(quit_result);
print("\n");

// Windows API demonstration (if available)
print("\nWindows API Functions:\n");
var message_result = show_message_box("Hello from rbasic!", "Standard Library Demo");
print("Message box returned: ");
print(message_result);
print("\n");

print("\n=== Demo Complete ===");
print("\nThis demonstrates how rbasic can use import statements");
print("\nto create a modular standard library ecosystem!");