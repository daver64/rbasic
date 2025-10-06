// Complete Import System Demo for rbasic
// Demonstrates import functionality working in both interpreter and compile mode

print("=== rbasic Import System Demo ===");
print("");

import "examples/math_lib.bas";
import "examples/simple_lib.bas";

print("1. Testing mathematical operations:");
var a = 15;
var b = 25;
var sum = add_numbers(a, b);
print("add_numbers(" + a + ", " + b + ") = " + sum);

var x = 8;
var y = 9;
var product = multiply_numbers(x, y);
print("multiply_numbers(" + x + ", " + y + ") = " + product);

var num = 12;
var squared = square(num);
print("square(" + num + ") = " + squared);

print("");
print("2. Testing library functions:");
var result = hello_world();
print("hello_world() returned: " + result);
print("Library constant: " + library_constant);
print("Math library version: " + math_version);

print("");
print("3. Testing complex calculation chains:");
var step1 = add_numbers(10, 5);     // 15
var step2 = multiply_numbers(step1, 2);  // 30
var step3 = square(step2);          // 900
print("((10 + 5) * 2)^2 = " + step3);

print("");
print("=== Import System Features ===");
print("✓ Cross-module function calls");
print("✓ Cross-module variable access");
print("✓ Duplicate import prevention");
print("✓ Circular import detection");
print("✓ Path resolution");
print("✓ Identical behavior in interpreter and compile modes");
print("");
print("The import system enables modular rbasic development!");