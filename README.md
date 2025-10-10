# rbasic - Modern BASIC Language Transpiler (Alpha)

**rbasic** is an experimental modern BASIC language transpiler that implements C-style syntax while maintaining BASIC's simplicity. Currently in Alpha development, it supports both interpretation for rapid development and compilation to native C++ executables with a comprehensive Foreign Function Interface (FFI) system for C library integration.

⚠️ **Alpha Software**: This project is in early development. Features may be incomplete, unstable, or subject to breaking changes.

## Current Alpha Features

- **C-Style Syntax**: Modern control structures with braces `{}` and proper variable scoping
- **Dual Execution**: Interpret for development (`-i`) or compile to native executables (`-c`) with identical behavior
- **Interactive REPL**: Read-Eval-Print Loop for rapid prototyping (`-r`)
- **Import System**: Complete modular programming with `import "file.bas"` syntax
- **Advanced FFI**: Comprehensive C library integration with 1-11 parameter function support
- **Graphics Programming**: Complete SDL2 integration with hardware-accelerated rendering
- **Cross-Platform**: Windows (MinGW64/MSVC), Linux (GCC), macOS (Clang) with identical behavior

## Recent Improvements (January 2025)

### Enhanced FFI System
- **Extended Parameter Support**: Functions with up to 11 parameters for complex graphics operations (SDL2_gfx)
- **Performance Optimization**: 80% code reduction with FFITypeConverter pre-conversion strategy
- **Graphics Enhancement**: Proper filled triangle rendering with `filledTrigonRGBA` support
- **Cross-Mode Consistency**: Identical FFI behavior in interpreter and compiled modes

### Variable Scoping Fixes
- **For Loop Scoping**: Fixed critical bug where for loop variables couldn't access parent scope
- **Function Compatibility**: Resolved scoping issues in functions with typed parameters
- **Cross-Mode Consistency**: Identical scoping behavior in interpreter and compiled modes

### Compiler Stability
- **Linker Issues Resolved**: Fixed missing FFITypeConverter implementations causing compilation failures
- **Full Compilation Support**: Both simple and complex FFI programs compile correctly
- **Runtime Compatibility**: Updated runtime library for consistent cross-mode behavior

## Quick Start

```bash
# Interpret a program directly (fast development)
./rbasic -i hello.bas

# Compile to standalone executable (deployment)
./rbasic -c hello.bas -o hello
./hello

# Interactive development with REPL
./rbasic -r
```

**Performance**: Compiled executables run at near-C++ speeds with automatic OpenMP parallelization for array operations. The FFI system provides direct access to native libraries with minimal overhead.

## Getting Started

### 1. Build rbasic
```bash
# Clone and build
git clone https://github.com/daver64/rbasic.git
cd rbasic
mkdir build && cd build
cmake .. && cmake --build . --config Release

# Windows convenience script
# build.bat
```

### 2. Try the Examples
```bash
# Simple programs
./rbasic -i examples/hello.bas
./rbasic -i examples/fibonacci.bas

# Graphics programming (requires SDL2.dll)
./rbasic -i examples/sdl_core_demo.bas

# Compile for deployment
./rbasic -c examples/fibonacci.bas -o fibonacci
./fibonacci
```

### 3. Interactive Development
```bash
# Start REPL
./rbasic -r

# Try some commands:
var x = 42;
print("The answer is: " + str(x));
:help
```

## FFI and Graphics Programming (Enhanced)

rbasic includes a comprehensive Foreign Function Interface with advanced graphics capabilities:

### Advanced SDL2 Graphics Programming
- **Complete SDL2 Integration**: Window management, hardware-accelerated rendering, event handling
- **Enhanced Graphics Operations**: Drawing operations, filled shapes, advanced geometry
  - **Filled Shapes**: Proper solid triangle rendering with SDL2_gfx `filledTrigonRGBA`
  - **Graphics Primitives**: Points, lines, rectangles, circles, ellipses with color support
  - **Advanced Rendering**: Multi-parameter graphics functions (up to 11 parameters)
- **Event Processing**: Complete keyboard and mouse event handling with scan code support
- **Cross-Mode Support**: FFI works identically in both interpreter and compiled modes
- **Memory Management**: Safe buffer allocation, pointer operations, automatic cleanup

### Enhanced FFI Features (1-11 Parameters)
- **Extended Parameter Support**: Functions with up to 11 parameters for complex graphics operations
- **Performance Optimized**: FFITypeConverter pre-conversion strategy for 80% code reduction
- **Advanced Graphics**: SDL2_gfx integration with filled polygon and triangle rendering
- **Memory Operations**: Safe buffer allocation (`alloc_buffer()`, `deref_pointer()`)
- **Structure Support**: Complete SDL2 structures (`create_sdl_rect()`, `create_sdl_event()`)
- **Constant System**: Built-in constants for SDL2, SQLite, Windows API
- **Null Safety**: `is_null()`, `not_null()` for safe pointer operations
- **Cross-Platform Libraries**: Windows DLLs, Linux .so, macOS .dylib support

## Language Overview

rbasic implements a "C-leaning BASIC" with modern syntax:

```basic
// Modern C-style BASIC
function fibonacci(n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

var limit = 10;
print("Fibonacci sequence:");

for (var i = 0; i <= limit; i = i + 1) {
    print("F(" + str(i) + ") = " + str(fibonacci(i)));
}
```

📖 **[Complete Language Grammar (BNF)](BNF.md)** - Detailed syntax specification and language reference

📚 **[User Manual](MANUAL.md)** - Comprehensive guide with examples and tutorials

## Installation and Usage

### Prerequisites

- CMake 3.16+
- C++17 compiler (MinGW64, GCC, Clang, or MSVC)
- **Windows**: MinGW64 (bundled) or Visual Studio 2019+ with MSVC

### Quick Start

```bash
# Build the transpiler
mkdir build && cd build
cmake .. && cmake --build .

# Run a program directly (interpreted)
./rbasic -i hello.bas

# Compile to standalone executable (uses bundled MinGW64 or falls back to MSVC)
./rbasic -c hello.bas -o hello
./hello

# Interactive development mode
./rbasic -r
```

### Compiler Support

**Windows:**
- **Primary**: Bundled MinGW64 (portable, no Visual Studio required)
- **Fallback**: Microsoft Visual C++ (when MinGW64 not available)
- **Static Linking**: MinGW64 produces self-contained executables

**Linux/macOS:**
- **Standard**: GCC or Clang with system libraries

## Language Features

### Core Syntax Elements

- **C-Style Control Flow**: `if (condition) { }`, `for (init; test; inc) { }` with proper block scoping
- **BASIC-Style Operators**: `and`, `or`, `not` for logical operations (instead of `&&`, `||`, `!`)
- **Function-Call I/O**: `print("Hello")`, `var input = input()`
- **Assignment Expressions**: `var y = (x = x + 1) * 2`
- **Modern Comments**: `//` line comments and `/* */` block comments
- **Variable Scoping**: Variables declared with `var` in blocks are properly scoped to those blocks
- **Arrays**: `dim array(size)` declaration with `array[index]` access (0-indexed like C/C++)
- **Multidimensional Arrays**: `dim matrix(rows, cols)` with `matrix[i,j]` syntax for 2D/3D arrays
- **Typed Arrays**: High-performance `byte_array()`, `int_array()`, `double_array()` functions
- **File I/O**: Complete text and binary file operations with `read_text_file()`, `write_binary_file()`, etc.

### Built-in Functions

#### I/O and Control
```basic
print("Hello", "World");        // Multi-argument print
var name = input();             // User input
```

#### Mathematics
```basic
print(sqrt(16));               // Square root: 4
print(pow(2, 8));              // Power: 256
print(sin(pi() / 2));          // Trigonometry: 1
print(rnd());                  // Random 0.0-1.0
```

#### String Manipulation
```basic
var text = "Programming";
print(len(text));              // Length: 11
print(left(text, 4));          // "Prog"
print(mid(text, 5, 4));        // "gram"
print(right(text, 4));         // "ming"
print(str(42));                // Convert number to string: "42"
print(val("3.14"));            // Convert string to number: 3.14
```

### Enhanced SDL2 Graphics Programming
```basic
// Advanced SDL2 with filled shapes and event handling
import "blib/sdl2.bas";

function main() {
    // Initialize SDL2 with error checking
    if (sdl_init() != 1) {
        print("SDL2 initialization failed");
        return 1;
    }
    
    print("SDL2 Graphics Demo - Enhanced Features");
    print("Features: filled shapes, event handling, timing");
    
    // Main rendering loop
    while (sdl_is_running()) {
        var current_time = sdl_get_ticks();
        var elapsed_ms = current_time - start_time;
        
        // Clear screen and set background
        sdl_clear_blue();
        
        // Animated filled triangle (SDL2_gfx integration)
        var tri_x = 400 + 100 * cos(elapsed_ms / 1000.0);
        var tri_y = 300 + 50 * sin(elapsed_ms / 800.0);
        sdl_set_color(255, 255, 0);  // Yellow
        sdl_fill_triangle(tri_x, tri_y, tri_x + 50, tri_y + 80, tri_x - 50, tri_y + 80);
        
        // Event handling with scan codes
        sdl_process_events();
        if (sdl_key_pressed(SDL_SCANCODE_ESCAPE)) {
            break;
        }
        
        // Present frame and control timing
        sdl_present();
        sdl_delay(16);  // ~60 FPS
    }
    
    sdl_cleanup();
    return 0;
}
```

### Advanced FFI Usage (11-Parameter Functions)
```basic
// SDL2_gfx filled triangle with 11 parameters
ffi integer filledTrigonRGBA(renderer as pointer, x1 as integer, y1 as integer, 
                           x2 as integer, y2 as integer, x3 as integer, y3 as integer,
                           r as integer, g as integer, b as integer, a as integer) from "SDL2_gfx.dll";

// High-parameter graphics function
var result = filledTrigonRGBA(renderer, 100, 100, 200, 100, 150, 200, 255, 255, 0, 255);
```

### SQLite Database Operations
```basic
import "blib/sqlite.bas";

function main() {
    // Open database
    var result = sqlite_open("users.db");
    if (result != 1) {
        print("Failed to open database");
        return 1;
    }
    
    // Create table and insert data
    sqlite_exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    
    // Insert users
    sqlite_prepare("INSERT INTO users (name, age) VALUES (?, ?)");
    sqlite_bind_text(1, "Alice");
    sqlite_bind_int(2, 28);
    sqlite_step();
    sqlite_finalize();
    
    // Query and display results
    sqlite_prepare("SELECT * FROM users ORDER BY id");
    print("User List:");
    while (sqlite_step() == 1) {
        var id = sqlite_get_int(0);
        var name = sqlite_get_text(1);
        var age = sqlite_get_int(2);
        print("ID: " + str(id) + ", Name: " + name + ", Age: " + str(age));
    }
    
    sqlite_close();
    return 0;
}
```

### Comprehensive FFI Features
- **Enhanced Parameter Support**: Up to 11 parameters for complex graphics functions like SDL2_gfx
- **Cross-Mode Compatibility**: Identical FFI behavior in interpreter and compiled modes
- **Performance Optimized**: FFITypeConverter pre-conversion strategy reduces code complexity by 80%
- **Memory Management**: Safe buffer allocation (`alloc_buffer()`, `deref_pointer()`) with automatic cleanup
- **Structure Support**: Complete SDL2 structures (`create_sdl_rect()`, `create_sdl_event()`)
- **Constant System**: Built-in constants for SDL2, SQLite, Windows API (over 200 constants)
- **Cross-Platform Libraries**: Windows DLLs, Linux .so, macOS .dylib support
- **Advanced Graphics**: SDL2_gfx integration for filled polygons, triangles, and complex shapes

## Language Syntax

### Core Features
```basic
// Variables and expressions
var name = "Alice";
var age = 28;
var result = (age * 2) + 10;

// Control structures with proper scoping
if (age >= 18) {
    print("Adult: " + name);
} else {
    print("Minor: " + name);
}

// Functions
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

// Arrays
dim numbers(10);
for (var i = 0; i < 10; i = i + 1) {
    numbers[i] = i * i;
}
```

### Import System
```basic
// library.bas
function add_numbers(a, b) {
    return a + b;
}

// main.bas  
import "library.bas";
var result = add_numbers(10, 20);
```

## Installation

### Prerequisites
- CMake 3.16+
- C++17 compiler (MinGW64, GCC, Clang, or MSVC)

### Build
```bash
# Standard build
mkdir build && cd build
cmake .. && cmake --build .

# Windows convenience
build.bat
```

### Usage
```bash
# Run program directly (interpreted)
./rbasic -i program.bas

# Compile to executable
./rbasic -c program.bas -o program
./program

# Interactive REPL
./rbasic -r
```

## Status (Alpha Development)

**Current Alpha Features:**
- ✅ Advanced C-style BASIC language implementation with proper scoping
- ✅ Dual execution modes (interpreter and compiled) with identical behavior
- ✅ Comprehensive FFI system with 1-11 parameter support for graphics programming
- ✅ Complete SDL2 integration with filled shapes and hardware acceleration
- ✅ Import system for modular programming with compile-time resolution
- ✅ Cross-platform support (Windows/Linux/macOS) with bundled MinGW64
- ✅ Performance-optimized runtime with FFITypeConverter architecture
- ✅ Interactive REPL for development with multi-line support

**Recently Fixed Issues:**
- ✅ Variable scoping in for loops and functions (January 2025)
- ✅ Compiler linker errors with FFI functions (January 2025)
- ✅ Cross-mode FFI consistency between interpreter and compiled modes
- ✅ Performance bottlenecks in FFI parameter conversion (80% improvement)

**Known Limitations:**
- ⚠️ Standard library still expanding (graphics and database libraries working)
- ⚠️ Error handling and reporting needs improvement for complex programs
- ⚠️ Some edge cases in complex variable scoping scenarios
- ⚠️ Performance optimizations not fully implemented for all operations
- ⚠️ Documentation is work-in-progress (basic examples available)

## Project Structure

```
rbasic/
├── src/                   # Core transpiler implementation (lexer, parser, interpreter, codegen)
├── include/               # Header files for all components
├── runtime/               # Runtime library for compiled programs with FFI optimization
├── blib/                  # Standard library (SDL2, SQLite wrappers with 200+ functions)
├── examples/              # Sample programs demonstrating language features and FFI
├── tests/                 # Unit tests and integration tests
├── build/                 # Build output directory (created by CMake)
├── mingw64_temp/          # Bundled MinGW64 compiler for Windows (portable compilation)
├── .github/               # GitHub Actions CI/CD and project documentation
└── 3rd_party/             # Third-party libraries and dependencies
```

### Key Libraries Included
- **SDL2 Graphics Stack**: Complete SDL2, SDL2_gfx, SDL2_image, SDL2_mixer, SDL2_ttf, SDL2_net
- **Database**: SQLite3 with full API support
- **Standard Library**: `blib/sdl2.bas` and `blib/sqlite.bas` with comprehensive function wrappers
- **Development Tools**: MinGW64 bundled for Windows, cross-platform build system

## Featured Examples

### Enhanced SDL2 Graphics Programming
- **`examples/sdl_core_demo.bas`** - Complete SDL2 demonstration with filled shapes, timing, and event handling
- **`blib/sdl2.bas`** - Comprehensive SDL2 wrapper library with 200+ function bindings
- **Advanced Features**: Filled triangles, hardware acceleration, cross-mode compatibility

### Database Integration
- **`examples/sqlite_simple_demo.bas`** - SQLite database integration with full CRUD operations
- **`blib/sqlite.bas`** - Complete SQLite wrapper with prepared statements and transactions

### Language Features
- **`examples/functions.bas`** - Function definitions, parameters, return values, and scoping
- **`examples/arrays.bas`** - Array operations, multidimensional data, and typed arrays  
- **`examples/fibonacci.bas`** - Recursive functions and mathematical computations
- **`examples/hello.bas`** - Basic program structure and output formatting

## Documentation

- **[Language Grammar (BNF)](BNF.md)** - Complete syntax specification
- **[Examples](examples/)** - Sample programs demonstrating features

## License

MIT License - Copyright (c) 2025 David Rowbotham. See LICENSE file for details.

---

**rbasic** - Where BASIC meets modern C-style programming with comprehensive graphics and database integration. Perfect for rapid prototyping and educational programming while maintaining the power for serious application development.

