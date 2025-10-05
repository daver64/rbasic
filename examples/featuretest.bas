// rbasic Core Language Feature Test
// Comprehensive test of all basic language features

print("=== rbasic Core Language Feature Test ===");

// Test 1: Basic I/O and Variables
print("\n1. Testing Basic I/O and Variables");
var test_string = "Hello, rbasic!";
var test_number = 42;
var test_float = 3.141590;
var test_boolean = true;
print("String: " + test_string);
print("Number: " + str(test_number));
print("Float: " + str(test_float));
print("Boolean: " + str(test_boolean));

// Test 2: Arithmetic Operations
print("\n2. Testing Arithmetic Operations");
var a = 10;
var b = 3;
print("a = " + str(a) + ", b = " + str(b));
print("Addition: " + str(a + b));
print("Subtraction: " + str(a - b));
print("Multiplication: " + str(a * b));
print("Division: " + str(a / b));

// Test 3: Comparison Operations
print("\n3. Testing Comparison Operations");
print("a > b: " + str(a > b));
print("a < b: " + str(a < b));
print("a >= b: " + str(a >= b));
print("a <= b: " + str(a <= b));
print("a == b: " + str(a == b));
print("a != b: " + str(a != b));

// Test 4: Logical Operations
print("\n4. Testing Logical Operations");
var x = true;
var y = false;
print("x = " + str(x) + ", y = " + str(y));
print("x and y: " + str(x and y));
print("x or y: " + str(x or y));
print("not x: " + str(not x));
print("not y: " + str(not y));

// Test 5: String Operations
print("\n5. Testing String Operations");
var str1 = "Hello";
var str2 = "World";
var combined = str1 + " " + str2;
print("String concatenation: " + combined);
print("String length: " + str(len(combined)));
print("Substring (mid): " + mid(combined, 7, 5));
print("Left substring: " + left(combined, 5));
print("Right substring: " + right(combined, 5));

// Test 6: Conditional Statements
print("\n6. Testing Conditional Statements");
var test_value = 15;
if (test_value > 10) {
    print("Value is greater than 10");
} else {
    print("Value is 10 or less");
}

if (test_value < 5) {
    print("Value is less than 5");
} else if (test_value < 15) {
    print("Value is between 5 and 14");
} else {
    print("Value is 15 or greater");
}

// Test 7: While Loops
print("\n7. Testing While Loops");
var counter = 1;
print("While loop counting to 3:");
while (counter <= 3) {
    print("Count: " + str(counter));
    counter = counter + 1;
}

// Test 8: For Loops (both types)
print("\n8. Testing For Loops");
print("For loop with var declaration:");
for (var i = 1; i <= 3; i = i + 1) {
    print("For: " + str(i));
}

print("For loop with external variable:");
var j = 1;
for (j = 1; j <= 3; j = j + 1) {
    print("For: " + str(j));
}

// Test 9: Arrays
print("\n9. Testing Arrays");
dim numbers(5);
numbers[0] = 10;
numbers[1] = 20;
numbers[2] = 30;
print("Array element 0: " + str(numbers[0]));
print("Array element 1: " + str(numbers[1]));
print("Array element 2: " + str(numbers[2]));

// Test 10: Functions
print("\n10. Testing Functions");
function square(n) {
    return n * n;
}

function greet(name) {
    return "Hello, " + name + "!";
}

print("Square of 5: " + str(square(5)));
print("Greeting: " + greet("rbasic"));

// Test 11: Variable Scoping in Functions
print("\n11. Testing Function Scoping");
var global_var = "global";

function test_scope() {
    var local_var = "local";
    print("Inside function - global: " + global_var);
    print("Inside function - local: " + local_var);
    return "function result";
}

var result = test_scope();
print("Function result: " + result);

// Test 12: Type Conversions
print("\n12. Testing Type Conversions");
var num_string = "123";
var string_num = val(num_string);
print("String to number: " + str(string_num));
print("Number to string: " + str(456));

// Test 13: Complex Expressions
print("\n13. Testing Complex Expressions");
var result1 = (a + b) * (a - b);
var result2 = a > b and b > 0;
print("Complex arithmetic: " + str(result1));
print("Complex boolean: " + str(result2));

// Test 14: Boolean Logic
print("\n14. Testing Boolean Logic");
var condition1 = true;
var condition2 = false;
var condition3 = a > 5;
print("All conditions: " + str(condition1 and condition2 and condition3));
print("Any condition: " + str(condition1 or condition2 or condition3));

print("\n=== Test Complete ===");