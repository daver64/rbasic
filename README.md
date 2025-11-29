# rbasic - Modern BASIC Language Transpiler (Alpha)

**rbasic** is an experimental modern BASIC language transpiler that implements C-style syntax while maintaining BASIC's simplicity. Currently in Alpha development, it supports both interpretation for rapid development and compilation to native C++ executables.

⚠️ **Alpha Software**: This project is in early development. Features may be incomplete, unstable, or subject to breaking changes.

## Current Alpha Features

- **C-Style Syntax**: Modern control structures with braces `{}` and proper variable scoping
- **Dual Execution**: Interpret for development (`-i`) or compile to native executables (`-c`) with identical behaviour
- **Interactive REPL**: Read-Eval-Print Loop for rapid prototyping (`-r`)
- **Import System**: Complete modular programming with `import "file.bas"` syntax
- **GLM Vector Math**: Native vec2, vec3, vec4 types with SIMD-optimised operations
- **Automatic Parallelisation**: OpenMP-based multi-core optimisation for large array operations
- **Raspberry Pi Support**: Native GPIO, SPI, I2C, PWM, and Serial access on ARM platforms
- **Cross-Platform**: Windows (MinGW64/MSVC), Linux (GCC), macOS (Clang), and ARM (Raspberry Pi) with identical behaviour

## Recent Improvements (November 2025)

### Raspberry Pi Hardware Support (November 2025)
- **Native GPIO Access**: Direct hardware control via /dev/gpiomem (no root required)
- **SPI Communication**: High-speed serial peripheral interface for sensors and displays
- **I2C Support**: Two-wire interface for sensor communication (BME280, OLED, etc.)
- **PWM Control**: Hardware PWM for servos, LED dimming, and motor control
- **Serial (UART)**: Asynchronous serial communication for GPS, Arduino, etc.
- **Conditional Compilation**: RPI functions only available on ARM platforms, x86/x64 compatibility maintained
- **Cross-Compilation**: Complete toolchain support for building on x86 for Raspberry Pi
- **See**: [RASPBERRY_PI.md](RASPBERRY_PI.md) for comprehensive hardware documentation

### Code Simplification (November 2025)
- **Cleaner Architecture**: Simplified codebase focuses on core language features and built-in functionality
- **Native Hardware Support**: Direct implementation of Raspberry Pi GPIO/SPI/I2C with conditional compilation

### Complete GLM Vector Mathematics Integration (October 2025)
- **Native GLM Types**: Built-in `vec2`, `vec3`, `vec4`, `mat3`, `mat4`, `quat` primitives with SIMD optimisation
- **Component Access**: Full read/write access with `.x`, `.y`, `.z`, `.w` component syntax
- **Vector Operations**: Native support for addition (`+`), subtraction (`-`), scalar multiplication (`*`)
- **GLM Functions**: `length()`, `normalize()`, `dot()`, `cross()` functions with mathematical accuracy
- **Dual-Mode Support**: Identical GLM behaviour in interpreter and compiled modes with native performance
- **Graphics Programming**: Professional-grade vector math for 3D graphics, game development, and scientific computing

### Variable Scoping Fixes
- **For Loop Scoping**: Fixed critical bug where for loop variables couldn't access parent scope
- **Function Compatibility**: Resolved scoping issues in functions with typed parameters
- **Cross-Mode Consistency**: Identical scoping behaviour in interpreter and compiled modes

### Compiler Stability
- **Linker Issues Resolved**: Fixed compilation failures and improved build reliability
- **Full Compilation Support**: Complex programs compile correctly with proper runtime integration
- **Runtime Compatibility**: Updated runtime library for consistent cross-mode behaviour

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

**Performance**: Compiled executables run at near-C++ speeds with automatic OpenMP parallelisation for array operations.

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
- **GLM Vector Math**: Native `vec2`, `vec3`, `vec4` types with component access and mathematical operations
- **Multidimensional Arrays**: `dim matrix(rows, cols)` with `matrix[i,j]` syntax for 2D/3D arrays
- **Typed Arrays**: High-performance `byte_array()`, `int_array()`, `double_array()` functions
- **File I/O**: Complete text and binary file operations with `read_text_file()`, `write_binary_file()`, etc.

### Built-in Functions

#### I/O and Control
```basic
print("Hello", "World");        // Multi-argument print
var name = input();             // User input
```

#### Mathematics and GLM Vector Operations
```basic
print(sqrt(16));               // Square root: 4
print(pow(2, 8));              // Power: 256
print(sin(pi() / 2));          // Trigonometry: 1
print(rnd());                  // Random 0.0-1.0

// GLM Vector Mathematics
var position = vec3(10.0, 20.0, 30.0);
position.x = 5.0;              // Component assignment
var velocity = vec3(1.0, 2.0, 3.0);
var result = position + velocity * 2.0;  // Vector operations
print("Length: " + length(result));      // Vector length
var normalized = normalize(result);       // Unit vector
var dotProduct = dot(position, velocity); // Dot product
var crossProduct = cross(position, velocity); // Cross product (3D only)
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

### Array Operations
```basic
// Standard arrays
dim numbers(10);
for (var i = 0; i < 10; i = i + 1) {
    numbers[i] = i * i;
}

// Typed arrays for performance
var bytes = byte_array(1024);    // Memory-efficient byte storage
var integers = int_array(100);   // Fast integer array
var doubles = double_array(50);  // High-precision floating point

// Multidimensional arrays
dim matrix(3, 3);
matrix[0,0] = 1;
matrix[1,1] = 5;
matrix[2,2] = 9;
```

### File I/O
```basic
// Text file operations
write_text_file("output.txt", "Hello, World!");
var content = read_text_file("output.txt");
print(content);

// Binary file operations
var data = byte_array(256);
// ... fill data ...
write_binary_file("data.bin", data, 256);
```

## Architecture

rbasic is designed as a transpiler where interpreter, REPL, and compiled output produce identical results by sharing the same AST representation and runtime function implementations:

1. **Rapid Prototyping**: Use interpreter/REPL mode for development
2. **Deployment**: Compile to optimised C++ executables  
3. **Modular Development**: Import system enables standard library ecosystem
4. **Cross-Platform Consistency**: Same behaviour on Windows, Linux, macOS, and ARM platforms

## Performance

Compiled executables run at near-C++ speeds with automatic OpenMP parallelisation for array operations. The transpiled code is optimised and benefits from the host C++ compiler's optimisation passes.

- **Compilation Speed**: Fast transpilation and compilation (sub-second for small programs)
- **Runtime Speed**: Native machine code performance
- **Parallel Arrays**: Automatic OpenMP optimisation for large array operations (≥1000 elements)
- **Memory Efficient**: Direct memory management, no garbage collection overhead


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

// GLM Vector Mathematics
var position = vec3(10.0, 20.0, 30.0);
position.x = 5.0;                          // Component assignment
var velocity = normalize(vec3(1, 2, 3));   // Unit vector
var newPos = position + velocity * 2.0;    // Vector arithmetic
var distance = length(newPos);             // Vector magnitude
print("New position: (" + newPos.x + ", " + newPos.y + ", " + newPos.z + ")");
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
- ✅ Complete GLM vector mathematics integration with native performance and component access
- ✅ Dual execution modes (interpreter and compiled) with identical behaviour
- ✅ Complete SDL2 integration with filled shapes and hardware acceleration
- ✅ Import system for modular programming with compile-time resolution
- ✅ Cross-platform support (Windows/Linux/macOS) with bundled MinGW64
- ✅ Performance-optimised runtime architecture
- ✅ Interactive REPL for development with multi-line support
- ✅ Native Raspberry Pi hardware support (GPIO, SPI, I2C, PWM, Serial)

**Recently Fixed Issues:**
- ✅ Texture rendering compatibility between interpreter and compiled modes (October 2025)
- ✅ SDL_RenderCopy pattern matching and cross-mode consistency (October 2025)
- ✅ Memory leak in create_sdl_rect with cached buffer implementation (October 2025)
- ✅ Variable scoping in for loops and functions (October 2025)
- ✅ Compiler linker errors resolved (October 2025)
- ✅ Cross-mode consistency between interpreter and compiled modes
- ✅ Performance optimisations in runtime implementation (80% improvement)

**Known Limitations:**
- ⚠️ **Error Reporting**: Main execution mode needs more detailed error messages with line numbers
- ⚠️ **Parser Diagnostics**: Limited syntax error reporting compared to modern language tools  
- ⚠️ **Import Error Details**: Basic import failure messages could be more descriptive
- ⚠️ **Compilation Diagnostics**: C++ compilation errors not always user-friendly for BASIC programmers
- ⚠️ Standard library still expanding (graphics and database libraries working)
- ⚠️ Some edge cases in complex variable scoping scenarios
- ⚠️ Performance optimizations not fully implemented for all operations
- ⚠️ Documentation is work-in-progress (basic examples available)

## Project Structure

```
rbasic/
├── src/                   # Core transpiler implementation (lexer, parser, interpreter, codegen)
├── include/               # Header files for all components
├── runtime/               # Runtime library for compiled programs
├── blib/                  # Standard library (SDL2, SQLite wrappers with 200+ functions)
├── examples/              # Sample programs demonstrating language features
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
- **`examples/glm_vector_math.bas`** - Comprehensive GLM vector mathematics with all operations and practical examples
- **`examples/glm_test.bas`** - Complete GLM test suite demonstrating all vector math features
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

