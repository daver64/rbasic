# rbasic Project Instructions

This is a C-leaning BASIC language transpiler written in C++. The project supports both interpreting BASIC programs directly and transpiling them to native C++ executables with identical behavior.

## Project Structure
- `src/` - Main source code (lexer, parser, interpreter, code generator)
- `include/` - Header files
- `runtime/` - Runtime library for compiled programs
- `examples/` - Sample BASIC programs demonstrating language features
- `tests/` - Unit tests
- `build/` - Build directory (created by CMake)

## Key Components
1. **Lexer** - Tokenizes BASIC source code with C-style syntax support
2. **Parser** - Builds Abstract Syntax Tree (AST) from tokens
3. **Interpreter** - Directly executes AST nodes for rapid development
4. **Code Generator** - Transpiles AST to optimized C++ source code
5. **Runtime Library** - Shared function implementations for both execution modes

## Build Instructions
Use CMake to build the project. The executable will be placed in the project root.

## Usage
- Interpret: `rbasic -i program.bas` (rapid prototyping)
- Transpile: `rbasic -c program.bas -o program` (production deployment)

## Language Features
rbasic implements a "C-leaning BASIC" with modern syntax:
- C-style control structures: `if (condition) { }`, `for (init; test; inc) { }`
- Function-call I/O: `print("Hello")`, `var input = input()`
- Modern comments: `//` line comments and `/* */` block comments
- Assignment expressions: `var y = (x = x + 1) * 2`
- Structured data: Arrays with `dim array(size)` and `array[index]` access
- Future external features: Graphics, databases, and system integration via FFI

## Syntax Features
- Modern variable declarations: `var name = value;`
- Required semicolons for statement termination
- Lowercase keywords (var, if, then, else, for, while, function, etc.)
- Typed parameters and return values in functions
- C++ style braces for code blocks
- Structures and arrays support

## Future: Foreign Function Interface

The project is designed as a transpiler where both interpreter and compiled output produce identical results by sharing the same AST representation and runtime function implementations. This enables rapid prototyping in interpreted mode and high-performance deployment via compilation.

External functionality like graphics, databases, and system integration will be provided through a Foreign Function Interface (FFI) system, allowing clean separation of core language features from external libraries.