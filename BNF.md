# rbasic Language Grammar (BNF) - Alpha

This document defines the Backus-Naur Form (BNF) grammar for the rbasic programming language as currently implemented.

⚠️ **Alpha Grammar**: This grammar represents the current Alpha implementation and may change as the language evolves.

## Overview

rbasic is an experimental modern BASIC language transpiler that combines BASIC simplicity with C-style syntax:
- C-style control structures with braces `{}`
- Basic import system for modular programming  
- Alpha Foreign Function Interface (FFI) for C library integration
- Growing built-in function library
- Dual execution modes (interpreter and compiler)

## Lexical Elements

### Tokens

```bnf
<token> ::= <identifier> | <number> | <string> | <keyword> | <operator> | <punctuation>

<identifier> ::= <letter> ( <letter> | <digit> | "_" )*
<letter> ::= "A".."Z" | "a".."z"
<digit> ::= "0".."9"

<number> ::= <integer> | <float>
<integer> ::= <digit>+
<float> ::= <digit>+ "." <digit>*

<string> ::= '"' <string_char>* '"'
<string_char> ::= <any_char_except_quote> | '\"'
```

### Keywords

```bnf
<keyword> ::= "var" | "if" | "else" | "for" | "while" | "function" | "return" 
            | "dim" | "struct" | "ffi" | "as" | "and" | "or" | "not"
```

### Operators

```bnf
<operator> ::= "+" | "-" | "*" | "/" | "mod" | "^"
             | "=" | "!=" | "<" | "<=" | ">" | ">="
             | "and" | "or" | "not"
             | "="
```

### Punctuation

```bnf
<punctuation> ::= "(" | ")" | "[" | "]" | "{" | "}" 
                | "," | ";" | ":" | "."
```

### Comments

```bnf
<comment> ::= <line_comment> | <block_comment>
<line_comment> ::= "//" <any_char>* <newline>
<block_comment> ::= "/*" <any_char>* "*/"
```

## Grammar Rules

### Program Structure

```bnf
<program> ::= <statement_list>

<statement_list> ::= <statement>*

<statement> ::= <variable_declaration>
              | <function_declaration>
              | <struct_declaration>
              | <ffi_declaration>
              | <assignment_statement>
              | <if_statement>
              | <for_statement>
              | <while_statement>
              | <return_statement>
              | <print_statement>
              | <input_statement>
              | <expression_statement>
              | <block_statement>
```

### Declarations

#### Variable Declaration

```bnf
<variable_declaration> ::= "dim" <identifier> <array_dimensions>? ( "as" <type> )? ";"

<array_dimensions> ::= "(" <expression> ( "," <expression> )* ")"

<type> ::= "integer" | "double" | "string" | "boolean" | <identifier>
```

#### Function Declaration

```bnf
<function_declaration> ::= "function" <identifier> "(" <parameter_list>? ")" 
                          ( "as" <type> )? <block_statement>

<parameter_list> ::= <parameter> ( "," <parameter> )*

<parameter> ::= <identifier> ( "as" <type> )?
```

#### Structure Declaration

```bnf
<struct_declaration> ::= "struct" <identifier> "{" <field_list> "}" ";"

<field_list> ::= <field_declaration> ( "," <field_declaration> )*

<field_declaration> ::= <identifier>
```

#### FFI Declaration

```bnf
<ffi_declaration> ::= "ffi" <string> <identifier> "(" <ffi_parameter_list>? ")" 
                     ( "as" <ffi_type> )? ";"
                    | "ffi" <ffi_type> <identifier> "(" <ffi_parameter_list>? ")" 
                     "from" <string> ";"

<ffi_parameter_list> ::= <ffi_parameter> ( "," <ffi_parameter> )*

<ffi_parameter> ::= <identifier> "as" <ffi_type>

<ffi_type> ::= "integer" | "string" | "pointer" | "double"
             | <identifier> "*"  // C-style pointer syntax
```

### Statements

#### Assignment Statement

```bnf
<assignment_statement> ::= "var" <lvalue> "=" <expression> ";"

<lvalue> ::= <identifier>
           | <identifier> "[" <expression> "]"
           | <identifier> "." <identifier>
```

#### Control Flow Statements

```bnf
<if_statement> ::= "if" "(" <expression> ")" <block_statement> 
                  ( "else" <block_statement> )?

<for_statement> ::= "for" "(" <assignment_statement> <expression> ";" <assignment_expression> ")" 
                   <block_statement>

<while_statement> ::= "while" "(" <expression> ")" <block_statement>

<return_statement> ::= "return" <expression>? ";"

<block_statement> ::= "{" <statement_list> "}"
```

#### I/O Statements

```bnf
<print_statement> ::= "print" "(" <expression_list>? ")" ";"

<input_statement> ::= "input" "(" ")" 

<expression_list> ::= <expression> ( "," <expression> )*
```

#### Expression Statement

```bnf
<expression_statement> ::= <expression> ";"
```

### Expressions

```bnf
<expression> ::= <assignment_expression>

<assignment_expression> ::= <logical_or_expression>
                           | <lvalue> "=" <assignment_expression>

<logical_or_expression> ::= <logical_and_expression> 
                          | <logical_or_expression> "or" <logical_and_expression>

<logical_and_expression> ::= <equality_expression>
                           | <logical_and_expression> "and" <equality_expression>

<equality_expression> ::= <comparison_expression>
                        | <equality_expression> "==" <comparison_expression>
                        | <equality_expression> "!=" <comparison_expression>

<comparison_expression> ::= <term_expression>
                          | <comparison_expression> "<" <term_expression>
                          | <comparison_expression> "<=" <term_expression>
                          | <comparison_expression> ">" <term_expression>
                          | <comparison_expression> ">=" <term_expression>

<term_expression> ::= <factor_expression>
                    | <term_expression> "+" <factor_expression>
                    | <term_expression> "-" <factor_expression>

<factor_expression> ::= <unary_expression>
                      | <factor_expression> "*" <unary_expression>
                      | <factor_expression> "/" <unary_expression>
                      | <factor_expression> "mod" <unary_expression>

<unary_expression> ::= <call_expression>
                     | "-" <unary_expression>
                     | "not" <unary_expression>

<call_expression> ::= <primary_expression>
                    | <call_expression> "(" <argument_list>? ")"
                    | <call_expression> "[" <expression> "]"
                    | <call_expression> "." <identifier>

<primary_expression> ::= <identifier>
                       | <number>
                       | <string>
                       | "(" <expression> ")"
                       | "true"
                       | "false"

<argument_list> ::= <expression> ( "," <expression> )*
```

## Type System

### Basic Types

```bnf
<basic_type> ::= "integer" | "double" | "string" | "boolean"
```

### Composite Types

```bnf
<array_type> ::= <basic_type> "[" <integer> "]"
<struct_type> ::= <identifier>
```

### Type Annotations

```bnf
<type_annotation> ::= "as" <type>
<type> ::= <basic_type> | <struct_type>
```

## Built-in Functions

The following built-in functions are available in rbasic:

### Math Functions
- `sqrt(x)` - Square root
- `abs(x)` - Absolute value
- `sin(x)`, `cos(x)`, `tan(x)` - Trigonometric functions
- `floor(x)`, `ceil(x)` - Floor and ceiling
- `rnd()` - Random number 0-1
- `randomise()` - Seed random generator (British spelling)
- `min(a, b)`, `max(a, b)` - Minimum and maximum
- `pow(base, exp)` - Power function
- `pi()` - Pi constant

### String Functions
- `len(str)` - String length
- `left(str, n)` - Left substring
- `right(str, n)` - Right substring
- `mid(str, start, length)` - Middle substring
- `str(x)` - Convert to string
- `val(str)` - Convert string to number

### System Functions
- `sleep_ms(ms)` - Sleep milliseconds

### FFI Helper Functions
- `alloc_buffer(size)` - Allocate memory buffer
- `alloc_int_buffer()` - Allocate integer buffer
- `alloc_pointer_buffer()` - Allocate pointer buffer
- `deref_int(ptr)` - Dereference integer pointer
- `deref_pointer(ptr)` - Dereference pointer
- `deref_string(ptr)` - Dereference string pointer
- `deref_int_offset(ptr, offset)` - Dereference with offset
- `is_null(ptr)`, `not_null(ptr)` - Null pointer checks

### SDL2 Structure Helpers
- `create_sdl_rect()` - Create SDL_Rect structure
- `create_sdl_event()` - Create SDL_Event structure
- `get_key_code(event)` - Extract keyboard scan code from event

### External Functions via FFI

Comprehensive FFI system provides extensive C library integration:

```basic
// Enhanced FFI with improved pointer support
ffi integer SDL_Init(flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateWindow(title as string, x as integer, y as integer, 
                           w as integer, h as integer, flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateRenderer(window as pointer, index as integer, 
                              flags as integer) from "SDL2.dll";

// Image loading with enhanced pattern matching
ffi pointer IMG_Load(filename as string) from "SDL2_image.dll";
ffi pointer SDL_CreateTextureFromSurface(renderer as pointer, 
                                        surface as pointer) from "SDL2.dll";

// Complete SDL2 texture pipeline working in both modes
var window = SDL_CreateWindow("Texture Demo", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
var renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
var surface = IMG_Load("texture.png");
var texture = SDL_CreateTextureFromSurface(renderer, surface);
```

## Example Programs

### Hello World
```basic
// Simple hello world
print("Hello, World!");
```

### Function Definition
```basic
function factorial(n as integer) as integer {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

var result = factorial(5);
print("5! =", result);
```

### FFI Integration Example
```basic
// Complete SDL2 graphics with texture loading
ffi integer SDL_Init(flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateWindow(title as string, x as integer, y as integer, 
                           w as integer, h as integer, flags as integer) from "SDL2.dll";
ffi pointer SDL_CreateRenderer(window as pointer, index as integer, 
                              flags as integer) from "SDL2.dll";
ffi pointer IMG_Load(filename as string) from "SDL2_image.dll";
ffi pointer SDL_CreateTextureFromSurface(renderer as pointer, 
                                        surface as pointer) from "SDL2.dll";

// Works identically in interpreter and compiled modes
SDL_Init(SDL_INIT_VIDEO);
var window = SDL_CreateWindow("Texture Demo", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
var renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
var surface = IMG_Load("texture.png");
if (not_null(surface)) {
    var texture = SDL_CreateTextureFromSurface(renderer, surface);
    print("Texture created successfully!");
}
```

### Structure Usage
```basic
struct Point {
    x,
    y
};

var p = Point { 10, 20 };
print("Point:", p.x, p.y);
```

### Array Usage
```basic
dim numbers(10) as integer;
for (var i = 1; i <= 10; i = i + 1) {
    var numbers[i] = i * i;
}
print("Square of 5:", numbers[5]);
```

## Notes

1. **Semicolons**: Required to terminate statements
2. **Braces**: Required for function bodies and control structure blocks
3. **Parentheses**: Required around conditions in if/while statements
4. **Case Sensitivity**: Keywords are case-sensitive and lowercase
5. **Array Indexing**: Arrays can be indexed starting from any integer
6. **Type Safety**: Basic type checking with optional type annotations
7. **Assignment Expressions**: Support for `var y = (x = x + 1)`
8. **Modern Comments**: C++ style `//` and `/* */` comments supported
9. **FFI Pattern Matching**: Enhanced support for pointer-returning functions like IMG_Load
10. **Cross-Mode Compatibility**: Identical behavior in interpreter and compiled execution modes
11. **Enhanced Graphics**: Complete SDL2 texture pipeline with IMG_Load and SDL_CreateTextureFromSurface support

This grammar defines the complete rbasic language as implemented in the current interpreter and compiler.