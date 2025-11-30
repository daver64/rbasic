# rbasic - Modern BASIC for Raspberry Pi & Embedded Systems

**rbasic** is a modern BASIC transpiler with C-style syntax, designed for Raspberry Pi and embedded development. Combines BASIC's simplicity with native hardware access, optional graphics/database support, and the ability to compile to optimized native executables.

🎯 **Primary Target**: Raspberry Pi (Zero 2W, 3/4/5) and ARM single-board computers

⚠️ **Alpha Software**: Early development - features may be incomplete or subject to change.

---

## Key Features

- **🔌 Native Hardware**: Direct GPIO, SPI, I2C, PWM, UART access on Raspberry Pi (no root required)
- **⚡ Dual Execution**: Interpret for rapid prototyping (`-i`) or compile to native executables (`-c`)
- **🎮 Interactive REPL**: Real-time hardware testing and algorithm development
- **🎨 Optional SDL2**: Graphics, windowing, events (build with `-DWITH_SDL2=ON`)
- **💾 Optional SQLite3**: Embedded database support (build with `-DWITH_SQLITE3=ON`)
- **📐 GLM Vector Math**: Native `vec2`, `vec3`, `vec4` types with SIMD optimization
- **🚀 Performance**: Near-C++ speeds with OpenMP parallelization
- **🔄 Cross-Compilation**: Build on x86/x64 for ARM targets

---

## Quick Start

### Build
```bash
git clone https://github.com/daver64/rbasic.git
cd rbasic && mkdir build && cd build

# Core build (interpreter + compiler)
cmake .. && make -j$(nproc)

# With optional libraries
cmake -DWITH_SDL2=ON -DWITH_SQLITE3=ON .. && make -j$(nproc)

# Cross-compile for Raspberry Pi
cmake -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake .. && make -j$(nproc)
```

**Prerequisites for optional features:**
```bash
# Ubuntu/Debian/Raspberry Pi OS
sudo apt install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsqlite3-dev

# macOS
brew install sdl2 sdl2_gfx sdl2_image sqlite3
```

### Usage
```bash
# Interpret (fast development)
./rbasic -i program.bas

# Compile to executable (deployment)
./rbasic -c program.bas -o program
./program

# Interactive REPL
./rbasic -r
```

---

## Language Example

```basic
// Modern C-style BASIC with hardware access
import "sensors.bas";

function read_temperature() {
    var i2c = i2c_open(1);
    i2c_set_address(i2c, 0x76);
    var temp = i2c_read_reg(i2c, 0xFA);
    i2c_close(i2c);
    return temp / 100.0;
}

// GPIO LED control
gpio_init();
gpio_set_mode(17, 1);  // OUTPUT

for (var i = 0; i < 10; i = i + 1) {
    gpio_write(17, 1);
    print("Temp: " + str(read_temperature()) + "°C");
    sleep(1000);
    gpio_write(17, 0);
    sleep(1000);
}

gpio_cleanup();
```

---

## Core Language Features

- **C-Style Syntax**: `if (x > 5) { }`, `for (i=0; i<10; i++) { }`
- **Modern I/O**: `print("text")`, `var input = input()`
- **Variables**: `var x = 42;` with proper block scoping
- **Functions**: User-defined with parameters and return values
- **Arrays**: `dim arr(100)` with `arr[i]` indexing
- **Import System**: Modular code with `import "file.bas"`
- **GLM Math**: `var pos = vec3(1, 2, 3); pos.x = 5.0;`
- **File I/O**: `write_text_file()`, `read_binary_file()`

📖 **Full Documentation**: [MANUAL.md](MANUAL.md) | [BNF.md](BNF.md)

---

## Hardware Support (Raspberry Pi)

| Feature | Functions | Description |
|---------|-----------|-------------|
| **GPIO** | `gpio_init()`, `gpio_write()`, `gpio_read()` | Direct pin control via /dev/gpiomem |
| **SPI** | `spi_open()`, `spi_transfer()` | High-speed serial for ADCs, sensors |
| **I2C** | `i2c_open()`, `i2c_write_reg()` | Two-wire bus for sensors (BME280, etc.) |
| **PWM** | `pwm_init()`, `pwm_set_duty_cycle()` | Hardware PWM for servos, LEDs |
| **Serial** | `serial_open()`, `serial_write_string()` | UART for GPS, Arduino communication |

**Auto-detected on ARM platforms** - Functions only available when building on/for Raspberry Pi.

📘 **Hardware Guide**: [RASPBERRY_PI.md](RASPBERRY_PI.md)

---

## Optional Libraries

### SDL2 Graphics (`-DWITH_SDL2=ON`)
- Window management and hardware-accelerated rendering
- Keyboard/mouse event handling with built-in constants
- SDL2_gfx filled shapes (circles, triangles, ellipses)
- SDL2_image texture loading (PNG, JPG, BMP)
- 40+ functions, identical behavior in interpreter and compiled modes

### SQLite3 Database (`-DWITH_SQLITE3=ON`)
- Full embedded database with prepared statements
- Transaction support (commit/rollback)
- Type-safe parameter binding
- 25+ functions with built-in constants (SQLITE_OK, SQLITE_ROW, etc.)

---

## Examples

### Raspberry Pi
- `examples/rpi_gpio_blink.bas` - LED control
- `examples/rpi_button_led.bas` - Interactive GPIO input
- `examples/rpi_pwm_servo.bas` - Servo motor control (0-180°)
- `examples/rpi_spi_sensor.bas` - MCP3008 ADC communication
- `examples/rpi_i2c_bme280.bas` - Temperature sensor reading

### SDL2 Graphics
- `examples/sdl_simple_window.bas` - Window with rendering
- Requires `-DWITH_SDL2=ON` build flag

### SQLite3 Database
- `examples/sqlite_demo.bas` - CRUD operations, transactions
- Requires `-DWITH_SQLITE3=ON` build flag

### General
- `examples/fibonacci.bas` - Recursive functions
- `examples/glm_vector_math.bas` - 3D vector operations
- `examples/arrays.bas` - Array manipulation

---

## Recent Changes (November 2025)

- ✅ **SDL2 Integration**: Complete graphics support with 40+ functions, event handling, SDL2_gfx/SDL2_image
- ✅ **SQLite3 Integration**: Full database support with 25+ functions, prepared statements, transactions
- ✅ **Constant System**: 50+ SDL2 and 30+ SQLite3 constants accessible by name
- ✅ **Cross-Compilation**: Library linking configured for native and ARM cross-compilation
- ✅ **Raspberry Pi Hardware**: GPIO, SPI, I2C, PWM, Serial with conditional compilation

---

## Architecture

rbasic transpiles BASIC to C++ for compilation, or directly interprets via AST. Both modes share the same runtime, ensuring identical behavior:

1. **Lexer** → Tokens with C-style syntax support
2. **Parser** → Abstract Syntax Tree (AST) with import resolution
3. **Interpreter** → Direct AST execution (rapid prototyping)
4. **Code Generator** → C++ source generation (deployment)
5. **Runtime Library** → Shared functions for both modes with OpenMP parallelization

---

## Status

**Working**: Core language, GPIO/SPI/I2C/PWM, SDL2, SQLite3, GLM math, REPL, imports, cross-compilation

**Known Limitations**:
- Error messages need line numbers and better context
- Documentation still expanding
- Some edge cases in complex scoping scenarios

---

## Project Structure

```
rbasic/
├── src/           # Transpiler (lexer, parser, interpreter, codegen)
├── include/       # Headers
├── runtime/       # Runtime library and optional wrappers (SDL2, SQLite3)
├── examples/      # Sample programs
├── tests/         # Unit and integration tests
├── build/         # Build output (created by CMake)
└── mingw64/       # Bundled MinGW64 for Windows
```

---

## License

MIT License - Copyright (c) 2025 David Rowbotham

---

**rbasic** - Modern BASIC for Raspberry Pi. Hardware access, graphics, databases, and native compilation in one lightweight package.
