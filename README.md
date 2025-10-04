# rbasic - C-Leaning BASIC Language Transpiler

**rbasic** is a modern BASIC language transpiler that bridges traditional BASIC with contemporary C-style programming. It features familiar C-style syntax while maintaining BASIC's simplicity, making it accessible to developers from C/C++/JavaScript backgrounds.

## What Makes rbasic Different

- **C-Style Syntax**: Uses braces `{}`, parentheses for conditions, and function-call I/O
- **Transpiler Architecture**: Compiles BASIC to optimized C++ executables or runs directly via interpreter
- **Modern Language Features**: C++ style comments, assignment expressions, structured control flow
- **Graphics Programming**: Built-in SDL2 graphics support with drawing primitives
- **Zero Dependencies**: Compiled programs run independently (except SDL for graphics)
- **Dual Execution**: Identical behavior in both interpreted and compiled modes

## Language Overview

rbasic implements a "C-leaning BASIC" that looks familiar to modern developers:

```basic
// Modern C-style BASIC with function-call syntax
function fibonacci(n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

var limit = 10;
print("Fibonacci sequence up to", limit, ":");

for (var i = 0; i <= limit; i = i + 1) {
    print("F(", i, ") =", fibonacci(i));
}
```

## Installation and Usage

### Prerequisites

- CMake 3.16+
- C++17 compiler (GCC, Clang, MSVC)
- SDL2 (optional, for graphics - auto-detected)

### Quick Start

```bash
# Build the transpiler
mkdir build && cd build
cmake .. && cmake --build .

# Run a program directly (interpreted)
./rbasic -i hello.bas

# Compile to standalone executable  
./rbasic -c hello.bas -o hello
./hello
```

### Graphics Programs

```bash
# Run with graphics support
./rbasic -i graphics_demo.bas --io sdl

# Compile graphics program (SDL automatically linked)
./rbasic -c graphics_demo.bas -o graphics_demo
./graphics_demo
```

## Key Language Features

### Modern Syntax Elements

- **C-Style Control Flow**: `if (condition) { }`, `for (init; test; inc) { }`
- **Function-Call I/O**: `print("Hello")`, `var input = input()`
- **Assignment Expressions**: `var y = (x = x + 1) * 2`
- **Modern Comments**: `//` line comments and `/* */` block comments
- **Structured Data**: Arrays with `dim array(size)` and `array[index]` access
- **Structures**: `struct Name { fields } end struct` with member access

### Function Library (30+ Functions)

#### I/O and Control
```basic
print("Hello", "World");        // Multi-argument print
var name = input();             // User input
clear_screen();                 // Clear display
sleep_ms(1000);                // Pause execution
```

#### Mathematics
```basic
print(sqrt(16));               // Square root: 4
print(pow(2, 8));              // Power: 256
print(sin(pi() / 2));          // Trigonometry: 1
print(min(5, 10), max(5, 10)); // Min/max: 5, 10
print(rnd(6));                 // Random 1-6
```

#### String Manipulation
```basic
var text = "Programming";
print(len(text));              // Length: 11
print(left(text, 4));          // "Prog"
print(mid(text, 5, 4));        // "gram"
print(right(text, 4));         // "ming"
```

#### Graphics (SDL2-based)
```basic
graphics_mode(800, 600);       // Create window
set_colour(255, 0, 0);          // Red colour
draw_pixel(100, 100);          // Single pixel
draw_line(0, 0, 200, 200);     // Diagonal line
draw_rect(50, 50, 100, 75, true); // Filled rectangle
draw_text(10, 10, "Hello!");   // Text rendering
refresh_screen();              // Update display
```

## Example Programs

### Hello World
```basic
// hello.bas - Classic greeting
var name = "World";
print("Hello,", name + "!");
```

### Calculator
```basic
// calculator.bas - Simple arithmetic
print("Enter two numbers:");
var a = input();
var b = input();

print("Results:");
print(a, "+", b, "=", a + b);
print(a, "-", b, "=", a - b);
print(a, "*", b, "=", a * b);
if (b != 0) {
    print(a, "/", b, "=", a / b);
}
```

### Functions and Structures
```basic
// User-defined functions and structures
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

struct Point
    x as integer;
    y as integer;
end struct;

dim p as Point;
var p.x = 10;
var p.y = 20;

print("Point: (", p.x, ",", p.y, ")");
print("5! =", factorial(5));
```

## Transpiler Architecture

rbasic uses a sophisticated transpiler design that ensures identical behavior between interpreted and compiled modes:

```
BASIC Source → Lexer → Parser → AST
                                 ↓
                        Interpreter ← → Code Generator
                             ↓                ↓
                        Direct Execution   C++ Source → Executable
```

### Components
- **Lexer**: Modern tokenization with C-style syntax support
- **Parser**: Builds comprehensive AST with all language constructs  
- **Interpreter**: Direct AST execution for rapid development
- **Code Generator**: Emits optimized C++ for production deployment
- **Runtime Library**: Shared function implementations for both modes

## Graphics System

rbasic includes a complete graphics programming environment:

### Automatic SDL Integration
- **Interpreted Mode**: Uses SDL2 IOHandler for immediate graphics
- **Compiled Mode**: Links SDL2 libraries automatically when graphics functions detected
- **Fallback**: Non-graphics programs have zero SDL dependencies

### Drawing Primitives
- Pixel-level control with `draw_pixel(x, y)`
- Vector graphics with `draw_line()` and `draw_rect()`
- Text rendering with `draw_text()`
- Colour management with `set_colour(r, g, b)`
- Double-buffering with `refresh_screen()`

### Interactive Features
- Real-time input with `key_pressed(key)`
- Window management with `quit_requested()`
- Animation timing with `get_ticks()` and `sleep_ms()`

## Project Structure

```
rbasic/
├── CMakeLists.txt         # Build configuration
├── README.md              # This documentation
├── BNF.md                 # Complete language grammar
├── include/               # Header files
├── src/                   # Implementation
├── runtime/               # Standalone runtime library
├── examples/              # Demonstration programs
└── tests/                 # Automated testing
```

## Build Configuration

### Standard Build (Console Only)
```bash
cmake -B build
cmake --build build
```

### With Graphics Support
```bash
# Auto-detects SDL2 installation
cmake -B build -DENABLE_SDL_SUPPORT=ON
cmake --build build
```

### Cross-Platform Notes
- **Windows**: Uses MSVC compiler, auto-detects SDL2
- **Linux**: Requires `libsdl2-dev` package for graphics
- **macOS**: Install SDL2 via Homebrew for graphics support

## Performance and Deployment

### Compilation Benefits
- **Native Speed**: Compiled programs run at C++ performance levels
- **No Dependencies**: Standalone executables (except SDL for graphics)
- **Optimized Output**: Generated C++ is human-readable and compiler-optimized
- **Cross-Platform**: Compiles to native code on Windows, Linux, macOS

### Development Workflow
1. **Rapid Prototyping**: Use interpreted mode (`-i`) for fast iteration
2. **Testing**: Identical behavior guarantees between modes  
3. **Production**: Compile (`-c`) for deployment and distribution
4. **Graphics**: Automatic SDL linking for visual applications

## Learning Resources

rbasic is designed for developers familiar with C-style languages who want:
- **Rapid Prototyping**: BASIC's simplicity with modern syntax
- **Graphics Programming**: Built-in visual capabilities
- **Educational Use**: Clear, readable transpiled C++ output
- **Cross-Platform Development**: Single source for multiple targets

The C-leaning syntax makes rbasic approachable for developers transitioning from:
- C/C++ (familiar control structures)
- JavaScript (function-call syntax) 
- Python (rapid development cycle)
- Java (structured programming)

## License

MIT License - See LICENSE file for details.

---

**rbasic** - Where BASIC meets modern development. Write like C, think like BASIC, compile to anything.