# rbasic - C-Leaning BASIC Language Transpiler

**rbasic** is a modern BASIC language transpiler that implements C-style syntax while maintaining BASIC's simplicity. It supports both interpretation for rapid development and compilation to native C++ executables.

## Key Features

- **C-Style Syntax**: Uses braces `{}`, parentheses for conditions, and modern control flow
- **Transpiler Architecture**: Compiles BASIC to C++ executables or runs directly via interpreter
- **Core Language Features**: Variables, functions, arrays, control structures, and built-in functions
- **Dual Execution**: Designed for identical behavior in both interpreted and compiled modes
- **Zero Dependencies**: Core language has no external dependencies
- **Cross-Platform**: Supports Windows, Linux, and macOS with native compilers

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
- C++17 compiler (GCC, Clang, MSVC)

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

## Language Features

### Core Syntax Elements

- **C-Style Control Flow**: `if (condition) { }`, `for (init; test; inc) { }`
- **Function-Call I/O**: `print("Hello")`, `var input = input()`
- **Assignment Expressions**: `var y = (x = x + 1) * 2`
- **Modern Comments**: `//` line comments and `/* */` block comments
- **Arrays**: `dim array(size)` declaration with `array[index]` access

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

### Future: External Functionality via FFI

External features like graphics, databases, and system integration will be provided through a Foreign Function Interface (FFI) system, allowing clean separation of core language features from external libraries.

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

dim numbers(5);
numbers[0] = 10;
numbers[1] = 20;

print("Array: ", numbers[0], numbers[1]);
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

This is an active development project. Core language features are implemented and working:

- ✅ Variables, expressions, and assignment
- ✅ Control flow (if/else, for, while)
- ✅ Functions and arrays
- ✅ Built-in mathematical and string functions
- ✅ Basic I/O operations
- ✅ Interpreter and compiler modes
- ✅ Cross-platform compilation (Windows MSVC, Linux/macOS g++)

**Future Development:**
- 🚀 Foreign Function Interface (FFI) for external library integration
- 🚀 Selectable compiler backends
- 🚀 Enhanced optimization features

The transpiler is suitable for educational use, prototyping, and experimenting with language design.

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
- **Windows**: Uses MSVC compiler, use `build.bat` for convenience builds
- **Linux**: Use `./build.sh` for convenience builds
- **macOS**: Standard CMake build process

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
External functionality like graphics, databases, and system integration will be provided through a Foreign Function Interface system, similar to Lisp's approach. This will allow clean separation of core language features from external libraries.

## Performance and Use Cases

### Compilation Benefits
- **Development Speed**: Interpreted mode for rapid iteration
- **Native Performance**: Compiled mode generates C++ for production
- **Standalone Executables**: No runtime dependencies for core language
- **Readable Output**: Generated C++ code is human-readable

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

