# rbasic - C-Leaning BASIC Language Transpiler# rbasic - C-Leaning BASIC Language Transpiler and Interpreter.



**rbasic** is a modern BASIC language transpiler that bridges traditional BASIC with contemporary C-style programming. It features familiar C-style syntax while maintaining BASIC's simplicity, making it accessible to developers from C/C++/JavaScript backgrounds who want the rapid development benefits of BASIC.



# 🚀 What Makes rbasic Different
**rbasic** is a modern BASIC language transpiler that bridges traditional BASIC with contemporary C-style programming. It features familiar C-style syntax while maintaining BASIC's simplicity, making it accessible to developers from C/C++/JavaScript backgrounds who want the rapid development benefits of BASIC.rbasic is a hybrid BASIC interpreter and compiler written in C++. It features modern C-style syntax that bridges traditional BASIC with contemporary programming languages, making it familiar to developers from C/C++/JavaScript backgrounds while maintaining BASIC's simplicity.



- **C-Style Syntax**: Uses braces `{}`, parentheses for conditions, and function-call I/O

- **Transpiler Architecture**: Compiles BASIC to optimized C++ executables or runs directly via interpreter

- **Modern Language Features**: C++ style comments, assignment expressions, structured control flow## 🚀 What Makes rbasic Different## Features

- **Graphics Programming**: Built-in SDL2 graphics support with drawing primitives

- **Zero Dependencies**: Compiled programs run independently without runtime libraries (except SDL for graphics)

- **Dual Execution**: Identical behavior in both interpreted and compiled modes

- **C-Style Syntax**: Uses braces `{}`, parentheses for conditions, and function-call I/O- **Modern C-Style Syntax**: C-style control structures with braces {}, parentheses for conditions, function-call I/O

## 📖 Language Overview

- **Transpiler Architecture**: Compiles BASIC to optimized C++ executables or runs directly via interpreter- **Dual Execution Modes**: Interpret BASIC code directly or compile to C++ executable

rbasic implements a "C-leaning BASIC" that looks familiar to modern developers:

- **Modern Language Features**: C++ style comments, assignment expressions, structured control flow- **Graphics Support**: SDL2-based graphics with pixel, line, circle, rectangle, and text drawing

```basic

// Modern C-style BASIC with function-call syntax- **Graphics Programming**: Built-in SDL2 graphics support with drawing primitives- **Modern Comments**: C++ style `//` and `/* */` comments

function fibonacci(n) {

    if (n <= 1) {- **Zero Dependencies**: Compiled programs run independently without runtime libraries (except SDL for graphics)- **Comprehensive Function Library**: 30+ built-in functions for math, string manipulation, graphics, and I/O

        return n;

    } else {- **Dual Execution**: Identical behavior in both interpreted and compiled modes- **I/O Abstraction**: Pluggable I/O system supporting console and graphics contexts

        return fibonacci(n - 1) + fibonacci(n - 2);

    }- **Compatible Behavior**: Interpreter and compiler produce identical results

}

## 📖 Language Overview- **Modern C++**: Built with C++17 for maintainability and performance

var limit = 10;

print("Fibonacci sequence up to", limit, ":");



for (var i = 0; i <= limit; i = i + 1) {rbasic implements a "C-leaning BASIC" that looks familiar to modern developers:## Building

    print("F(", i, ") =", fibonacci(i));

}

```

```basic### Prerequisites

## 🛠️ Installation & Usage

// Modern C-style BASIC with function-call syntax- CMake 3.16 or higher

### Prerequisites

function fibonacci(n) {- C++17 compatible compiler (GCC, Clang, MSVC)

- CMake 3.16+

- C++17 compiler (GCC, Clang, MSVC)    if (n <= 1) {- SDL2 and SDL_ttf (optional, for graphics support)

- SDL2 (optional, for graphics - auto-detected)

        return n;  - Windows: Automatically downloaded via vcpkg if available

### Quick Start

    } else {  - Linux: `sudo apt-get install libsdl2-dev libsdl2-ttf-dev`

```bash

# Build the transpiler        return fibonacci(n - 1) + fibonacci(n - 2);  - macOS: `brew install sdl2 sdl2_ttf`

mkdir build && cd build

cmake .. && cmake --build .    }



# Run a program directly (interpreted)}### Build Instructions

./rbasic -i hello.bas



# Compile to standalone executable  

./rbasic -c hello.bas -o hellovar limit = 10;```bash

./hello

```print("Fibonacci sequence up to", limit, ":");# Create build directory



### Graphics Programsmkdir build



```bashfor (var i = 0; i <= limit; i = i + 1) {cd build

# Run with graphics support

./rbasic -i graphics_demo.bas --io sdl    print("F(" + i + ") =", fibonacci(i));



# Compile graphics program (SDL automatically linked)}# Configure

./rbasic -c graphics_demo.bas -o graphics_demo

./graphics_demo  # Runs with SDL graphics```cmake ..

```



## 🎯 Key Language Features

## 🛠️ Installation & Usage# Build

### Modern Syntax Elements

cmake --build .

- **C-Style Control Flow**: `if (condition) { }`, `for (init; test; inc) { }`

- **Function-Call I/O**: `print("Hello")`, `var input = input()`### Prerequisites

- **Assignment Expressions**: `var y = (x = x + 1) * 2`

- **Modern Comments**: `//` line comments and `/* */` block comments- CMake 3.16+# Run tests

- **Structured Data**: Arrays with `dim array(size)` and `array[index]` access

- **Structures**: `struct Name { fields } end struct` with member access- C++17 compiler (GCC, Clang, MSVC)ctest



### Complete Function Library (30+ Functions)- SDL2 (optional, for graphics - auto-detected)```



#### I/O & Control

```basic

print("Hello", "World");        // Multi-argument print### Quick StartThe `rbasic` executable will be placed in the project root directory.

var name = input();             // User input

clear_screen();                 // Clear display```bash

sleep_ms(1000);                // Pause execution

```# Build the transpiler## Usage



#### Mathematicsmkdir build && cd build

```basic

print(sqrt(16));               // Square root: 4cmake .. && cmake --build .### Interpreter Mode

print(pow(2, 8));              // Power: 256

print(sin(pi() / 2));          // Trigonometry: 1```bash

print(min(5, 10), max(5, 10)); // Min/max: 5, 10

print(rnd(6));                 // Random 1-6# Run a program directly (interpreted)./rbasic -i program.bas                    # Console mode

```

./rbasic -i hello.bas./rbasic -i program.bas --io sdl          # Graphics mode (SDL2)

#### String Manipulation

```basic```

var text = "Programming";

print(len(text));              // Length: 11# Compile to standalone executable  

print(left(text, 4));          // "Prog"

print(mid(text, 5, 4));        // "gram"./rbasic -c hello.bas -o hello### Compiler Mode

print(right(text, 4));         // "ming"

```./hello```bash



#### Graphics (SDL2-based)```./rbasic -c program.bas -o program

```basic

graphics_mode(800, 600);       // Create window```

set_color(255, 0, 0);          // Red color

draw_pixel(100, 100);          // Single pixel### Graphics Programs

draw_line(0, 0, 200, 200);     // Diagonal line

draw_rect(50, 50, 100, 75, true); // Filled rectangle```bash### Help

draw_text(10, 10, "Hello!");   // Text rendering

refresh_screen();              // Update display# Run with graphics support```bash

```

./rbasic -i graphics_demo.bas --io sdl./rbasic --help

## 📁 Example Programs

```

### Hello World

```basic# Compile graphics program (SDL automatically linked)

// hello.bas - Classic greeting

var name = "World";./rbasic -c graphics_demo.bas -o graphics_demo## Language Features Status

print("Hello,", name + "!");

```./graphics_demo  # Runs with SDL graphics



### Calculator```### ✅ Fully Implemented

```basic

// calculator.bas - Simple arithmetic- **Modern C-Style Syntax**: Control structures with braces `{}`, parentheses for conditions

print("Enter two numbers:");

var a = input();## 🎯 Key Language Features- **Variables and I/O**: `var` declarations, `print()`, `input()` 

var b = input();

- **Control Structures**: `if/else`, `for()`, `while()` with C-style syntax

print("Results:");

print(a, "+", b, "=", a + b);### Modern Syntax Elements- **Arrays**: `dim array(size) as type`, `var array[index] = value`, `array[index]` access

print(a, "-", b, "=", a - b);

print(a, "*", b, "=", a * b);- **C-Style Control Flow**: `if (condition) { }`, `for (init; test; inc) { }`- **User-Defined Functions**: `function name() { }` with parameters and return values

if (b != 0) {

    print(a, "/", b, "=", a / b);- **Function-Call I/O**: `print("Hello")`, `var input = input()`- **Built-in Functions**: 30+ math, string, graphics, and system functions

}

```- **Assignment Expressions**: `var y = (x = x + 1) * 2`- **Graphics Support**: SDL2-based graphics with pixel, line, rectangle drawing



### Graphics Demo- **Modern Comments**: `//` line comments and `/* */` block comments- **Comments**: C++ style `//` and `/* */` comments

```basic

// graphics_demo.bas - Visual programming- **Structured Data**: Arrays with `dim array(size)` and `array[index]` access- **Operators**: All arithmetic and comparison operators including `!=`

graphics_mode(640, 480);



var running = 1;

while (running) {### Complete Function Library (30+ Functions)### 🚧 Partially Implemented  

    clear_screen();

    - **else-if Chains**: Currently requires nested if-else statements

    // Draw animated circle

    var time = get_ticks() / 100;#### I/O & Control- **Structures**: Basic parsing exists but needs implementation

    var x = 320 + 100 * cos(time);

    var y = 240 + 100 * sin(time);```basic

    

    set_color(255, 128, 0);print("Hello", "World");        // Multi-argument print### 📋 Planned Features

    draw_circle(x, y, 20);

    var name = input();             // User input- **Advanced Control Flow**: `break`, `continue` statements, direct `else if` syntax

    refresh_screen();

    clear_screen();                 // Clear display- **String Conversion**: `str()` function for explicit string conversion

    if (key_pressed("ESC") or quit_requested()) {

        running = 0;sleep_ms(1000);                // Pause execution- **String Interpolation**: Modern string formatting

    }

}```- **File I/O**: Reading and writing files



text_mode();

```

#### Mathematics### Variables and I/O (Modern Function-Call Style)

### Functions and Structures

```basic```basic```basic

// functions.bas - User-defined functions and structures

function factorial(n) {print(sqrt(16));               // Square root: 4// Modern syntax with C++ style comments

    if (n <= 1) {

        return 1;print(pow(2, 8));              // Power: 256var name = "World";

    } else {

        return n * factorial(n - 1);print(sin(pi() / 2));          // Trigonometry: 1var number = 42;

    }

}print(min(5, 10), max(5, 10)); // Min/max: 5, 10



struct Pointprint(rnd(6));                 // Random 1-6print("Hello,", name);

    x as integer;

    y as integer;```print("The answer is", number);

end struct;



dim p as Point;

var p.x = 10;#### String Manipulationprint("Enter a number:");

var p.y = 20;

```basicvar userInput = input();

print("Point: (", p.x, ",", p.y, ")");

print("5! =", factorial(5));var text = "Programming";print("You entered:", userInput);

```

print(len(text));              // Length: 11```

## 🏗️ Transpiler Architecture

print(left(text, 4));          // "Prog"

rbasic uses a sophisticated transpiler design that ensures identical behavior between interpreted and compiled modes:

print(mid(text, 5, 4));        // "gram"### Modern Control Structures (C-Style Syntax)

```

BASIC Source → Lexer → Parser → ASTprint(right(text, 4));         // "ming"```basic

                                 ↓

                        Interpreter ← → Code Generator```// Modern for loops with C-style syntax

                             ↓                ↓

                        Direct Execution   C++ Source → Executablefor(var i = 1; i <= 10; i = i + 1) {

```

#### Graphics (SDL2-based)    print("Count: ", i);

### Components

- **Lexer**: Modern tokenization with C-style syntax support```basic}

- **Parser**: Builds comprehensive AST with all language constructs  

- **Interpreter**: Direct AST execution for rapid developmentgraphics_mode(800, 600);       // Create window

- **Code Generator**: Emits optimized C++ for production deployment

- **Runtime Library**: Shared function implementations for both modesset_color(255, 0, 0);          // Red color// While loops with braces



## 🎮 Graphics Systemdraw_pixel(100, 100);          // Single pixelvar counter = 5;



rbasic includes a complete graphics programming environment:draw_line(0, 0, 200, 200);     // Diagonal linewhile(counter > 0) {



### Automatic SDL Integrationdraw_rect(50, 50, 100, 75, true); // Filled rectangle    print("Countdown: ", counter);

- **Interpreted Mode**: Uses SDL2 IOHandler for immediate graphics

- **Compiled Mode**: Links SDL2 libraries automatically when graphics functions detecteddraw_text(10, 10, "Hello!");   // Text rendering    counter = counter - 1;

- **Fallback**: Non-graphics programs have zero SDL dependencies

refresh_screen();              // Update display}

### Drawing Primitives

- Pixel-level control with `draw_pixel(x, y)````

- Vector graphics with `draw_line()` and `draw_rect()`

- Text rendering with `draw_text()`// If statements with parentheses and braces (nested for multiple conditions)

- Color management with `set_color(r, g, b)`

- Double-buffering with `refresh_screen()`## 📁 Example Programsvar x = 15;



### Interactive Featuresif (x > 10) {

- Real-time input with `key_pressed(key)`

- Window management with `quit_requested()`### Hello World    print("x is greater than 10");

- Animation timing with `get_ticks()` and `sleep_ms()`

```basic} else {

## 📋 Project Structure

// hello.bas - Classic greeting    if (x > 5) {

```

rbasic/var name = "World";        print("x is greater than 5 but not greater than 10");

├── CMakeLists.txt         # Build configuration

├── README.md              # This documentationprint("Hello,", name + "!");    } else {

├── BNF.md                 # Complete language grammar

├── include/               # Header files```        print("x is 5 or less");

│   ├── lexer.h           # Tokenization

│   ├── parser.h          # Syntax analysis      }

│   ├── ast.h             # Abstract syntax tree

│   ├── interpreter.h     # Direct execution### Calculator}

│   ├── codegen.h         # C++ transpilation

│   └── runtime.h         # Function library```basic```

├── src/                   # Implementation

├── runtime/               # Standalone runtime library// calculator.bas - Simple arithmetic

├── examples/              # Demonstration programs

│   ├── hello.bas         # Basic I/Oprint("Enter two numbers:");### Assignment Expressions

│   ├── calculator.bas    # Arithmetic operations

│   ├── fibonacci.bas     # Recursion and mathvar a = input();```basic

│   ├── graphics_demo.bas # Visual programming

│   ├── arrays.bas        # Data structuresvar b = input();// Assignment works as both statement and expression

│   ├── functions.bas     # User-defined functions

│   └── structures.bas    # Advanced data typesvar x = 5;

└── tests/                 # Automated testing

```print("Results:");var y = (x = x + 1) * 2;  // x becomes 6, y becomes 12



## 🔧 Build Configurationprint(a, "+", b, "=", a + b);```



### Standard Build (Console Only)print(a, "-", b, "=", a - b);

```bash

cmake -B buildprint(a, "*", b, "=", a * b);### Math Functions

cmake --build build

```if (b != 0) {```basic



### With Graphics Support    print(a, "/", b, "=", a / b);var x = 16;

```bash

# Auto-detects SDL2 installation}print("Square root:", sqrt(x));

cmake -B build -DENABLE_SDL_SUPPORT=ON

cmake --build build```print("Sine of 1:", sin(1));

```

print("Pi:", pi());

### Cross-Platform Notes

- **Windows**: Uses MSVC compiler, auto-detects SDL2### Graphics Demoprint("Power:", pow(2, 3));

- **Linux**: Requires `libsdl2-dev` package for graphics

- **macOS**: Install SDL2 via Homebrew for graphics support```basicprint("Min/Max:", min(5, 10), max(5, 10));



## 🚀 Performance & Deployment// graphics_demo.bas - Visual programming```



### Compilation Benefitsgraphics_mode(640, 480);

- **Native Speed**: Compiled programs run at C++ performance levels

- **No Dependencies**: Standalone executables (except SDL for graphics)### String Functions

- **Optimized Output**: Generated C++ is human-readable and compiler-optimized

- **Cross-Platform**: Compiles to native code on Windows, Linux, macOSvar running = 1;```basic



### Development Workflowwhile (running) {var text = "Hello World";

1. **Rapid Prototyping**: Use interpreted mode (`-i`) for fast iteration

2. **Testing**: Identical behavior guarantees between modes      clear_screen();print("Length:", len(text));

3. **Production**: Compile (`-c`) for deployment and distribution

4. **Graphics**: Automatic SDL linking for visual applications    print("Substring:", mid(text, 7, 5));  // "World"



## 🎓 Learning Resources    // Draw animated circleprint("Left 5 chars:", left(text, 5)); // "Hello"



rbasic is designed for developers familiar with C-style languages who want:    var time = get_ticks() / 100;print("Right 5 chars:", right(text, 5)); // "World"

- **Rapid Prototyping**: BASIC's simplicity with modern syntax

- **Graphics Programming**: Built-in visual capabilities    var x = 320 + 100 * cos(time);```

- **Educational Use**: Clear, readable transpiled C++ output

- **Cross-Platform Development**: Single source for multiple targets    var y = 240 + 100 * sin(time);



The C-leaning syntax makes rbasic approachable for developers transitioning from:    ### Graphics Functions (SDL Mode)

- C/C++ (familiar control structures)

- JavaScript (function-call syntax)     set_color(255, 128, 0);```basic

- Python (rapid development cycle)

- Java (structured programming)    draw_circle(x, y, 20);// Graphics operations



## 📄 License    graphics_mode(640, 480);  // Initialize graphics window



MIT License - See LICENSE file for details.    refresh_screen();clear_screen();           // Clear screen



---    set_color(255, 0, 0);     // Set color to red



**rbasic** - Where BASIC meets modern development. Write like C, think like BASIC, compile to anything.    if (key_pressed("ESC") or quit_requested()) {draw_pixel(100, 100);     // Draw pixel at (100,100)

        running = 0;draw_line(0, 0, 100, 100); // Draw line

    }draw_circle(200, 200, 50); // Draw circle outline

}draw_rect(300, 300, 100, 50, false); // Draw rectangle outline

draw_text(10, 10, "Hello SDL!"); // Draw text

text_mode();refresh_screen();         // Update display

``````



## 🏗️ Transpiler Architecture### Control and System Functions

```basic

rbasic uses a sophisticated transpiler design that ensures identical behavior between interpreted and compiled modes:// System information

print("Milliseconds: " + str(get_ticks()));

```print("Random 1-6: " + str(rnd(6)));

BASIC Source → Lexer → Parser → AST

                                 ↓// Input checking

                        Interpreter ← → Code Generatorif (key_pressed("A")) {  // Check if 'A' key is pressed

                             ↓                ↓    print("A key is down");

                        Direct Execution   C++ Source → Executable}

```

if (quit_requested()) {  // Check if user wants to quit

### Components    print("Quit requested");

- **Lexer**: Modern tokenization with C-style syntax support}

- **Parser**: Builds comprehensive AST with all language constructs  

- **Interpreter**: Direct AST execution for rapid development// Mode switching

- **Code Generator**: Emits optimized C++ for production deploymenttext_mode();             // Switch to text mode

- **Runtime Library**: Shared function implementations for both modes```



## 🎮 Graphics System### Functions (Modern C-Style)

```basic

rbasic includes a complete graphics programming environment:function factorial(n) {

    if (n <= 1) {

### Automatic SDL Integration        return 1;

- **Interpreted Mode**: Uses SDL2 IOHandler for immediate graphics    } else {

- **Compiled Mode**: Links SDL2 libraries automatically when graphics functions detected        return n * factorial(n - 1);

- **Fallback**: Non-graphics programs have zero SDL dependencies    }

}

### Drawing Primitives

- Pixel-level control with `draw_pixel(x, y)`var result = factorial(5);

- Vector graphics with `draw_line()` and `draw_rect()`  print("5! = ", result);

- Text rendering with `draw_text()````

- Color management with `set_color(r, g, b)`

- Double-buffering with `refresh_screen()`### Arrays

```basic

### Interactive Features// Array declaration

- Real-time input with `key_pressed(key)`dim numbers(10) as integer;

- Window management with `quit_requested()`dim names(5) as string;

- Animation timing with `get_ticks()` and `sleep_ms()`

// Array assignment and access

## 📋 Project Structurevar numbers[1] = 42;

var numbers[2] = numbers[1] * 2;

```

rbasic/// Array iteration

├── CMakeLists.txt         # Build configurationfor(var i = 1; i <= 5; i = i + 1) {

├── README.md              # This documentation    var numbers[i] = i * i;

├── include/               # Header files    print("Square of ", i, " is ", numbers[i]);

│   ├── lexer.h           # Tokenization}

│   ├── parser.h          # Syntax analysis  ```

│   ├── ast.h             # Abstract syntax tree

│   ├── interpreter.h     # Direct execution## Complete Function Reference

│   ├── codegen.h         # C++ transpilation

│   └── runtime.h         # Function libraryAll functions in rbasic use modern function-call syntax with parentheses, making them familiar to developers from other languages.

├── src/                   # Implementation

├── runtime/               # Standalone runtime library### I/O Functions

├── examples/              # Demonstration programs- `print(value1, value2, ...)` - Print values to output (console or graphics)

│   ├── hello.bas         # Basic I/O- `input()` - Read input from user

│   ├── calculator.bas    # Arithmetic operations- `clear_screen()` - Clear screen

│   ├── fibonacci.bas     # Recursion and math

│   ├── graphics_demo.bas # Visual programming### String Functions  

│   ├── arrays.bas        # Data structures- `len(string)` - Get length of string

│   ├── functions.bas     # User-defined functions- `mid(string, start, length)` - Extract substring (1-based indexing)

│   └── structures.bas    # Advanced data types- `left(string, count)` - Get leftmost characters

└── tests/                 # Automated testing- `right(string, count)` - Get rightmost characters

```- `str(value)` - Convert number to string



## 🔧 Build Configuration### Math Functions - Single Argument

- `sqrt(x)` or `sqr(x)` - Square root

### Standard Build (Console Only)- `abs(x)` - Absolute value

```bash- `sin(x)` - Sine (radians)

cmake -B build- `cos(x)` - Cosine (radians)  

cmake --build build- `tan(x)` - Tangent (radians)

```- `log(x)` - Natural logarithm

- `log10(x)` - Base-10 logarithm

### With Graphics Support- `exp(x)` - e^x

```bash- `floor(x)` - Floor function

# Auto-detects SDL2 installation- `ceil(x)` - Ceiling function

cmake -B build -DENABLE_SDL_SUPPORT=ON- `round(x)` - Round to nearest integer

cmake --build build

```### Math Functions - Two Arguments

- `pow(base, exponent)` - Power function

### Cross-Platform Notes- `min(a, b)` - Minimum value

- **Windows**: Uses MSVC compiler, auto-detects SDL2- `max(a, b)` - Maximum value

- **Linux**: Requires `libsdl2-dev` package for graphics- `mod(a, b)` - Modulo operation

- **macOS**: Install SDL2 via Homebrew for graphics support

### Math Functions - Zero Arguments

## 🚀 Performance & Deployment- `pi()` - Pi constant (3.14159...)

- `rnd(max)` - Random integer from 1 to max

### Compilation Benefits

- **Native Speed**: Compiled programs run at C++ performance levels### Graphics Functions (SDL Mode)

- **No Dependencies**: Standalone executables (except SDL for graphics)- `graphics_mode(width, height)` - Initialize graphics window

- **Optimized Output**: Generated C++ is human-readable and compiler-optimized- `set_color(r, g, b)` - Set drawing color (RGB 0-255)

- **Cross-Platform**: Compiles to native code on Windows, Linux, macOS- `draw_pixel(x, y)` - Draw pixel with current color

- `draw_line(x1, y1, x2, y2)` - Draw line with current color

### Development Workflow- `draw_circle(x, y, radius)` - Draw circle outline

1. **Rapid Prototyping**: Use interpreted mode (`-i`) for fast iteration- `draw_rect(x, y, width, height, filled)` - Draw rectangle (filled: true/false)

2. **Testing**: Identical behavior guarantees between modes  - `draw_text(x, y, string)` - Draw text with current color

3. **Production**: Compile (`-c`) for deployment and distribution- `refresh_screen()` - Update display

4. **Graphics**: Automatic SDL linking for visual applications- `text_mode()` - Switch to text mode



## 🎓 Learning Resources### Control and System Functions

- `get_ticks()` - Get milliseconds since program start

rbasic is designed for developers familiar with C-style languages who want:- `key_pressed(key)` - Check if key is currently pressed (string or keycode)

- **Rapid Prototyping**: BASIC's simplicity with modern syntax- `quit_requested()` - Check if user requested quit (window close/ESC)

- **Graphics Programming**: Built-in visual capabilities- `sleep_ms(milliseconds)` - Sleep for specified milliseconds

- **Educational Use**: Clear, readable transpiled C++ output

- **Cross-Platform Development**: Single source for multiple targets### Language Keywords and Syntax

- `var` - Variable declaration

The C-leaning syntax makes rbasic approachable for developers transitioning from:- `if (condition) { }`, `else { }` - Conditional statements with C-style braces

- C/C++ (familiar control structures)- `for(init; condition; increment) { }` - C-style for loops with braces

- JavaScript (function-call syntax) - `while(condition) { }` - While loops with C-style braces

- Python (rapid development cycle)- `function name() { }` - Function definition with braces

- Java (structured programming)- `struct name { }` - Structure definition (planned)

- `//` and `/* */` - C-style comments

## 📄 License- Operators: `+`, `-`, `*`, `/`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `=` (assignment)



MIT License - See LICENSE file for details.## Project Structure



---```

rbasic/

**rbasic** - Where BASIC meets modern development. Write like C, think like BASIC, compile to anything.├── CMakeLists.txt
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