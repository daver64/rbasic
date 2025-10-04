# rbasic Project Instructions

This is a BASIC interpreter and compiler written in C++. The project supports both interpreting BASIC programs directly and compiling them to native C++ executables.

## Project Structure
- `src/` - Main source code (lexer, parser, interpreter, code generator)
- `include/` - Header files
- `runtime/` - Runtime library for compiled programs
- `examples/` - Sample BASIC programs
- `tests/` - Unit tests
- `build/` - Build directory (created by CMake)

## Key Components
1. **Lexer** - Tokenizes BASIC source code
2. **Parser** - Builds Abstract Syntax Tree (AST)
3. **Interpreter** - Directly executes AST nodes
4. **Code Generator** - Emits C++ code from AST
5. **Runtime Library** - Shared functions for both execution modes

## Build Instructions
Use CMake to build the project. The executable will be placed in the project root.

## Usage
- Interpret: `rbasic -i program.bas`
- Compile: `rbasic -c program.bas -o program`

## Syntax Features
- Modern variable declarations: `var name = value;`
- Required semicolons for statement termination
- Lowercase keywords (var, if, then, else, for, while, function, etc.)
- Typed parameters and return values
- Structures and arrays support

The project is designed so that both interpreter and compiler produce identical results by sharing the same AST representation and runtime functions.