# rbasic Project Instructions

**⚠️ ALPHA SOFTWARE**: This project is in active Alpha development. While core features are functional, expect bugs, breaking changes, and incomplete functionality. Not recommended for production use.

This is a C-leaning BASIC language transpiler written in C++. The project supports interpreting BASIC programs directly, transpiling them to native C++ executables, and interactive development via REPL, all with identical behaviour.

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
5. **Code Generator** - Transpiles AST to optimised C++ source code
6. **Runtime Library** - Shared function implementations for both execution modes with OpenMP parallelisation
7. **Cross-Platform Terminal** - Colour support and cursor management for all platforms
8. **Automatic Parallelisation** - OpenMP-based multi-core optimisation for large array operations
9. **Import System** - Complete modular programming with compile-time and runtime import resolution
10. **Raspberry Pi Support** - Native GPIO, SPI, I2C, PWM, and Serial hardware access on ARM platforms

## Build Instructions
Use CMake to build the project. The executable will be placed in the project root.

## Usage
- Interpret: `rbasic -i program.bas` (rapid prototyping with runtime imports)
- Transpile: `rbasic -c program.bas -o program` (deployment with resolved imports)
- Interactive: `rbasic -r` (REPL mode for development and testing)

## Compilation Support
- **Windows**: Bundled MinGW64 (portable) with MSVC fallback
- **Linux/macOS**: System GCC or Clang
- **Raspberry Pi**: ARM64/aarch64 support with hardware access
- **Cross-Compilation**: pi-toolchain.cmake for building on x86 for ARM
- **Automatic Detection**: Chooses best available compiler
- **Static Linking**: Self-contained executables with MinGW64
- **OpenMP Parallelisation**: Automatic multi-core optimisation for both interpreted and compiled modes
- **Conditional Compilation**: RPI hardware support only compiled on ARM platforms

## Performance Features
- **Automatic Parallelisation**: OpenMP-based array operations for arrays ≥1000 elements
- **Invisible to Users**: Parallelisation is completely transparent and automatic
- **Cross-Platform**: OpenMP support on Windows (MinGW64/MSVC), Linux (GCC), and macOS (Clang)
- **Smart Thresholding**: Serial execution for small arrays to avoid threading overhead
- **Both Modes**: Parallelisation works in interpreter mode and compiled executables

## Language Features
rbasic implements a "C-leaning BASIC" with modern syntax:
- C-style control structures: `if (condition) { }`, `for (init; test; inc) { }`
- Function-call I/O: `print("Hello")`, `var input = input()`
- Modern comments: `//` line comments and `/* */` block comments
- Assignment expressions: `var y = (x = x + 1) * 2`
- Structured data: Arrays with `dim array(size)` and `array[index]` access
- GLM vector math: Native `vec2`, `vec3`, `vec4` types with component access and mathematical operations
- Interactive development: Full REPL with session management
- Modular programming: Complete import system with `import "file.bas"` syntax

## Import System Features
- **Modular Programming**: `import "filename.bas"` syntax for code organization
- **Cross-Module Access**: Functions and variables accessible across imported files
- **Path Resolution**: Multi-directory search (current, executable, lib/, stdlib/)
- **Duplicate Prevention**: Each file imported only once per program
- **Circular Detection**: Prevents infinite import loops with clear error messages
- **Compile-Time Resolution**: Imports resolved and inlined during compilation (C-style preprocessing)
- **Runtime Resolution**: Dynamic import loading in interpreter mode
- **Identical Behaviour**: Same output in both interpreter and compile modes

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


### SDL2 Integration
- **Window Management**: Window creation, destruction, and property control
- **Rendering System**: Hardware-accelerated 2D rendering with colour management
- **Filled Shapes**: Proper solid triangle rendering with SDL2_gfx `filledTrigonRGBA`
- **Event Handling**: Complete keyboard and mouse event processing
  - **Keyboard Events**: Full scan code support with built-in SDL_SCANCODE_* constants
  - **Mouse Events**: Click detection, movement tracking, button state management
  - **Window Events**: Focus, resize, and close event handling
- **Memory-Safe Event Access**: Direct event buffer reading with `deref_int_offset()`
- **Cross-Mode Compatibility**: Identical event handling in interpreter and compiled modes

### Library Integration Ready
- **SDL2**: Complete structure support, event handling, graphics primitives, input management
- **SDL2_gfx**: Filled polygon and triangle rendering with up to 11-parameter function support
- **SQLite**: Database constants, prepared statement patterns
- **Windows API**: System calls, message boxes, process management
- **OpenGL**: Ready for graphics programming integration

## Recent Improvements (2025)

### Complete GLM Vector Mathematics Integration (October 2025)
- **Native GLM Types**: Built-in `vec2`, `vec3`, `vec4`, `mat3`, `mat4`, `quat` primitives with SIMD optimisation
- **Component Access**: Full read/write access with `.x`, `.y`, `.z`, `.w` component syntax using ComponentAssignExpr AST node
- **Vector Operations**: Native support for addition (`+`), subtraction (`-`), scalar multiplication (`*`) with GLM backing
- **GLM Functions**: `length()`, `normalize()`, `dot()`, `cross()` functions with mathematical accuracy
- **Dual-Mode Support**: Identical GLM behaviour in interpreter and compiled modes with native performance
- **Architecture**: Extended BasicValue variant with GLM wrapper types, runtime library GLM integration, proper AST nodes

### Variable Scoping Fix (October 2025)
- **For Loop Scoping**: Fixed critical bug where for loop variables couldn't access parent scope
- **Function Compatibility**: Resolved scoping issues in functions with typed parameters  
- **Cross-Mode Consistency**: Identical scoping behaviour in interpreter and compiled modes
- **Root Cause**: Modified `ModernForStmt::visit()` to use variable backup/restore instead of isolated scopes


### SDL2 Integration
- **Window Management**: Window creation, destruction, and property control
- **Rendering System**: Hardware-accelerated 2D rendering with colour management
- **Texture Rendering**: Complete texture support with proper SDL_RenderCopy null parameter handling
- **Filled Shapes**: Proper solid triangle rendering with SDL2_gfx `filledTrigonRGBA`
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

## Raspberry Pi Hardware Support

### Overview
Native hardware access for Raspberry Pi devices with conditional compilation:
- **GPIO**: Direct /dev/gpiomem register access (no root required)
- **SPI**: High-speed serial peripheral interface for sensors/displays
- **I2C**: Two-wire interface for sensor communication
- **PWM**: Hardware PWM for servos, LED dimming, motor control
- **Serial (UART)**: Asynchronous serial communication

### Platform Detection
CMake automatically detects ARM architecture and enables RPI_SUPPORT_ENABLED:
```cmake
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm.*|ARM.*|aarch64.*|AARCH64.*")
    add_compile_definitions(RPI_SUPPORT_ENABLED)
endif()
```

### Cross-Compilation
Use pi-toolchain.cmake for building on x86 for Raspberry Pi:
```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake ..
make
```

### Usage Examples
```basic
// GPIO LED Blink
gpio_init();
gpio_set_mode(17, 1); // OUTPUT
gpio_write(17, 1);    // ON
sleep(500);
gpio_write(17, 0);    // OFF
gpio_cleanup();

// I2C Sensor
var i2c = i2c_open(1);
i2c_set_address(i2c, 0x76);
var chip_id = i2c_read_reg(i2c, 0xD0);
i2c_close(i2c);

// PWM Servo Control
pwm_init(0);
pwm_set_frequency(0, 50);
pwm_set_duty_cycle(0, 7.5); // Center position
pwm_enable(0);

// Serial Communication
var serial = serial_open("/dev/serial0");
serial_set_baud(serial, 115200);
serial_write_string(serial, "Hello!\n");
serial_close(serial);
```

## Architecture Philosophy

The project is designed as a transpiler where interpreter, REPL, and compiled output produce identical results by sharing the same AST representation and runtime function implementations. This enables:

1. **Rapid Prototyping**: Use interpreter/REPL mode for development
2. **Deployment**: Compile to optimised C++ executables
3. **Modular Development**: Import system enables standard library ecosystem
5. **Cross-Platform Consistency**: Same behaviour on Windows, Linux, and macOS

The architecture supports building a comprehensive standard library ecosystem where modules can be developed, tested, and deployed across all execution modes with identical behaviour.