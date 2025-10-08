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
2. **Parser** - Builds Abstract Syntax Tree (AST) from tokens with import statement support
3. **Interpreter** - Directly executes AST nodes for rapid development with runtime import resolution
4. **REPL** - Interactive Read-Eval-Print Loop with multi-line support and meta commands
5. **Code Generator** - Transpiles AST to optimized C++ source code
6. **Runtime Library** - Shared function implementations for both execution modes with OpenMP parallelization
7. **Cross-Platform Terminal** - Color support and cursor management for all platforms
8. **Automatic Parallelization** - OpenMP-based multi-core optimization for large array operations
9. **Import System** - Complete modular programming with compile-time and runtime import resolution
10. **Foreign Function Interface (FFI)** - Comprehensive C library integration with extensive API support

## Build Instructions
Use CMake to build the project. The executable will be placed in the project root.

## Usage
- Interpret: `rbasic -i program.bas` (rapid prototyping with runtime imports)
- Transpile: `rbasic -c program.bas -o program` (deployment with resolved imports)
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
- Modular programming: Complete import system with `import "file.bas"` syntax
- External integration: Comprehensive FFI system for C libraries

## Import System Features
- **Modular Programming**: `import "filename.bas"` syntax for code organization
- **Cross-Module Access**: Functions and variables accessible across imported files
- **Path Resolution**: Multi-directory search (current, executable, lib/, stdlib/)
- **Duplicate Prevention**: Each file imported only once per program
- **Circular Detection**: Prevents infinite import loops with clear error messages
- **Compile-Time Resolution**: Imports resolved and inlined during compilation (C-style preprocessing)
- **Runtime Resolution**: Dynamic import loading in interpreter mode
- **Identical Behavior**: Same output in both interpreter and compile modes

## Syntax Features
- Modern variable declarations: `var name = value;`
- Required semicolons for statement termination
- Lowercase keywords (var, if, then, else, for, while, function, import, etc.)
- Typed parameters and return values in functions
- C++ style braces for code blocks
- Structures and arrays support
- Import statements: `import "library.bas";`

## REPL Features
- Multi-line statement detection with automatic brace counting
- Variable and function persistence between commands
- Meta commands: `:help`, `:load`, `:save`, `:clear`, `:exit`
- Session management for saving and loading work
- Immediate execution and feedback
- Import support in interactive mode

## Foreign Function Interface (FFI)

The FFI system provides comprehensive C library integration:

### Core FFI Features
- **Library Loading**: Dynamic loading of Windows DLLs, Linux .so, and macOS .dylib files
- **Function Calling**: Support for 0-8 parameter functions with flexible types
- **Memory Management**: Safe buffer allocation, pointer dereferencing, and cleanup
- **Type System**: Comprehensive support for integers, strings, pointers, and structures

### Advanced FFI Features
- **Buffer Operations**: `alloc_buffer()`, `alloc_int_buffer()`, `alloc_pointer_buffer()`
- **Pointer Operations**: `deref_int()`, `deref_pointer()`, `deref_string()`, `deref_int_offset()`
- **Structure Support**: SDL2 structures (`create_sdl_rect()`, `create_sdl_event()`)
- **Constant System**: Built-in constants for SDL2, SQLite, Windows API
- **Null Handling**: `is_null()`, `not_null()` for safe pointer operations
- **Event Processing**: Complete SDL2 event handling with `get_key_code()` for keyboard events

### SDL2 Integration
- **Window Management**: Window creation, destruction, and property control
- **Rendering System**: Hardware-accelerated 2D rendering with color management
- **Event Handling**: Complete keyboard and mouse event processing
  - **Keyboard Events**: Full scan code support with built-in SDL_SCANCODE_* constants
  - **Mouse Events**: Click detection, movement tracking, button state management
  - **Window Events**: Focus, resize, and close event handling
- **Memory-Safe Event Access**: Direct event buffer reading with `deref_int_offset()`
- **Cross-Mode Compatibility**: Identical event handling in interpreter and compiled modes

### Library Integration Ready
- **SDL2**: Complete structure support, event handling, graphics primitives, input management
- **SQLite**: Database constants, prepared statement patterns
- **Windows API**: System calls, message boxes, process management
- **OpenGL**: Ready for graphics programming integration

### Usage Examples
```basic
// SDL2 Graphics with Event Handling
ffi integer SDL_Init(flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateWindow(title as string, x as integer, y as integer, w as integer, h as integer, flags as integer) from "SDL2.dll";
ffi integer SDL_PollEvent(event as pointer) from "SDL2.dll";

var window = SDL_CreateWindow("Interactive Demo", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
var event_buffer = create_sdl_event();
while (running == 1) {
    if (SDL_PollEvent(event_buffer) == 1) {
        var event_type = deref_int(event_buffer);
        if (event_type == SDL_MOUSEBUTTONDOWN) {
            print("Mouse clicked!");
        } else if (event_type == SDL_KEYDOWN) {
            var key = get_key_code(event_buffer);
            if (key == SDL_SCANCODE_ESCAPE) {
                running = 0;
            }
        }
    }
}

// SQLite Database
ffi "sqlite3.dll" sqlite3_open(filename as string, db as pointer) as integer;
var db_ptr = alloc_pointer_buffer();
var result = sqlite3_open("data.db", db_ptr);

// Windows API
ffi "user32.dll" MessageBoxA(hwnd as integer, text as string, caption as string, type as integer) as integer;
MessageBoxA(0, "Hello from rbasic!", "FFI Demo", MB_OK);
```

## Architecture Philosophy

The project is designed as a transpiler where interpreter, REPL, and compiled output produce identical results by sharing the same AST representation and runtime function implementations. This enables:

1. **Rapid Prototyping**: Use interpreter/REPL mode for development
2. **Deployment**: Compile to optimized C++ executables
3. **Modular Development**: Import system enables standard library ecosystem
4. **External Integration**: FFI provides seamless C library access
5. **Cross-Platform Consistency**: Same behavior on Windows, Linux, and macOS

The architecture supports building a comprehensive standard library ecosystem where modules can be developed, tested, and deployed across all execution modes with identical behavior.