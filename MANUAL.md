# rbasic User Manual

This manual provides comprehensive documentation for using the rbasic programming language, including all keywords, built-in functions, and tools.

## Table of Contents

- [Getting Started](#getting-started)
- [Command Line Usage](#command-line-usage)
- [Language Syntax](#language-syntax)
- [Keywords Reference](#keywords-reference)
- [Built-in Functions](#built-in-functions)
- [Data Types](#data-types)
- [Control Flow](#control-flow)
- [Functions and Procedures](#functions-and-procedures)
- [Arrays and Structures](#arrays-and-structures)
- [Examples](#examples)
- [Error Handling](#error-handling)
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
| `-o, --output <file>` | Specify output executable name | `rbasic -c program.bas -o myprogram` |
| `--io <type>` | Set I/O handler (console) | `rbasic -i program.bas --io console` |
| `--keep-cpp` | Keep generated C++ file | `rbasic -c program.bas --keep-cpp` |
| `-h, --help` | Show help message | `rbasic --help` |

### Usage Examples

```bash
# Interpret a program (fast development)
rbasic -i calculator.bas

# Compile to executable (production)
rbasic -c calculator.bas -o calc
./calc

# Compile and keep the generated C++ code
rbasic -c program.bas -o program --keep-cpp
```

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
dim matrix(5, 5);       // 2D array (future feature)

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

#### Declaring Arrays

```basic
dim numbers(5);     // Array with 5 elements
dim names(10);      // Array with 10 elements
```

#### Using Arrays

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

*This manual covers rbasic language features as of version 1.0. Features marked as "future" are planned but not yet implemented.*