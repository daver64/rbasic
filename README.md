# rbasic - A Modern BASIC Interpreter and Compiler

rbasic is a hybrid BASIC interpreter and compiler written in C++. It features modern syntax that bridges traditional BASIC with contemporary programming languages, making it familiar to developers from C/C++/JavaScript backgrounds while maintaining BASIC's simplicity.

## Features

- **Modern Syntax**: C-style for loops, function-call I/O, assignment expressions
- **Dual Execution Modes**: Interpret BASIC code directly or compile to C++ executable
- **Modern Comments**: C++ style `//` and `/* */` comments
- **Comprehensive Math Library**: 20+ built-in math functions
- **Compatible Behavior**: Interpreter and compiler produce identical results
- **Modern C++**: Built with C++17 for maintainability and performance

## Building

### Prerequisites
- CMake 3.16 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests
ctest
```

The `rbasic` executable will be placed in the project root directory.

## Usage

### Interpreter Mode
```bash
./rbasic -i program.bas
```

### Compiler Mode
```bash
./rbasic -c program.bas -o program
```

### Help
```bash
./rbasic --help
```

## Modern rbasic Language Features

### Variables and I/O (Modern Function-Call Style)
```basic
// Modern syntax with C++ style comments
var name = "World";
var number = 42;

print("Hello,", name);
print("The answer is", number);

print("Enter a number:");
var userInput = input();
print("You entered:", userInput);
```

### Modern For Loops (C-Style Syntax)
```basic
// Modern for loops with parentheses and semicolons
for(var i = 1; i <= 10; i = i + 1)
    print("Count:", i);
end;
```

### Modern While Loops
```basic
var j = 1;
while(j <= 5)
    print("While loop:", j);
    j = j + 1;
wend;
```

### Conditionals
```basic
var x = 15;
if x > 10 then
    print("x is greater than 10");
else
    print("x is not greater than 10");
end;
```

### Assignment Expressions
```basic
// Assignment works as both statement and expression
var x = 5;
var y = (x = x + 1) * 2;  // x becomes 6, y becomes 12
```

### Math Functions
```basic
var x = 16;
print("Square root:", sqrt(x));
print("Sine of 1:", sin(1));
print("Pi:", pi());
print("Power:", pow(2, 3));
```

### Functions
```basic
function factorial(n)
    if n <= 1 then
        return 1;
    else
        return n * factorial(n - 1);
    end;
end;

var result = factorial(5);
print("5! =", result);
```

## Project Structure

```
rbasic/
├── CMakeLists.txt
├── README.md
├── include/           # Header files
├── src/               # Source files
├── runtime/           # Runtime library for compiled programs
├── examples/          # Example BASIC programs
├── tests/             # Unit and integration tests
└── build/             # Build directory (created by CMake)
```

## Architecture

RBasic uses a shared AST (Abstract Syntax Tree) representation that both the interpreter and compiler operate on:

1. **Lexer**: Tokenizes BASIC source code
2. **Parser**: Builds AST from tokens
3. **Interpreter**: Directly executes AST nodes
4. **Code Generator**: Emits C++ code from AST
5. **Runtime Library**: Shared functions for both execution modes

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

MIT License - see LICENSE file for details.