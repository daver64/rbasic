# rbasic - Modern BASIC for Raspberry Pi & Single Board Computers

**rbasic** is a modern BASIC language transpiler designed for Raspberry Pi, embedded systems, and single-board computers. It combines BASIC's simplicity with C-style syntax, native hardware access, and the ability to compile to optimised native executables.

🎯 **Primary Target**: Raspberry Pi Zero 2W, Pi 3/4/5, and compatible ARM single-board computers

⚠️ **Alpha Software**: This project is in early development. Features may be incomplete, unstable, or subject to breaking changes.

## Why rbasic for Embedded Systems?

- **🔌 Native Hardware Access**: Direct GPIO, SPI, I2C, PWM, and UART control without external libraries
- **🎨 Graphics & Database**: Optional SDL2 for GUIs and SQLite3 for data storage (build-time configurable)
- **⚡ Dual Execution**: Rapid prototyping with interpreter (`-i`), deploy with native compilation (`-c`)
- **🎮 Interactive Development**: REPL mode for testing hardware in real-time
- **📦 Zero Dependencies**: Self-contained executables, optional libraries linked only when needed
- **🚀 Performance**: Near-C++ speeds with automatic multi-core optimisation
- **🔄 Cross-Compilation**: Develop on PC, compile for ARM from any platform
- **💾 Small Footprint**: Minimal memory usage, perfect for constrained systems

## Current Alpha Features

- **C-Style Syntax**: Modern control structures with braces `{}` and proper variable scoping
- **Dual Execution**: Interpret for development (`-i`) or compile to native executables (`-c`) with identical behaviour
- **Interactive REPL**: Read-Eval-Print Loop for rapid hardware prototyping (`-r`)
- **Import System**: Complete modular programming with `import "file.bas"` syntax
- **GLM Vector Math**: Native vec2, vec3, vec4 types with SIMD-optimised operations
- **Automatic Parallelisation**: OpenMP-based multi-core optimisation for large array operations
- **SDL2 Graphics** *(optional)*: Complete window management, rendering, events, and hardware acceleration
- **SQLite3 Database** *(optional)*: Embedded database with prepared statements and transactions
- **Raspberry Pi Hardware**: Native GPIO, SPI, I2C, PWM, and Serial (UART) access on ARM platforms
- **Cross-Platform Development**: Windows (MinGW64/MSVC), Linux (GCC), macOS (Clang) with ARM cross-compilation

## Recent Improvements (November 2025)

### SDL2 & SQLite3 Integration (November 2025)
- **Complete SDL2 Graphics Support**: Window management, hardware-accelerated rendering, event handling
  - 40+ SDL2 functions with resource management
  - SDL2_gfx integration for filled shapes (circles, ellipses, triangles)
  - SDL2_image support for texture loading (PNG, JPG, BMP)
  - Full keyboard/mouse event system with scan code constants
  - Identical behavior in interpreter and compiled modes
- **SQLite3 Database Support**: Full-featured embedded database integration
  - 25+ SQLite3 functions including prepared statements
  - Transaction support with commit/rollback
  - Complete CRUD operations with type-safe bindings
  - 30+ built-in constants (SQLITE_OK, SQLITE_ROW, etc.)
  - Works in both interpreter and compiled modes
- **Constant System**: 50+ SDL2 and 30+ SQLite3 constants accessible by name
  - SDL_INIT_VIDEO, SDL_WINDOW_SHOWN, SDL_RENDERER_ACCELERATED
  - SQLITE_OK, SQLITE_ROW, SQLITE_DONE
  - Dynamic SDL scan code resolution for keyboard input
- **Optional Compilation**: Enable with `-DWITH_SDL2=ON` and `-DWITH_SQLITE3=ON` flags
- **Cross-Compilation Ready**: Library linking configured for native and cross-platform builds

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

### On Raspberry Pi
```bash
# Blink an LED connected to GPIO 17
./rbasic -i examples/rpi_gpio_blink.bas

# Control a servo on GPIO 18 (PWM)
./rbasic -i examples/rpi_pwm_servo.bas

# Compile for production deployment
./rbasic -c examples/rpi_button_led.bas -o button_led
./button_led

# Interactive hardware testing
./rbasic -r
```

### On Development PC
```bash
# Interpret a program directly (fast development)
./rbasic -i hello.bas

# Compile to standalone executable
./rbasic -c hello.bas -o hello
./hello

# Interactive REPL for algorithm testing
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

# Standard build (interpreter + compiler only)
cmake .. && cmake --build . --config Release

# With SDL2 graphics support
cmake -DWITH_SDL2=ON .. && cmake --build . --config Release

# With SQLite3 database support
cmake -DWITH_SQLITE3=ON .. && cmake --build . --config Release

# With both SDL2 and SQLite3
cmake -DWITH_SDL2=ON -DWITH_SQLITE3=ON .. && cmake --build . --config Release

# Cross-compile for Raspberry Pi (from x86/x64 Linux)
cmake -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake -DWITH_SQLITE3=ON .. && make -j$(nproc)

# Windows convenience script
# build.bat
```

**Note**: SDL2 and SQLite3 support requires development libraries:
```bash
# Ubuntu/Debian/Raspberry Pi OS
sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsqlite3-dev

# Fedora/RHEL
sudo dnf install SDL2-devel SDL2_gfx-devel SDL2_image-devel sqlite-devel

# macOS
brew install sdl2 sdl2_gfx sdl2_image sqlite3
```

### 2. Try the Examples

#### Raspberry Pi Hardware Examples
```bash
# GPIO LED control
./rbasic -i examples/rpi_gpio_blink.bas
./rbasic -i examples/rpi_button_led.bas

# PWM servo control
./rbasic -i examples/rpi_pwm_servo.bas

# SPI sensor communication
./rbasic -i examples/rpi_spi_sensor.bas

# System monitoring and file I/O
./rbasic -i examples/rpi_temperature_logger.bas

# Board LED control (requires sudo)
sudo ./rbasic -i examples/rpi_act_led_blink.bas
```

#### SDL2 Graphics Examples (requires -DWITH_SDL2=ON)
```bash
# Simple window with rendering
./rbasic -i examples/sdl_simple_window.bas

# Compile SDL2 program to executable
./rbasic -c examples/sdl_simple_window.bas -o sdl_demo
./sdl_demo
```

#### SQLite3 Database Examples (requires -DWITH_SQLITE3=ON)
```bash
# Complete database operations
./rbasic -i examples/sqlite_demo.bas

# Compile database program
./rbasic -c examples/sqlite_demo.bas -o db_demo
./db_demo
```

#### General Programming Examples
```bash
# Algorithm development
./rbasic -i examples/fibonacci.bas
./rbasic -i examples/functions.bas

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
# Standard build (core language only)
mkdir build && cd build
cmake .. && cmake --build .

# With optional libraries
cmake -DWITH_SDL2=ON -DWITH_SQLITE3=ON .. && cmake --build .

# Cross-compile for Raspberry Pi
cmake -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake .. && make

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
- ✅ Complete SDL2 graphics integration with hardware acceleration and event handling (optional)
- ✅ Complete SQLite3 database integration with prepared statements and transactions (optional)
- ✅ Import system for modular programming with compile-time resolution
- ✅ Cross-platform support (Windows/Linux/macOS) with bundled MinGW64
- ✅ Performance-optimised runtime architecture
- ✅ Interactive REPL for development with multi-line support
- ✅ Native Raspberry Pi hardware support (GPIO, SPI, I2C, PWM, Serial)
- ✅ Cross-compilation toolchain for ARM/Raspberry Pi targets

**Recently Fixed Issues:**
- ✅ SDL2 and SQLite3 namespace integration and linking (November 2025)
- ✅ Constant system for SDL2 and SQLite3 in both execution modes (November 2025)
- ✅ Cross-compilation library linking for optional dependencies (November 2025)
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

### Featured Examples

### Raspberry Pi Hardware Programming
- **`examples/rpi_gpio_blink.bas`** - LED control with GPIO output
- **`examples/rpi_button_led.bas`** - Interactive button input with LED toggle
- **`examples/rpi_pwm_servo.bas`** - Hardware PWM servo motor control (0-180 degrees)
- **`examples/rpi_spi_sensor.bas`** - SPI communication with MCP3008 ADC chip
- **`examples/rpi_i2c_bme280.bas`** - I2C temperature/humidity sensor reading
- **`examples/rpi_temperature_logger.bas`** - System monitoring with file logging
- **`examples/rpi_act_led_blink.bas`** - Control onboard Activity LED
- **`examples/rpi_distance_sensor.bas`** - HC-SR04 ultrasonic distance measurement

### Graphics Programming (SDL2 - optional)
- **`examples/sdl_simple_window.bas`** - Window creation with rendering and events
- **`examples/sqlite_demo.bas`** - Complete database demo with CRUD operations

### Database Integration (SQLite3 - optional)
- **`examples/sqlite_demo.bas`** - Full database operations with prepared statements

### Algorithm Development
- **`examples/functions.bas`** - Function definitions, parameters, scoping
- **`examples/glm_vector_math.bas`** - GLM vector mathematics for 3D graphics
- **`examples/arrays.bas`** - Array operations and multidimensional data
- **`examples/fibonacci.bas`** - Recursive functions and computations

## Documentation

- **[Language Grammar (BNF)](BNF.md)** - Complete syntax specification
- **[Examples](examples/)** - Sample programs demonstrating features

## License

MIT License - Copyright (c) 2025 David Rowbotham. See LICENSE file for details.

---

**rbasic** - Modern BASIC designed for Raspberry Pi and embedded systems. Combining BASIC's simplicity with native hardware access, C-style syntax, and the performance of compiled executables. Perfect for robotics, IoT, sensor projects, and embedded application development.

