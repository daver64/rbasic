# rbasic - Modern BASIC Language Transpiler

**rbasic** is a modern BASIC language transpiler that implements C-style syntax while maintaining BASIC's simplicity. It supports both interpretation for rapid development and compilation to native C++ executables with a comprehensive Foreign Function Interface (FFI) system for direct C library integration.

## Key Features

- **C-Style Syntax**: Modern control structures with braces `{}` and proper scoping
- **Dual Execution**: Interpret for development (`-i`) or compile to native executables (`-c`) 
- **Interactive REPL**: Read-Eval-Print Loop for rapid prototyping (`-r`)
- **Import System**: Modular programming with `import "file.bas"` syntax
- **Foreign Function Interface**: Direct integration with C libraries (SDL2, SQLite, Windows API)
- **Comprehensive Standard Library**: Pre-built wrappers for graphics, databases, and system APIs
- **Automatic Parallelization**: OpenMP-based multi-core optimization for large arrays
- **Cross-Platform**: Windows (MinGW64/MSVC), Linux (GCC), macOS (Clang)

## Quick Start

```bash
# Interpret a program directly
./rbasic -i hello.bas

# Compile to standalone executable  
./rbasic -c hello.bas -o hello
./hello

# Interactive development
./rbasic -r
```

## FFI and Library Integration

rbasic includes a production-ready Foreign Function Interface with comprehensive wrapper libraries:

## Key Features

- **C-Style Syntax**: Uses braces `{}`, parentheses for conditions, and modern control flow
- **Transpiler Architecture**: Compiles BASIC to C++ executables or runs directly via interpreter  
- **Interactive REPL**: Read-Eval-Print Loop for rapid development and testing
- **Import System**: Modular programming with `import "file.bas"` for code organization
- **Automatic Parallelization**: OpenMP-based automatic multi-core optimization for large array operations
- **Comprehensive I/O**: Text and binary file operations with full file system support
- **Typed Arrays**: High-performance `byte_array()`, `int_array()`, `double_array()` for numerical computing
- **Proper Scoping**: C-style variable scoping within control structures (if/for/while blocks)
- **Dual Execution**: Identical behavior in both interpreted and compiled modes
- **Zero Dependencies**: Core language has no external dependencies  
- **Cross-Platform**: Supports Windows, Linux, and macOS with native compilers
- **Foreign Function Interface (FFI)**: Direct integration with C libraries and system APIs

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

### SDL2 Graphics Programming
```basic
import "blib/sdl2.bas";

function main() {
    // Initialize SDL2
    var result = SDL_Init(SDL_INIT_VIDEO);
    if (result != 0) {
        print("SDL2 initialization failed");
        return 1;
    }
    
    // Create window
    var window = SDL_CreateWindow("rbasic Graphics", 
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  800, 600, SDL_WINDOW_SHOWN);
    
    // Main event loop with graphics
    var renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    var running = true;
    
    while (running) {
        // Handle events
        var event = create_sdl_event();
        while (SDL_PollEvent(event) != 0) {
            if (get_event_type(event) == SDL_QUIT) {
                running = false;
            }
        }
        
        // Clear screen and draw
        SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
        SDL_RenderClear(renderer);
        
        // Draw rectangle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        var rect = create_sdl_rect(100, 100, 200, 150);
        SDL_RenderFillRect(renderer, rect);
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
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
- **Generic Type System**: Automatic conversion between BASIC and C types (int, double, string, pointer)
- **Memory Management**: Safe buffer allocation (`alloc_buffer()`, `deref_pointer()`) 
- **Structure Support**: SDL2 structures (`create_sdl_rect()`, `create_sdl_event()`)
- **Constant System**: Built-in constants for SDL2, SQLite, Windows API
- **Cross-Platform Libraries**: Windows DLLs, Linux .so, macOS .dylib support
- **Production Ready**: Complete wrapper libraries included in `blib/` directory

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

## Status

**Production Ready Features:**
- ✅ Complete C-style BASIC language implementation
- ✅ Dual execution modes with identical behavior
- ✅ Foreign Function Interface with SDL2/SQLite wrapper libraries
- ✅ Import system for modular programming
- ✅ Cross-platform support (Windows/Linux/macOS)
- ✅ Comprehensive built-in function library
- ✅ Interactive REPL for development

## Project Structure

```
rbasic/
├── src/                   # Core transpiler implementation  
├── include/               # Header files
├── runtime/               # Runtime library for compiled programs
├── blib/                  # Standard library (SQLite, SDL2 wrappers)
├── examples/              # Sample programs
├── tests/                 # Unit tests
└── build/                 # Build output directory
```

## Documentation

- **[Language Grammar (BNF)](BNF.md)** - Complete syntax specification
- **[Examples](examples/)** - Sample programs demonstrating features

## License

MIT License - See LICENSE file for details.

