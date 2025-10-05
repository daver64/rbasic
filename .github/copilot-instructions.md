# rbasic Project Instructions

This is a C-leaning BASIC language transpiler written in C++. The project supports interpreting BASIC programs directly, transpiling them to native C++ executables, and interactive development via REPL, all with identical behavior.

## Project Structure
- `src/` - Main source code (lexer, parser, interpreter, code generator, REPL)
- `include/` - Header files
- `runtime/` - Runtime library for compiled programs
- `examples/` - Sample BASIC programs demonstrating language features
- `tests/` - Unit tests
- `build/` - Build directory (created by CMake)
- `mingw64/` - Bundled MinGW64 compiler for Windows

## Key Components
1. **Lexer** - Tokenizes BASIC source code with C-style syntax support
2. **Parser** - Builds Abstract Syntax Tree (AST) from tokens
3. **Interpreter** - Directly executes AST nodes for rapid development
4. **REPL** - Interactive Read-Eval-Print Loop with multi-line support and meta commands
5. **Code Generator** - Transpiles AST to optimized C++ source code
6. **Runtime Library** - Shared function implementations for both execution modes with OpenMP parallelization
7. **Cross-Platform Terminal** - Color support and cursor management for all platforms
8. **Automatic Parallelization** - OpenMP-based multi-core optimization for large array operations

## Build Instructions
Use CMake to build the project. The executable will be placed in the project root.

## Usage
- Interpret: `rbasic -i program.bas` (rapid prototyping)
- Transpile: `rbasic -c program.bas -o program` (production deployment)
- Interactive: `rbasic -r` (REPL mode for development and testing)

## Compilation Support
- **Windows**: Bundled MinGW64 (portable) with MSVC fallback
- **Linux/macOS**: System GCC or Clang
- **Automatic Detection**: Chooses best available compiler
- **Static Linking**: Self-contained executables with MinGW64
- **OpenMP Parallelization**: Automatic multi-core optimization for both interpreted and compiled modes

## Performance Features
- **Automatic Parallelization**: OpenMP-based array operations for arrays ≥1000 elements
- **Invisible to Users**: Parallelization is completely transparent and automatic
- **Cross-Platform**: OpenMP support on Windows (MinGW64/MSVC), Linux (GCC), and macOS (Clang)
- **Smart Thresholding**: Serial execution for small arrays to avoid threading overhead
- **Both Modes**: Parallelization works in interpreter mode and compiled executables

## Language Features
rbasic implements a "C-leaning BASIC" with modern syntax:
- C-style control structures: `if (condition) { }`, `for (init; test; inc) { }`
- Function-call I/O: `print("Hello")`, `var input = input()`
- Modern comments: `//` line comments and `/* */` block comments
- Assignment expressions: `var y = (x = x + 1) * 2`
- Structured data: Arrays with `dim array(size)` and `array[index]` access
- Interactive development: Full REPL with session management
- Future external features: Graphics, databases, and system integration via FFI

## Syntax Features
- Modern variable declarations: `var name = value;`
- Required semicolons for statement termination
- Lowercase keywords (var, if, then, else, for, while, function, etc.)
- Typed parameters and return values in functions
- C++ style braces for code blocks
- Structures and arrays support

## REPL Features
- Multi-line statement detection with automatic brace counting
- Variable and function persistence between commands
- Meta commands: `:help`, `:load`, `:save`, `:clear`, `:exit`
- Session management for saving and loading work
- Immediate execution and feedback

## Future: Foreign Function Interface

The project is designed as a transpiler where interpreter, REPL, and compiled output produce identical results by sharing the same AST representation and runtime function implementations. This enables rapid prototyping in interpreted/REPL mode and high-performance deployment via compilation.

External functionality like graphics, databases, and system integration will be provided through a Foreign Function Interface (FFI) system, allowing clean separation of core language features from external libraries.