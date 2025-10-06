// math_lib.bas - A simple math library for testing imports

function add_numbers(a as int, b as int) as int {
    return a + b;
}

function multiply_numbers(x as int, y as int) as int {
    return x * y;
}

function square(num as int) as int {
    return num * num;
}

var math_version = 1;

// Print a message to show this library was loaded
print("Math library loaded successfully!");