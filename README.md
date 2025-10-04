# rbasic - A Modern BASIC Interpreter and Compiler

rbasic is a hybrid BASIC interpreter and compiler written in C++. It features modern C-style syntax that bridges traditional BASIC with contemporary programming languages, making it familiar to developers from C/C++/JavaScript backgrounds while maintaining BASIC's simplicity.

## Features

- **Modern C-Style Syntax**: C-style control structures with braces {}, parentheses for conditions, function-call I/O
- **Dual Execution Modes**: Interpret BASIC code directly or compile to C++ executable
- **Graphics Support**: SDL2-based graphics with pixel, line, circle, rectangle, and text drawing
- **Modern Comments**: C++ style `//` and `/* */` comments
- **Comprehensive Function Library**: 30+ built-in functions for math, string manipulation, graphics, and I/O
- **I/O Abstraction**: Pluggable I/O system supporting console and graphics contexts
- **Compatible Behavior**: Interpreter and compiler produce identical results
- **Modern C++**: Built with C++17 for maintainability and performance

## Building

### Prerequisites
- CMake 3.16 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- SDL2 and SDL_ttf (optional, for graphics support)
  - Windows: Automatically downloaded via vcpkg if available
  - Linux: `sudo apt-get install libsdl2-dev libsdl2-ttf-dev`
  - macOS: `brew install sdl2 sdl2_ttf`

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests
ctest
```

The `rbasic` executable will be placed in the project root directory.

## Usage

### Interpreter Mode
```bash
./rbasic -i program.bas                    # Console mode
./rbasic -i program.bas --io sdl          # Graphics mode (SDL2)
```

### Compiler Mode
```bash
./rbasic -c program.bas -o program
```

### Help
```bash
./rbasic --help
```

## Modern rbasic Language Features

### Variables and I/O (Modern Function-Call Style)
```basic
// Modern syntax with C++ style comments
var name = "World";
var number = 42;

print("Hello,", name);
print("The answer is", number);

print("Enter a number:");
var userInput = input();
print("You entered:", userInput);
```

### Modern Control Structures (C-Style Syntax)
```basic
// Modern for loops with C-style syntax
for(var i = 1; i <= 10; i = i + 1) {
    print("Count: " + str(i));
}

// While loops with braces
var counter = 5;
while(counter > 0) {
    print("Countdown: " + str(counter));
    counter = counter - 1;
}

// If statements with parentheses and braces
var x = 15;
if (x > 10) {
    print("x is greater than 10");
} else {
    print("x is not greater than 10");
}
```

### Assignment Expressions
```basic
// Assignment works as both statement and expression
var x = 5;
var y = (x = x + 1) * 2;  // x becomes 6, y becomes 12
```

### Math Functions
```basic
var x = 16;
print("Square root:", sqrt(x));
print("Sine of 1:", sin(1));
print("Pi:", pi());
print("Power:", pow(2, 3));
print("Min/Max:", min(5, 10), max(5, 10));
```

### String Functions
```basic
var text = "Hello World";
print("Length:", len(text));
print("Substring:", mid(text, 7, 5));  // "World"
print("Left 5 chars:", left(text, 5)); // "Hello"
print("Right 5 chars:", right(text, 5)); // "World"
```

### Graphics Functions (SDL Mode)
```basic
// Graphics operations
graphics_mode(640, 480);  // Initialize graphics window
clear_screen();           // Clear screen
set_color(255, 0, 0);     // Set color to red
draw_pixel(100, 100);     // Draw pixel at (100,100)
draw_line(0, 0, 100, 100); // Draw line
draw_circle(200, 200, 50); // Draw circle outline
draw_rect(300, 300, 100, 50, false); // Draw rectangle outline
draw_text(10, 10, "Hello SDL!"); // Draw text
refresh_screen();         // Update display
```

### Control and System Functions
```basic
// System information
print("Milliseconds: " + str(get_ticks()));
print("Random 1-6: " + str(rnd(6)));

// Input checking
if (key_pressed("A")) {  // Check if 'A' key is pressed
    print("A key is down");
}

if (quit_requested()) {  // Check if user wants to quit
    print("Quit requested");
}

// Mode switching
text_mode();             // Switch to text mode
```

### Functions (Modern C-Style)
```basic
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

var result = factorial(5);
print("5! = " + str(result));
```

## Complete Function Reference

All functions in rbasic use modern function-call syntax with parentheses, making them familiar to developers from other languages.

### I/O Functions
- `print(value1, value2, ...)` - Print values to output (console or graphics)
- `input()` - Read input from user
- `clear_screen()` - Clear screen

### String Functions  
- `len(string)` - Get length of string
- `mid(string, start, length)` - Extract substring (1-based indexing)
- `left(string, count)` - Get leftmost characters
- `right(string, count)` - Get rightmost characters
- `str(value)` - Convert number to string

### Math Functions - Single Argument
- `sqrt(x)` or `sqr(x)` - Square root
- `abs(x)` - Absolute value
- `sin(x)` - Sine (radians)
- `cos(x)` - Cosine (radians)  
- `tan(x)` - Tangent (radians)
- `log(x)` - Natural logarithm
- `log10(x)` - Base-10 logarithm
- `exp(x)` - e^x
- `floor(x)` - Floor function
- `ceil(x)` - Ceiling function
- `round(x)` - Round to nearest integer

### Math Functions - Two Arguments
- `pow(base, exponent)` - Power function
- `min(a, b)` - Minimum value
- `max(a, b)` - Maximum value
- `mod(a, b)` - Modulo operation

### Math Functions - Zero Arguments
- `pi()` - Pi constant (3.14159...)
- `rnd(max)` - Random integer from 1 to max

### Graphics Functions (SDL Mode)
- `graphics_mode(width, height)` - Initialize graphics window
- `set_color(r, g, b)` - Set drawing color (RGB 0-255)
- `draw_pixel(x, y)` - Draw pixel with current color
- `draw_line(x1, y1, x2, y2)` - Draw line with current color
- `draw_circle(x, y, radius)` - Draw circle outline
- `draw_rect(x, y, width, height, filled)` - Draw rectangle (filled: true/false)
- `draw_text(x, y, string)` - Draw text with current color
- `refresh_screen()` - Update display
- `text_mode()` - Switch to text mode

### Control and System Functions
- `get_ticks()` - Get milliseconds since program start
- `key_pressed(key)` - Check if key is currently pressed (string or keycode)
- `quit_requested()` - Check if user requested quit (window close/ESC)
- `sleep_ms(milliseconds)` - Sleep for specified milliseconds

### Language Keywords and Syntax
- `var` - Variable declaration
- `if (condition) { }`, `else { }` - Conditional statements with C-style braces
- `for(init; condition; increment) { }` - C-style for loops with braces
- `while(condition) { }` - While loops with C-style braces
- `function name() { }` - Function definition with braces
- `struct name { }` - Structure definition (planned)
- `//` and `/* */` - C-style comments
- Operators: `+`, `-`, `*`, `/`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `=` (assignment)

## Project Structure

```
rbasic/
├── CMakeLists.txt
├── README.md
├── include/           # Header files
├── src/               # Source files
├── runtime/           # Runtime library for compiled programs
├── examples/          # Example BASIC programs
├── tests/             # Unit and integration tests
└── build/             # Build directory (created by CMake)
```

## Architecture

RBasic uses a shared AST (Abstract Syntax Tree) representation that both the interpreter and compiler operate on:

1. **Lexer**: Tokenizes BASIC source code
2. **Parser**: Builds AST from tokens
3. **Interpreter**: Directly executes AST nodes
4. **Code Generator**: Emits C++ code from AST
5. **Runtime Library**: Shared functions for both execution modes

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

MIT License - see LICENSE file for details.