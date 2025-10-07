# rbasic User Manual

This manual provides comprehensive documentation for using the rbasic programming language, including all keywords, built-in functions, and tools.

## Implementation Status

**rbasic is feature-complete and production-ready** with all core language features fully implemented and tested:

- ✅ **Complete Language**: All syntax, control flow, functions, and data structures working
- ✅ **Multidimensional Arrays**: True `array[i,j,k]` syntax working in both execution modes
- ✅ **Import System**: Complete modular programming with `import "file.bas"` syntax
- ✅ **Foreign Function Interface (FFI)**: Production-ready C library integration with comprehensive pointer and struct support
- ✅ **Automatic Parallelization**: OpenMP-based multi-core optimization for large array operations
- ✅ **Triple Execution Modes**: Interpreter (`-i`), compiler (`-c`), and interactive REPL (`-r`) fully functional
- ✅ **Portable Compilation**: MinGW64 bundled for Windows, falls back to MSVC when needed
- ✅ **File I/O**: Complete text and binary file operations
- ✅ **Built-in Functions**: Math, string, array, and file functions all implemented
- ✅ **Error Handling**: Source position tracking with detailed error messages
- ✅ **Cross-Platform**: Works on Windows (MinGW64/MSVC) and Linux/macOS (g++)
- ✅ **All Tests Passing**: Comprehensive test suite validates functionality

## Table of Contents

- [Getting Started](#getting-started)
- [Command Line Usage](#command-line-usage)
- [Interactive REPL Mode](#interactive-repl-mode)
- [Language Syntax](#language-syntax)
- [Variable Scope](#variable-scope)
- [Keywords Reference](#keywords-reference)
- [Built-in Functions](#built-in-functions)
- [Data Types](#data-types)
- [Control Flow](#control-flow)
- [Functions and Procedures](#functions-and-procedures)
- [Arrays and Structures](#arrays-and-structures)
- [Import System](#import-system)
- [Foreign Function Interface (FFI)](#foreign-function-interface-ffi)
- [Examples](#examples)
- [Error Handling](#error-handling)
- [Performance and Optimization](#performance-and-optimization)
- [Best Practices](#best-practices)

## Getting Started

### Installation

1. **Build rbasic** from source:
   ```bash
   git clone https://github.com/daver64/rbasic.git
   cd rbasic
   mkdir build && cd build
   cmake .. && cmake --build .
   ```

2. **Verify installation**:
   ```bash
   ./rbasic --help
   ```

### Your First Program

Create a file called `hello.bas`:

```basic
// hello.bas - Your first rbasic program
print("Hello, World!");
var name = input("What's your name? ");
print("Nice to meet you,", name + "!");
```

Run it:
```bash
# Interpret directly
./rbasic -i hello.bas

# Or compile to executable
./rbasic -c hello.bas -o hello
./hello
```

## Command Line Usage

### Basic Syntax

```bash
rbasic [options] <source-file>
```

### Options

| Option | Description | Example |
|--------|-------------|---------|
| `-i, --interpret` | Run program in interpreter mode | `rbasic -i program.bas` |
| `-c, --compile` | Compile program to executable | `rbasic -c program.bas` |
| `-r, --repl` | Start interactive REPL mode | `rbasic -r` |
| `-o, --output <file>` | Specify output executable name | `rbasic -c program.bas -o myprogram` |
| `--io <type>` | Set I/O handler (console) | `rbasic -i program.bas --io console` |
| `--keep-cpp` | Keep generated C++ file | `rbasic -c program.bas --keep-cpp` |
| `-h, --help` | Show help message | `rbasic --help` |

### Usage Examples

```bash
# Interpret a program (fast development)
rbasic -i calculator.bas

# Compile to executable (production - uses MinGW64 or MSVC)
rbasic -c calculator.bas -o calc
./calc

# Start interactive REPL mode
rbasic -r

# Compile and keep the generated C++ code
rbasic -c program.bas -o program --keep-cpp
```

### Compilation Notes

**Windows:**
- **Primary**: Uses bundled MinGW64 compiler (portable, no Visual Studio required)
- **Fallback**: Uses Microsoft Visual C++ if MinGW64 not available
- **Static Linking**: MinGW64 produces self-contained executables

**Linux/macOS:**
- Uses system GCC or Clang compiler

## Interactive REPL Mode

The REPL (Read-Eval-Print Loop) provides an interactive environment for rapid development and testing:

### Starting REPL

```bash
rbasic -r
```

### REPL Features

- **Multi-line Support**: Automatic detection of incomplete statements
- **Variable Persistence**: Variables and functions persist between commands
- **Immediate Execution**: See results instantly
- **Session Management**: Save and load complete sessions

### Meta Commands

| Command | Description | Example |
|---------|-------------|---------|
| `:help` | Show available commands | `:help` |
| `:load <file>` | Load and execute a file | `:load examples/hello.bas` |
| `:save <file>` | Save current session | `:save my_session.bas` |
| `:clear` | Clear all variables and functions | `:clear` |
| `:exit` | Exit REPL mode | `:exit` |

### Example REPL Session

```
$ rbasic -r
rbasic REPL v1.0 - Interactive Mode
Type :help for commands, :exit to quit

rbasic> var x = 10;
rbasic> var y = 20;
rbasic> function add(a, b) {
...>     return a + b;
...> }
rbasic> print("Result:", add(x, y));
Result: 30

rbasic> for (var i = 1; i <= 3; i = i + 1) {
...>     print("Count:", i);
...> }
Count: 1
Count: 2
Count: 3

rbasic> :save my_work.bas
Session saved to my_work.bas

rbasic> :exit
Goodbye!
```

### REPL Best Practices

- Use multi-line mode for complex functions and control structures
- Save interesting sessions with `:save` for later use
- Load examples with `:load` to explore language features
- Use `:clear` to start fresh when experimenting

## Language Syntax

### Comments

```basic
// Single line comment

/* 
   Multi-line comment
   spanning several lines
*/

var x = 5; // End-of-line comment
```

### Statements

All statements must end with a semicolon (`;`):

```basic
var x = 10;
print("Hello");
if (x > 5) { print("Greater"); }
```

### Case Sensitivity

- Keywords are **case-sensitive** and **lowercase**: `var`, `if`, `function`
- Variable names are **case-sensitive**: `myVar` ≠ `MyVar`

## Variable Scope

rbasic follows C-style block scoping rules. Variables declared with `var` inside a block (between `{` and `}`) are only accessible within that block.

### Block Scope

Variables declared inside control structures have block scope:

```basic
var x = 10;  // Global scope

if (x > 5) {
    var y = 20;  // Block scope - only accessible inside this if block
    print("x =", x);  // Can access outer scope variables
    print("y =", y);  // Can access block scope variables
}

// print("y =", y);  // ERROR: y is not accessible here
```

### Nested Blocks

Inner blocks can access variables from outer blocks:

```basic
var outer = 1;

if (outer == 1) {
    var middle = 2;
    
    for (var i = 0; i < 3; i = i + 1) {
        var inner = 3;
        print("outer:", outer);   // ✓ Accessible
        print("middle:", middle); // ✓ Accessible  
        print("inner:", inner);   // ✓ Accessible
        print("i:", i);           // ✓ Accessible
    }
    
    // print("inner:", inner);  // ERROR: inner not accessible
    // print("i:", i);          // ERROR: i not accessible
}

// print("middle:", middle);    // ERROR: middle not accessible
```

### Function Scope

Function parameters and local variables have function scope:

```basic
var global = "I'm global";

function myFunction(param) {
    var local = "I'm local";
    print(global);  // ✓ Can access global variables
    print(param);   // ✓ Can access parameters
    print(local);   // ✓ Can access local variables
}

// print(param);   // ERROR: param not accessible outside function
// print(local);   // ERROR: local not accessible outside function
```

## Keywords Reference

### Variable Declaration

#### `var`
Declares a variable with optional initialization.

```basic
var x;              // Declare uninitialized
var y = 10;         // Declare and initialize
var name = "John";  // String variable
var flag = true;    // Boolean variable
```

### Control Flow Keywords

#### `if`, `then`, `else`

```basic
if (condition) {
    // statements
} else {
    // alternative statements
}

// Multi-branch
if (score >= 90) {
    print("A grade");
} else if (score >= 80) {
    print("B grade");
} else {
    print("C grade");
}
```

#### `for`
C-style for loops with initialization, condition, and increment.

```basic
for (var i = 0; i < 10; i = i + 1) {
    print("Count:", i);
}

// Nested loops
for (var row = 1; row <= 3; row = row + 1) {
    for (var col = 1; col <= 3; col = col + 1) {
        print("Position:", row, col);
    }
}
```

#### `while`
Loop while condition is true.

```basic
var count = 0;
while (count < 5) {
    print("Count:", count);
    count = count + 1;
}
```

### Function Keywords

#### `function`
Declares a function with parameters and return value.

```basic
function add(a, b) {
    return a + b;
}

function greet(name) {
    print("Hello,", name);
    return true;
}

// Using functions
var result = add(5, 3);
greet("Alice");
```

#### `return`
Returns a value from a function.

```basic
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
```

### Array Keywords

#### `dim`
Declares an array with specified size.

```basic
dim numbers(10);        // Array of 10 elements
dim matrix(5, 5);       // 2D array - fully supported

// Initialize array elements
numbers[0] = 42;
numbers[1] = 13;
print("First element:", numbers[0]);
```

### Structure Keywords

#### `struct`
Defines custom data structures using C-style syntax.

```basic
struct Point {
    x,
    y
};

struct Person {
    name,
    age,
    email
};

// Using structures
var p = Point { 10, 20 };
var person = Person { "John Doe", 30, "john@example.com" };

print("Point coordinates:", p.x, p.y);
print("Person:", person.name, "is", person.age, "years old");
```

### Logical Operators

#### `and`, `or`, `not`
Boolean logic operators.

```basic
var a = true;
var b = false;

if (a and b) {
    print("Both true");
}

if (a or b) {
    print("At least one true");
}

if (not b) {
    print("b is false");
}
```

## Built-in Functions

### Input/Output Functions

#### `print(...)`
Outputs values to console with automatic spacing.

```basic
print("Hello");                    // Hello
print("Sum:", 5 + 3);             // Sum: 8
print("Values:", 1, 2, 3);        // Values: 1 2 3
```

#### `input([prompt])`
Reads user input, optionally with a prompt.

```basic
var name = input();                // No prompt
var age = input("Enter age: ");    // With prompt
```

### Mathematical Functions

#### `sqrt(x)`
Square root of x.

```basic
print(sqrt(16));    // 4
print(sqrt(2));     // 1.41421...
```

#### `pow(base, exponent)`
Raises base to the power of exponent.

```basic
print(pow(2, 8));   // 256
print(pow(3, 3));   // 27
```

#### `sin(x)`, `cos(x)`, `tan(x)`
Trigonometric functions (x in radians).

```basic
print(sin(pi() / 2));   // 1
print(cos(0));          // 1
print(tan(pi() / 4));   // 1
```

#### `pi()`
Returns the value of π (pi).

```basic
var circumference = 2 * pi() * radius;
```

#### `rnd()`, `randomise()`
Random number generation.

```basic
randomise();           // Seed random generator
var dice = rnd() * 6 + 1;  // Random 1-6
```

#### `abs(x)`
Absolute value of x.

```basic
print(abs(-5));     // 5
print(abs(3.14));   // 3.14
```

#### `int(x)`
Convert to integer (truncate).

```basic
print(int(3.7));    // 3
print(int(-2.9));   // -2
```

### String Functions

#### `len(str)`
Returns length of string.

```basic
var text = "Hello";
print(len(text));   // 5
```

#### `left(str, n)`
Returns leftmost n characters.

```basic
var text = "Programming";
print(left(text, 4));   // "Prog"
```

#### `right(str, n)`
Returns rightmost n characters.

```basic
var text = "Programming";
print(right(text, 4));  // "ming"
```

#### `mid(str, start, length)`
Returns substring starting at position start.

```basic
var text = "Programming";
print(mid(text, 5, 4)); // "gram" (0-based indexing)
```

#### `str(x)`
Converts number to string.

```basic
var num = 42;
var text = "Number: " + str(num);
print(text);    // "Number: 42"
```

#### `val(str)`
Converts string to number.

```basic
var numStr = "3.14";
var num = val(numStr);
print(num * 2);     // 6.28
```

### Type Conversion Functions

#### Automatic Type Conversion

rbasic performs automatic type conversion in many contexts:

```basic
var result = "Number: " + 42;       // "Number: 42"
var sum = "5" + 3;                  // 8 (string to number)
print("Value: " + 3.14);            // "Value: 3.14"
```

## Data Types

### Primitive Types

| Type | Description | Examples |
|------|-------------|----------|
| **Integer** | Whole numbers | `42`, `-17`, `0` |
| **Double** | Floating-point numbers | `3.14`, `-2.5`, `1.0` |
| **String** | Text data | `"Hello"`, `'World'`, `""` |
| **Boolean** | True/false values | `true`, `false` |

### Type Examples

```basic
// Integer
var count = 10;
var negative = -5;

// Double
var pi = 3.14159;
var temperature = -2.5;

// String
var message = "Hello, World!";
var empty = "";

// Boolean
var isReady = true;
var isComplete = false;
```

## Control Flow

### Conditional Statements

#### Simple If Statement

```basic
if (temperature > 30) {
    print("It's hot outside!");
}
```

#### If-Else Statement

```basic
if (age >= 18) {
    print("You can vote!");
} else {
    print("Too young to vote.");
}
```

#### Multiple Conditions

```basic
if (score >= 90) {
    print("Excellent!");
} else if (score >= 70) {
    print("Good job!");
} else if (score >= 50) {
    print("You passed.");
} else {
    print("Better luck next time.");
}
```

### Loops

#### For Loop

```basic
// Count from 0 to 9
for (var i = 0; i < 10; i = i + 1) {
    print("Number:", i);
}

// Count backwards
for (var i = 10; i > 0; i = i - 1) {
    print("Countdown:", i);
}
```

#### While Loop

```basic
var guess = 0;
var target = 7;

while (guess != target) {
    guess = input("Guess the number (1-10): ");
    if (guess == target) {
        print("Correct!");
    } else {
        print("Try again!");
    }
}
```

### Comparison Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `==` | Equal to | `x == 5` |
| `!=` | Not equal to | `x != 0` |
| `<` | Less than | `x < 10` |
| `<=` | Less than or equal | `x <= 5` |
| `>` | Greater than | `x > 0` |
| `>=` | Greater than or equal | `x >= 18` |

### Logical Operators

```basic
// AND - both conditions must be true
if (age >= 16 and age <= 65) {
    print("Can drive");
}

// OR - at least one condition must be true
if (day == "Saturday" or day == "Sunday") {
    print("Weekend!");
}

// NOT - reverses boolean value
if (not isFinished) {
    print("Still working...");
}
```

## Functions and Procedures

### Defining Functions

```basic
function functionName(parameter1, parameter2) {
    // function body
    return value;
}
```

### Function Examples

#### Simple Function

```basic
function square(x) {
    return x * x;
}

var result = square(5);  // result = 25
```

#### Function with Multiple Parameters

```basic
function calculateArea(width, height) {
    return width * height;
}

var area = calculateArea(10, 5);  // area = 50
```

#### Function without Return Value

```basic
function printWelcome(name) {
    print("Welcome to rbasic,", name + "!");
    print("Enjoy programming!");
}

printWelcome("Alice");
```

### Recursive Functions

```basic
function fibonacci(n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

// Print first 10 Fibonacci numbers
for (var i = 0; i < 10; i = i + 1) {
    print("F(" + str(i) + ") =", fibonacci(i));
}
```

## Arrays and Structures

### Arrays

**Important: All arrays in rbasic are 0-indexed (like C/C++), not 1-indexed like traditional BASIC.**

#### Generic Arrays (Declaring Arrays)

```basic
// 1D arrays
dim numbers(5);     // Array with 5 elements (indices 0-4)
dim names(10);      // Array with 10 elements (indices 0-9)

// Multidimensional arrays with comma-separated syntax
dim matrix(3, 3);         // 3x3 2D array
dim cube(4, 4, 4);        // 4x4x4 3D array
dim tensor(2, 3, 4, 5);   // 4D array

// Access with comma-separated indices
matrix[0, 0] = 1;         // Top-left element
matrix[2, 2] = 9;         // Bottom-right element
cube[1, 2, 3] = 42;       // 3D access

// Use in loops for matrix operations
for (var i = 0; i < 3; i = i + 1) {
    for (var j = 0; j < 3; j = j + 1) {
        matrix[i, j] = i * 3 + j + 1;  // Fill with values 1-9
    }
}

// Variable assignment works with multidimensional syntax
var value = matrix[1, 1];  // Gets element at row 1, column 1
```

#### Typed Arrays (High Performance)

For better performance with homogeneous data, use typed arrays:

```basic
// Create typed arrays - much faster than generic arrays
var bytes = byte_array(256);      // Byte array (0-255 values), indices 0-255
var numbers = int_array(100);     // Integer array, indices 0-99
var decimals = double_array(50);  // Double precision array, indices 0-49

// Typed arrays support multi-dimensional as well
var image = byte_array(640, 480); // 2D byte array for image data
var matrix = double_array(4, 4);  // 4x4 matrix of doubles

// Assignment with automatic type conversion (0-indexed!)
bytes[0] = 255;                   // First element
bytes[255] = 128;                 // Last element  
numbers[0] = 42;                  // First integer
decimals[0] = 3.14159;           // First double

// Access returns the appropriate type
var b = bytes[0];     // Returns int (byte as int 0-255)
var n = numbers[0];   // Returns int
var d = decimals[0];  // Returns double
```

**Performance Note**: Typed arrays are significantly faster than generic arrays because they avoid variant type overhead. Use them for:
- Large data sets
- Numerical computations  
- File I/O buffers
- Image/audio processing

## File I/O Operations

rbasic provides comprehensive file I/O capabilities optimized for both text and binary data:

### File Management

```basic
// Check if file exists
if (file_exists("data.txt")) {
    print("File found!");
}

// Get file size in bytes
var size = file_size("data.txt");
print("File size:", size, "bytes");

// Copy files (read + write)
var content = read_text_file("source.txt");
write_text_file("copy.txt", content);

// Rename files
rename_file("old_name.txt", "new_name.txt");

// Delete files
delete_file("unwanted.txt");
```

### Text File I/O

```basic
// Write text to file
var text = "Hello, World!\nSecond line.";
write_text_file("output.txt", text);

// Read entire text file
var content = read_text_file("input.txt");
print("File contents:", content);

// Append to existing file
append_text_file("log.txt", "New log entry\n");
```

### Binary File I/O with Typed Arrays

```basic
// Create binary data
var data = byte_array(1024);  // 1KB buffer
for (var i = 0; i < 1024; i = i + 1) {
    data[i] = i mod 256;  // Fill with pattern
}

// Write binary file
write_binary_file("data.bin", data);

// Load binary file (auto-sized)
var loaded = load_binary_file("data.bin");
print("Loaded", len(loaded), "bytes");

// Process binary data
for (var i = 0; i < 10; i = i + 1) {
    print("Byte", i, "=", loaded[i]);
}
```

### Performance Benefits

File I/O with typed arrays provides significant advantages:

- **Zero-Copy Operations**: Direct memory access without variant overhead
- **Large File Support**: Efficient handling of multi-megabyte files
- **Type Safety**: Automatic bounds checking and type validation
- **Cross-Platform**: Works identically on Windows, Linux, and macOS

### Example: Image Processing Pipeline

```basic
// Load image data
var image = load_binary_file("photo.raw");
var width = 640;
var height = 480;

// Simple brightness adjustment
for (var i = 0; i < width * height; i = i + 1) {
    var pixel = image[i];
    pixel = pixel + 50;  // Brighten
    if (pixel > 255) pixel = 255;  // Clamp
    image[i] = pixel;
}

// Save processed image
write_binary_file("photo_bright.raw", image);
```

**Zero-Indexing Note**: Unlike traditional BASIC languages, rbasic uses 0-based indexing for all arrays, consistent with C/C++ and most modern languages. This makes interfacing with external libraries and file formats much more natural.

#### Using Generic Arrays

```basic
dim scores(3);

// Set values
scores[0] = 85;
scores[1] = 92;
scores[2] = 78;

// Get values
print("First score:", scores[0]);
print("Average:", (scores[0] + scores[1] + scores[2]) / 3);
```

#### Array Example - Shopping List

```basic
dim shopping(5);
shopping[0] = "Milk";
shopping[1] = "Bread";
shopping[2] = "Eggs";
shopping[3] = "Cheese";
shopping[4] = "Apples";

print("Shopping List:");
for (var i = 0; i < 5; i = i + 1) {
    print((i + 1) + ".", shopping[i]);
}
```

### Structures

#### Defining Structures

```basic
struct Person {
    name,
    age,
    email
};

struct Point {
    x,
    y
};
```

#### Using Structures

```basic
// Create structure with initial values
var person = Person { "John Doe", 30, "john@example.com" };

// Access structure fields
print("Name:", person.name);
print("Age:", person.age);
print("Email:", person.email);

// Modify structure fields
person.age = 31;
person.email = "newemail@example.com";
```

#### Structure Example - Point System

```basic
struct Point {
    x,
    y
};

function distance(p1, p2) {
    var dx = p1.x - p2.x;
    var dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

var start = Point { 0, 0 };
var end = Point { 3, 4 };

print("Distance:", distance(start, end));  // 5.0
```

## Import System

rbasic supports modular programming through a comprehensive import system that allows code organization across multiple files. The import system works identically in both interpreter and compile modes.

### Import Syntax

Import files using the `import` statement:

```basic
import "filename.bas";
```

### Basic Import Example

**math_lib.bas** - Mathematical utility functions:
```basic
// Math library with reusable functions
var math_version = 1;
print("Math library loaded successfully!");

function add_numbers(a as double, b as double) as double {
    return a + b;
}

function multiply_numbers(x as double, y as double) as double {
    return x * y;
}

function square(num as double) as double {
    return num * num;
}
```

**main.bas** - Using the math library:
```basic
import "math_lib.bas";

function main() {
    print("Using imported math functions:");
    
    var sum = add_numbers(10, 20);
    var product = multiply_numbers(5, 6);
    var squared = square(7);
    
    print("10 + 20 =", sum);        // 30
    print("5 * 6 =", product);      // 30  
    print("7^2 =", squared);        // 49
    
    print("Math library version:", math_version);
    
    return 0;
}
```

### Import Features

#### Cross-Module Function Calls
Functions defined in imported files are directly callable:

```basic
// graphics_lib.bas
function draw_rectangle(x, y, width, height) {
    print("Drawing rectangle at", x, y, "size", width, "x", height);
}

// main.bas
import "graphics_lib.bas";
draw_rectangle(10, 20, 100, 50);  // Works directly
```

#### Cross-Module Variable Access
Variables from imported files are accessible:

```basic
// config.bas
var app_name = "My Application";
var version = "1.0";
var debug_mode = true;

// main.bas
import "config.bas";
print("Running", app_name, "version", version);
if (debug_mode) {
    print("Debug mode enabled");
}
```

#### Path Resolution
The import system searches for files in multiple locations:

1. **Relative to importing file** - Files in the same directory
2. **Current working directory** - Where rbasic was executed
3. **Executable directory** - Where rbasic.exe is located  
4. **Library paths** - `lib/`, `stdlib/`, `library/` directories

**Example directory structure:**
```
project/
├── main.bas
├── lib/
│   ├── math_utils.bas
│   └── string_utils.bas
└── stdlib/
    ├── graphics.bas
    └── database.bas
```

```basic
// These all work from main.bas
import "lib/math_utils.bas";       // Relative path
import "stdlib/graphics.bas";       // Library path
import "config.bas";               // Same directory
```

#### Duplicate Import Prevention
Each file is imported only once, even if multiple files import it:

```basic
// utils.bas
print("Utils loaded");

// math.bas  
import "utils.bas";  // Loads utils.bas

// main.bas
import "utils.bas";  // Skipped (already loaded)
import "math.bas";   // Loads math.bas, utils.bas already loaded
```

#### Circular Import Detection
The system detects and prevents circular import loops:

```basic
// file_a.bas
import "file_b.bas";  // Error: Circular import detected

// file_b.bas
import "file_a.bas";
```

### Advanced Import Patterns

#### Standard Library Architecture
Create reusable library modules:

```basic
// stdlib/text_processing.bas
function trim_whitespace(text) {
    // Implementation here
    return trimmed_text;
}

function split_string(text, delimiter) {
    // Implementation here
    return string_array;
}

// stdlib/file_utils.bas
import "text_processing.bas";

function load_config_file(filename) {
    var content = read_text_file(filename);
    return split_string(trim_whitespace(content), "\n");
}

// main.bas
import "stdlib/file_utils.bas";
var config = load_config_file("settings.conf");
```

#### Execution Mode Differences

**Interpreter Mode** (`rbasic -i`):
- Imports resolved at runtime
- Fast iteration during development
- Dynamic import loading

**Compile Mode** (`rbasic -c`):
- Imports resolved at compile time  
- All imports inlined into single source file
- C-style preprocessing approach
- Self-contained executables

Both modes produce identical output and behavior.

### Import Best Practices

1. **Use clear file organization**: Group related functions in logical modules
2. **Avoid deep nesting**: Keep import chains reasonably shallow
3. **Document dependencies**: Comment what each import provides
4. **Use library directories**: Organize reusable code in `lib/` or `stdlib/`
5. **Consistent naming**: Use descriptive filenames that match their purpose

## Foreign Function Interface (FFI)

The rbasic FFI system provides production-ready integration with C libraries and system APIs. It supports comprehensive pointer operations, structure manipulation, and works identically in both interpreted and compiled modes. Recent enhancements include improved support for pointer-returning functions like `IMG_Load` and `SDL_CreateTextureFromSurface`, enabling full SDL2 texture loading capabilities.

### Key FFI Features

- **Enhanced Pattern Matching**: Improved support for functions returning pointers (IMG_Load, SDL_CreateTextureFromSurface)
- **Cross-Mode Compatibility**: Identical behavior in interpreter and compiled modes
- **Automatic Type Conversion**: Seamless conversion between BASIC and C types
- **Memory-Safe Operations**: Safe buffer allocation and pointer dereferencing
- **Production-Ready Graphics**: Full SDL2 texture pipeline working across all execution modes

### FFI Declaration Syntax

FFI functions are declared using the `ffi` keyword:

```basic
ffi "library_name" FunctionName(parameter1 as type1, parameter2 as type2) as return_type;
```

**Supported Types:**
- `integer` - 32-bit signed integer
- `string` - C-style null-terminated string  
- `pointer` - Generic void pointer for handles and structures

### Advanced FFI Features

#### Buffer Management and Pointer Operations

```basic
// Allocate different types of buffers
var int_buffer = alloc_int_buffer();       // Single int storage
var ptr_buffer = alloc_pointer_buffer();   // Single pointer storage  
var byte_buffer = alloc_buffer(1024);      // 1KB byte buffer

// Pointer dereferencing
var int_value = deref_int(int_buffer);     // Read int from buffer
var ptr_value = deref_pointer(ptr_buffer); // Read pointer from buffer
var str_value = deref_string(ptr_value);   // Read string from pointer

// Null pointer checking
if (is_null(ptr_value)) {
    print("Pointer is null");
} else {
    print("Pointer is valid");
}
```

#### SDL2 Integration Support - Production Ready

The FFI system includes comprehensive SDL2 support with full texture loading capabilities:

```basic
// SDL2 core functions
ffi "SDL2.dll" SDL_Init(flags as integer) as integer;
ffi "SDL2.dll" SDL_CreateWindow(title as string, x as integer, y as integer,
                                width as integer, height as integer, flags as integer) as pointer;
ffi "SDL2.dll" SDL_CreateRenderer(window as pointer, index as integer, flags as integer) as pointer;

// Enhanced texture support (production-ready)
ffi "SDL2_image.dll" IMG_Load(file as string) as pointer;
ffi "SDL2.dll" SDL_CreateTextureFromSurface(renderer as pointer, surface as pointer) as pointer;
ffi "SDL2.dll" SDL_FreeSurface(surface as pointer) as integer;
ffi "SDL2.dll" SDL_RenderCopy(renderer as pointer, texture as pointer, srcrect as pointer, dstrect as pointer) as integer;

// Event handling
ffi "SDL2.dll" SDL_PollEvent(event as pointer) as integer;
ffi "SDL2.dll" SDL_Quit() as integer;

function sdl_texture_demo() {
    // Initialize SDL2
    var result = SDL_Init(SDL_INIT_VIDEO);
    if (result != 0) {
        print("SDL2 initialization failed");
        return 1;
    }
    
    // Create window and renderer
    var window = SDL_CreateWindow("rbasic Texture Demo", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    var renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Load and create texture (works identically in interpreter and compiled modes)
    var surface = IMG_Load("image.bmp");
    if (not_null(surface)) {
        var texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface); // Free surface after texture creation
        
        if (not_null(texture)) {
            print("Texture created successfully!");
            
            // Display texture for 5 seconds
            for (var frame = 0; frame < 300; frame = frame + 1) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, get_constant("NULL"), get_constant("NULL"));
                SDL_RenderPresent(renderer);
                SDL_Delay(16); // ~60 FPS
            }
            
            SDL_DestroyTexture(texture);
        }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
```

#### SQLite Database Integration

```basic
// SQLite database operations
ffi "sqlite3.dll" sqlite3_open(filename as string, db as pointer) as integer;
ffi "sqlite3.dll" sqlite3_prepare_v2(db as pointer, sql as string, length as integer,
                                     stmt as pointer, tail as pointer) as integer;
ffi "sqlite3.dll" sqlite3_step(stmt as pointer) as integer;
ffi "sqlite3.dll" sqlite3_close(db as pointer) as integer;

function database_demo() {
    var db_ptr = alloc_pointer_buffer();
    var result = sqlite3_open("example.db", db_ptr);
    
    if (result != SQLITE_OK) {
        print("Cannot open database");
        return 1;
    }
    
    var db = deref_pointer(db_ptr);
    
    // Create table
    var stmt_ptr = alloc_pointer_buffer();
    var sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT)";
    
    result = sqlite3_prepare_v2(db, sql, len(sql), stmt_ptr, null);
    if (result == SQLITE_OK) {
        var stmt = deref_pointer(stmt_ptr);
        sqlite3_step(stmt);
        print("Table created successfully");
    }
    
    sqlite3_close(db);
    return 0;
}
```

### Built-in Constants

The FFI system includes extensive constant definitions:

#### SDL2 Constants
```basic
// Initialization flags
SDL_INIT_VIDEO, SDL_INIT_AUDIO, SDL_INIT_EVERYTHING

// Window flags  
SDL_WINDOW_SHOWN, SDL_WINDOW_RESIZABLE, SDL_WINDOW_FULLSCREEN

// Event types
SDL_QUIT, SDL_KEYDOWN, SDL_KEYUP, SDL_MOUSEBUTTONDOWN

// Key codes
SDLK_ESCAPE, SDLK_SPACE, SDLK_RETURN, SDLK_UP, SDLK_DOWN

// Renderer flags
SDL_RENDERER_ACCELERATED, SDL_RENDERER_PRESENTVSYNC
```

#### SQLite Constants
```basic
// Result codes
SQLITE_OK, SQLITE_ERROR, SQLITE_DONE, SQLITE_ROW

// Open flags
SQLITE_OPEN_READONLY, SQLITE_OPEN_READWRITE, SQLITE_OPEN_CREATE
```

#### Windows API Constants
```basic
// MessageBox types
MB_OK, MB_OKCANCEL, MB_YESNO
MB_ICONERROR, MB_ICONWARNING, MB_ICONINFORMATION
```

### Structure Support

#### SDL Structures

```basic
// Create and manipulate SDL structures
var rect = create_sdl_rect(10, 20, 100, 50);
print("Rectangle x:", get_rect_field(rect, "x"));    // 10
print("Rectangle y:", get_rect_field(rect, "y"));    // 20  
print("Rectangle w:", get_rect_field(rect, "w"));    // 100
print("Rectangle h:", get_rect_field(rect, "h"));    // 50

// SDL event structure
var event = create_sdl_event();
// Use with SDL_PollEvent, SDL_WaitEvent, etc.
```

### Basic FFI Examples

#### Windows API Functions

```basic
// System information functions
ffi "kernel32.dll" GetCurrentProcessId() as integer;
ffi "kernel32.dll" GetTickCount() as integer;
ffi "kernel32.dll" Sleep(milliseconds as integer) as integer;

function main() {
    print("Process ID:", GetCurrentProcessId());
    print("System uptime:", GetTickCount(), "ms");
    
    print("Sleeping for 1 second...");
    Sleep(1000);
    print("Done!");
    
    return 0;
}
```

#### User Interface Functions

```basic
// Message box and user interaction
ffi "user32.dll" MessageBoxA(hwnd as integer, text as string, caption as string, type as integer) as integer;
ffi "user32.dll" MessageBeep(type as integer) as integer;

function show_message(message, title) {
    // MB_OK = 0, MB_ICONINFORMATION = 64
    return MessageBoxA(0, message, title, 64);
}

function main() {
    MessageBeep(0);  // System beep
    show_message("Hello from rbasic!", "FFI Demo");
    return 0;
}
```

### Advanced FFI: Pointer Handling

FFI supports pointer types for working with handles, structures, and complex data:

```basic
// Example: File handling with pointers
ffi "kernel32.dll" CreateFileA(filename as string, access as integer, share as integer, 
                               security as pointer, creation as integer, flags as integer, 
                               template as pointer) as pointer;
ffi "kernel32.dll" WriteFile(handle as pointer, buffer as string, bytes as integer,
                            written as pointer, overlapped as pointer) as integer;
ffi "kernel32.dll" CloseHandle(handle as pointer) as integer;

function write_file_via_api(filename, content) {
    // GENERIC_WRITE = 0x40000000, CREATE_ALWAYS = 2
    var handle = CreateFileA(filename, 1073741824, 0, null, 2, 128, null);
    
    if (handle != null) {
        WriteFile(handle, content, len(content), null, null);
        CloseHandle(handle);
        return true;
    }
    
    return false;
}
```

### FFI Function Signature Patterns

The FFI system supports several common function signature patterns:

**No Parameters, Integer Return:**
```basic
ffi "kernel32.dll" GetCurrentProcessId() as integer;
ffi "kernel32.dll" GetTickCount() as integer;
```

**Single Integer Parameter:**
```basic
ffi "kernel32.dll" Sleep(milliseconds as integer) as integer;
ffi "user32.dll" MessageBeep(type as integer) as integer;
```

**Multiple Parameters with Mixed Types:**
```basic
ffi "user32.dll" MessageBoxA(hwnd as integer, text as string, caption as string, type as integer) as integer;
```

**Pointer Returns for Handles:**
```basic
ffi "kernel32.dll" CreateFileA(filename as string, access as integer, share as integer,
                               security as pointer, creation as integer, flags as integer,
                               template as pointer) as pointer;
```

### Cross-Platform FFI

FFI declarations can be made platform-specific using conditional compilation or runtime detection:

```basic
// Windows-specific functions
ffi "kernel32.dll" GetCurrentProcessId() as integer;
ffi "user32.dll" MessageBoxA(hwnd as integer, text as string, caption as string, type as integer) as integer;

// Linux-specific functions (hypothetical syntax for future)
// ffi "libc.so.6" getpid() as integer;
// ffi "libX11.so" XOpenDisplay(display_name as string) as pointer;

function main() {
    var pid = GetCurrentProcessId();  // Windows
    // var pid = getpid();            // Linux (future)
    
    print("Process ID:", pid);
    return 0;
}
```

### FFI Best Practices

1. **Error Checking**: Always check pointer returns for null
2. **Resource Management**: Close handles and free resources
3. **Type Safety**: Use appropriate types for parameters
4. **Documentation**: Comment API constants and behavior

```basic
// Good FFI usage example
ffi "kernel32.dll" CreateFileA(filename as string, access as integer, share as integer,
                               security as pointer, creation as integer, flags as integer,
                               template as pointer) as pointer;
ffi "kernel32.dll" CloseHandle(handle as pointer) as integer;

function safe_create_file(filename) {
    // Windows API constants
    var GENERIC_WRITE = 1073741824;    // 0x40000000
    var CREATE_ALWAYS = 2;
    var FILE_ATTRIBUTE_NORMAL = 128;
    
    var handle = CreateFileA(filename, GENERIC_WRITE, 0, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, null);
    
    if (handle == null) {
        print("Error: Failed to create file", filename);
        return null;
    }
    
    return handle;
}

function main() {
    var file = safe_create_file("test.txt");
    if (file != null) {
        // Use file...
        CloseHandle(file);  // Always clean up!
    }
    return 0;
}
```

### FFI Execution Modes

FFI works identically in both execution modes:

**Interpreted Mode** (`-i`):
- Direct function calls via dynamic library loading
- Immediate execution for development and testing

**Compiled Mode** (`-c`):
- Generated C++ code with runtime FFI calls
- Same behavior as interpreted mode
- Optimized for production deployment

Both modes provide the same FFI functionality and produce identical results.

## Examples

### Complete Programs

#### Calculator

```basic
// calculator.bas - Simple calculator
function add(a, b) { return a + b; }
function subtract(a, b) { return a - b; }
function multiply(a, b) { return a * b; }
function divide(a, b) {
    if (b == 0) {
        print("Error: Division by zero!");
        return 0;
    }
    return a / b;
}

print("Simple Calculator");
print("================");

var num1 = input("Enter first number: ");
var operator = input("Enter operator (+, -, *, /): ");
var num2 = input("Enter second number: ");

var result = 0;

if (operator == "+") {
    result = add(num1, num2);
} else if (operator == "-") {
    result = subtract(num1, num2);
} else if (operator == "*") {
    result = multiply(num1, num2);
} else if (operator == "/") {
    result = divide(num1, num2);
} else {
    print("Invalid operator!");
}

print("Result:", result);
```

#### Number Guessing Game

```basic
// guess.bas - Number guessing game
randomise();
var target = int(rnd() * 100) + 1;
var attempts = 0;
var maxAttempts = 7;

print("Guess the Number Game!");
print("I'm thinking of a number between 1 and 100.");
print("You have", maxAttempts, "attempts.");

while (attempts < maxAttempts) {
    var guess = input("Enter your guess: ");
    attempts = attempts + 1;
    
    if (guess == target) {
        print("Congratulations! You guessed it in", attempts, "attempts!");
        return;
    } else if (guess < target) {
        print("Too low! Try again.");
    } else {
        print("Too high! Try again.");
    }
    
    var remaining = maxAttempts - attempts;
    if (remaining > 0) {
        print("Attempts remaining:", remaining);
    }
}

print("Game over! The number was:", target);
```

#### Factorial and Prime Calculator

```basic
// math.bas - Mathematical functions
function factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

function isPrime(n) {
    if (n < 2) {
        return false;
    }
    for (var i = 2; i * i <= n; i = i + 1) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

function showPrimes(limit) {
    print("Prime numbers up to", limit + ":");
    for (var i = 2; i <= limit; i = i + 1) {
        if (isPrime(i)) {
            print(i);
        }
    }
}

print("Mathematical Functions Demo");
print("===========================");

var num = input("Enter a number for factorial: ");
print(num + "! =", factorial(num));

var limit = input("Find primes up to: ");
showPrimes(limit);
```

## Error Handling

### Common Errors and Solutions

#### Syntax Errors

```basic
// ERROR: Missing semicolon
var x = 5  // Should be: var x = 5;

// ERROR: Missing braces
if (x > 0)
    print("positive");  // Should use braces

// CORRECT:
if (x > 0) {
    print("positive");
}
```

#### Runtime Errors

```basic
// ERROR: Division by zero
function safeDivide(a, b) {
    if (b == 0) {
        print("Warning: Division by zero!");
        return 0;
    }
    return a / b;
}

// ERROR: Array out of bounds
dim arr(5);
// arr[10] = 5;  // This would cause an error

// CORRECT: Check bounds
if (index >= 0 and index < 5) {
    arr[index] = value;
}
```

#### Type Errors

```basic
// Be careful with type conversions
var userInput = input("Enter a number: ");
var number = val(userInput);  // Convert to number

// Check for valid conversion
if (number == 0 and userInput != "0") {
    print("Invalid number entered!");
}
```

## Performance and Optimization

### Automatic Parallelization

rbasic includes **automatic OpenMP-based parallelization** that provides significant performance improvements for large array operations without requiring any code changes:

#### How It Works

```basic
// This code automatically uses parallel execution for large arrays
dim large_array(50000);  // Arrays ≥1000 elements are automatically parallelized

// Array initialization automatically runs on multiple cores
for (var i = 0; i < 50000; i = i + 1) {
    large_array[i] = i * i;  // This loop runs in parallel automatically!
}

// Small arrays remain serial to avoid threading overhead
dim small_array(500);  // Arrays <1000 elements use serial execution
for (var i = 0; i < 500; i = i + 1) {
    small_array[i] = i;  // This runs serially for optimal performance
}
```

#### Key Features

- **Completely Invisible**: No code changes needed - parallelization happens automatically
- **Smart Thresholding**: Only arrays with 1000+ elements use parallel execution
- **Both Execution Modes**: Works in interpreter (`-i`) and compiled (`-c`) modes
- **Cross-Platform**: OpenMP support on Windows (MinGW64/MSVC), Linux (GCC), and macOS (Clang)
- **Error-Free**: No threading bugs since users don't write parallel code directly

#### Performance Tips

```basic
// For maximum performance with large datasets, use compiled mode
// ./rbasic -c your_program.bas -o optimized_program

// Typed arrays get better parallelization than generic arrays
var fast_array = double_array(100000);  // Preferred for numerical work
dim generic_array(100000);              // Still parallelized, but slower

// Large array operations benefit most from parallelization
for (var i = 0; i < 100000; i = i + 1) {
    fast_array[i] = sin(i * 0.001) * cos(i * 0.002);  // Parallel execution!
}
```

#### Execution Mode Comparison

| Mode | Array Size | Execution | Performance |
|------|------------|-----------|-------------|
| Interpreter | < 1000 elements | Serial | Fast startup |
| Interpreter | ≥ 1000 elements | Parallel | Good for development |
| Compiled | < 1000 elements | Serial | Optimized serial |
| Compiled | ≥ 1000 elements | Parallel | Maximum performance |

**Recommendation**: Use interpreter mode (`-i`) for development and testing, compiled mode (`-c`) for production and performance-critical applications.

## Best Practices

### Code Style

#### Naming Conventions

```basic
// Use descriptive variable names
var playerScore = 100;          // Good
var x = 100;                    // Poor

// Use camelCase for variables
var firstName = "John";         // Good
var first_name = "John";        // Acceptable but inconsistent

// Use descriptive function names
function calculateTotalPrice() { ... }  // Good
function calc() { ... }                 // Poor
```

#### Code Organization

```basic
// Group related functions together
// Math functions
function add(a, b) { return a + b; }
function multiply(a, b) { return a * b; }

// Display functions
function showMenu() { ... }
function showResults() { ... }

// Main program
function main() {
    showMenu();
    // ... main logic
}
```

#### Comments

```basic
// Use comments to explain complex logic
function fibonacci(n) {
    // Base case: F(0) = 0, F(1) = 1
    if (n <= 1) {
        return n;
    }
    
    // Recursive case: F(n) = F(n-1) + F(n-2)
    return fibonacci(n - 1) + fibonacci(n - 2);
}
```

### Performance Tips

#### Avoid Deep Recursion

```basic
// INEFFICIENT: Deep recursion
function fibonacciSlow(n) {
    if (n <= 1) return n;
    return fibonacciSlow(n - 1) + fibonacciSlow(n - 2);
}

// BETTER: Iterative approach
function fibonacciFast(n) {
    if (n <= 1) return n;
    
    var a = 0;
    var b = 1;
    
    for (var i = 2; i <= n; i = i + 1) {
        var temp = a + b;
        a = b;
        b = temp;
    }
    
    return b;
}
```

#### Minimize Function Calls in Loops

```basic
// INEFFICIENT: Function call in every iteration
for (var i = 0; i < len(someString); i = i + 1) {
    // len() called every iteration
}

// BETTER: Cache the result
var length = len(someString);
for (var i = 0; i < length; i = i + 1) {
    // len() called only once
}
```

### Development Workflow

#### Use Interpreter for Development

```bash
# Quick testing and debugging
rbasic -i myprogram.bas
```

#### Compile for Production

```bash
# Create optimized executable
rbasic -c myprogram.bas -o myprogram
```

#### Keep C++ Code for Inspection

```bash
# See generated C++ code
rbasic -c myprogram.bas --keep-cpp
cat temp_myprogram.cpp
```

---

## Further Reading

- **[Language Grammar (BNF)](BNF.md)** - Complete syntax specification
- **[README.md](README.md)** - Project overview and setup
- **[Examples Directory](examples/)** - More sample programs

For questions, bug reports, or contributions, visit the [GitHub repository](https://github.com/daver64/rbasic).

---

*This manual covers rbasic language features as of version 1.0. All documented features are fully implemented and tested.*