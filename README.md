# rbasic - C-Leaning BASIC Language Transpiler

**rbasic** is a modern BASIC### Interactive REPL Mode

rbasic includes a powerful Read-Eval-Print Loop for interactive development:

```bash
# Start interactive mode
./rbasic -r

# REPL supports:
# - Multi-line statements with automatic brace detection
# - Variable persistence between commands
# - Function definitions and immediate execution
# - Meta commands for file operations and help
```

**REPL Meta Commands:**
- `:help` - Show available commands
- `:load filename.bas` - Load and execute a file
- `:save filename.bas` - Save current session to file
- `:clear` - Clear all variables and functions
- `:exit` - Exit REPL mode

**Example REPL Session:**
```
rbasic> var x = 10;
rbasic> function double(n) {
...>     return n * 2;
...> }
rbasic> print(double(x));
20
rbasic> :save my_session.bas
Session saved to my_session.bas
```

### Language Featureslanguage transpiler that implements C-style syntax while maintaining BASIC's simplicity. It supports both interpretation for rapid development and compilation to native C++ executables.

## Key Features

- **C-Style Syntax**: Uses braces `{}`, parentheses for conditions, and modern control flow
- **Transpiler Architecture**: Compiles BASIC to C++ executables or runs directly via interpreter  
- **Interactive REPL**: Read-Eval-Print Loop for rapid development and testing
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

#### Foreign Function Interface (FFI)
```basic
// Direct integration with C libraries and system APIs
ffi "kernel32.dll" GetCurrentProcessId() as integer;
ffi "kernel32.dll" Sleep(milliseconds as integer) as integer;
ffi "user32.dll" MessageBoxA(hwnd as integer, text as string, caption as string, type as integer) as integer;

function main() {
    var pid = GetCurrentProcessId();
    print("Process ID:", pid);
    
    print("Sleeping for 1 second...");
    Sleep(1000);
    
    MessageBoxA(0, "Hello from rbasic!", "FFI Demo", 0);
    return 0;
}
```

### Future: Enhanced External Functionality

The FFI system provides direct access to C libraries and system APIs. Current FFI capabilities include:

**✅ Implemented FFI Features:**
- **Direct Library Integration**: Call functions from Windows DLLs, Linux shared libraries, and macOS dylibs
- **Multiple Parameter Types**: Support for integers, strings, and pointers
- **Flexible Return Types**: Functions can return integers, pointers, or void
- **Memory Management**: Safe pointer handling with type information
- **Both Execution Modes**: FFI works in both interpreted and compiled programs

**🚀 Planned FFI Extensions:**
- **Automatic Header Parsing**: Generate rbasic bindings from C header files
- **Structure Support**: Pass and return C structs by value
- **Advanced Graphics**: SDL2 integration for 2D/3D graphics programming
- **Database Connectivity**: Direct SQL database access
- **Network Programming**: Socket and HTTP client/server capabilities

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

### Data Processing Pipeline
```basic
// Advanced example: data_pipeline.bas - Real-world data processing
var sensor_data = double_array(100);
var filtered_data = double_array(100);
var binary_package = byte_array(400);

// Generate and process data
for (var i = 0; i < 100; i = i + 1) {
    sensor_data[i] = 20.0 + sin(i * 0.1) * 5.0 + (rnd() - 0.5) * 2.0;
}

// Apply smoothing filter
for (var i = 1; i < 99; i = i + 1) {
    filtered_data[i] = (sensor_data[i-1] + sensor_data[i] + sensor_data[i+1]) / 3.0;
}

// Export to text file
var report = "TEMPERATURE ANALYSIS REPORT\n";
report = report + "Data Points: 100\n";
write_text_file("analysis_report.txt", report);

// Export to binary format for external tools
for (var j = 0; j < 10; j = j + 1) {
    var temp_int = int(filtered_data[j] * 100);
    binary_package[j * 4 + 0] = (temp_int / 16777216) mod 256;
    binary_package[j * 4 + 1] = (temp_int / 65536) mod 256;
    binary_package[j * 4 + 2] = (temp_int / 256) mod 256;
    binary_package[j * 4 + 3] = temp_int mod 256;
}
write_binary_file("sensor_data.bin", binary_package);

// Verify file integrity
if (file_exists("analysis_report.txt") and file_exists("sensor_data.bin")) {
    print("Report:", file_size("analysis_report.txt"), "bytes");
    print("Binary:", file_size("sensor_data.bin"), "bytes");
}
```

### Functions and Arrays
```basic
// User-defined functions and array usage
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

// 1D arrays
dim numbers(5);
numbers[0] = 10;
numbers[1] = 20;

// 2D arrays with modern syntax
dim matrix(3, 3);
for (var i = 0; i < 3; i = i + 1) {
    for (var j = 0; j < 3; j = j + 1) {
        matrix[i, j] = i * 3 + j + 1;
    }
}

print("1D Array: ", numbers[0], numbers[1]);
print("2D Matrix[1,1] =", matrix[1, 1]);
print("5! =", factorial(5));
```

## Transpiler Architecture

rbasic uses a transpiler design that aims for identical behavior between interpreted and compiled modes:

```
BASIC Source → Lexer → Parser → AST
                                 ↓
                        Interpreter ← → Code Generator
                             ↓                ↓
                        Direct Execution   C++ Source → Executable
```

### Components
- **Lexer**: Tokenizes BASIC source with C-style syntax support
- **Parser**: Builds AST from tokens with support for modern language constructs  
- **Interpreter**: Direct AST execution for development and testing
- **Code Generator**: Transpiles AST to C++ source code
- **Runtime Library**: Shared function implementations for both execution modes

## Development Status

This is an active development project. **All core language features are implemented and fully functional:**

- ✅ **Complete Language Implementation**: Variables, expressions, assignment with proper C-style scoping
- ✅ **Control Flow**: if/else, for, while with block-scoped variable declarations  
- ✅ **Functions & Data Structures**: User-defined functions, arrays, and structures
- ✅ **Multidimensional Arrays**: True `array[i,j,k]` syntax for 2D/3D arrays in both execution modes
- ✅ **Comprehensive File I/O**: Text and binary file operations with error handling
- ✅ **Built-in Function Library**: Mathematical, string, and array manipulation functions
- ✅ **Dual Execution Modes**: Interpreter (`-i`), compiler (`-c`), and interactive REPL (`-r`) with identical behavior
- ✅ **Cross-Platform Support**: Windows MinGW64/MSVC, Linux/macOS g++ compilation
- ✅ **Advanced Features**: Typed arrays, struct literals, comprehensive data processing
- ✅ **Foreign Function Interface (FFI)**: Direct C library integration with multiple parameter types
- ✅ **Error Handling**: Source position tracking with detailed error reporting
- ✅ **Production Ready**: All unit tests passing, comprehensive example suite

**Architecture Highlights:**
- ✅ **Function Dispatch System**: Organized function handling with clean separation
- ✅ **Console-Only Core**: Clean separation for future FFI integration
- ✅ **Shared Runtime**: Identical behavior between interpreted and compiled modes
- ✅ **Robust Type System**: Variant-based ValueType with comprehensive conversions

**Example Capabilities:**
- Real-world data processing pipelines (`examples/data_pipeline.bas`)
- Mathematical computations with built-in functions
- File I/O operations for data persistence
- Structured data manipulation with arrays and structs
- User-defined functions with proper scoping

**Next Phase Development:**
- 🚀 **Enhanced Terminal Support**: Cross-platform colored console output and input
- 🚀 **Advanced FFI Features**: Automatic header parsing and structure support for SDL2/OpenGL
- 🚀 **Compiler Optimizations**: Performance improvements for compiled output
- 🚀 **Extended Standard Library**: Additional domain-specific functionality

The rbasic transpiler is **production-ready** for educational use, rapid prototyping, numerical computing, and language design experimentation.

## Project Structure

```
rbasic/
├── CMakeLists.txt         # Build configuration
├── README.md              # This documentation
├── [MANUAL.md](MANUAL.md) # User manual and reference
├── [BNF.md](BNF.md)       # Complete language grammar
├── include/               # Header files
├── src/                   # Implementation
├── runtime/               # Standalone runtime library
├── examples/              # Demonstration programs
└── tests/                 # Automated testing
```

## Build Configuration

### Standard Build (Core Language Only)
```bash
mkdir build && cd build
cmake ..
cmake --build build
```

### Platform Notes
- **Windows**: Uses bundled MinGW64 (portable) or falls back to MSVC, use `build.bat` for convenience builds
- **Linux**: Use `./build.sh` for convenience builds
- **macOS**: Standard CMake build process

### Portable Deployment
- **MinGW64**: Self-contained executables with static linking (Windows)
- **No Dependencies**: Core language requires no external libraries
- **Bundled Compiler**: MinGW64 included for systems without Visual Studio

### Convenience Build Scripts
```bash
# Windows (Release build)
build.bat

# Linux (with options)
./build.sh                    # Release build
./build.sh debug             # Debug build
./build.sh clean             # Clean build
./build.sh --jobs 8          # Parallel build with 8 threads
./build.sh --help            # Show all options
```

### Future: Foreign Function Interface (FFI)
The FFI system provides seamless integration with external C libraries, enabling rbasic programs to access system APIs, graphics libraries, databases, and more while maintaining the same clean syntax in both interpreted and compiled modes.

## Performance and Use Cases

### Compilation Benefits
- **Development Speed**: Interpreted mode for rapid iteration
- **Native Performance**: Compiled mode generates optimized C++ for production
- **Automatic Parallelization**: OpenMP-based multi-core optimization for large arrays (≥1000 elements)
- **Standalone Executables**: No runtime dependencies for core language
- **Readable Output**: Generated C++ code is human-readable

### Performance Features
- **Multi-Core Array Operations**: Automatic parallelization for large array processing
- **Smart Thresholding**: OpenMP applied only when beneficial (arrays ≥1000 elements)
- **Cross-Platform Optimization**: OpenMP support on Windows (MinGW64/MSVC), Linux, and macOS
- **Zero-Overhead Abstraction**: Small arrays use serial execution to avoid threading overhead

### Intended Use Cases
- **Learning**: Explore language implementation and transpiler design
- **Prototyping**: Rapid development with C-style syntax
- **Educational**: Teaching programming concepts with familiar syntax
- **Experimentation**: Test ideas before implementing in other languages

## Contributing and Learning

rbasic is designed for developers interested in:
- **Language Implementation**: Study transpiler architecture
- **Rapid Prototyping**: BASIC simplicity with modern syntax
- **Educational Use**: Clear, understandable codebase
- **Cross-Platform Development**: Single source for multiple targets

The project welcomes contributions, bug reports, and suggestions for improving the core language implementation.

## Documentation

- **[User Manual](MANUAL.md)** - Complete guide to using rbasic
- **[Language Grammar (BNF)](BNF.md)** - Complete syntax specification and built-in functions
- **[Examples](examples/)** - Sample programs demonstrating language features
- **[VS Code Extension](vscode-extension/)** - Syntax highlighting and editor support

## License

MIT License - See LICENSE file for details.

