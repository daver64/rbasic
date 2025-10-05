# rbasic Language Grammar (BNF)

This document defines the complete Backus-Naur Form (BNF) grammar for the rbasic programming language.

## Overview

rbasic is a "C-leaning BASIC" language that combines traditional BASIC simplicity with modern C-style syntax. It features:
- C-style control structures with braces `{}`
- Function-call I/O syntax
- Modern comments (`//` and `/* */`)
- Strong typing with type annotations
- Structures and arrays
- Assignment expressions
- Complete function library with 30+ built-in functions
- Extensible via Foreign Function Interface (FFI)

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
            | "dim" | "struct" | "and" | "or" | "not"
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

### Future: External Functions via FFI
External functionality such as graphics, databases, and system integration will be available through a Foreign Function Interface system.

### System Functions
- `sleep_ms(ms)` - Sleep milliseconds (Note: Core sleep function, not graphics-dependent)

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

This grammar defines the complete rbasic language as implemented in the current interpreter and compiler.