// math_lib.bas - A simple math library for testing imports (working version)

function get_pi() as double {
    return 3.14159;
}

function double_number() as int {
    var input = 5;  // For now, use fixed values 
    return input * 2;
}

var math_version = 1;

// Print a message to show this library was loaded
print("Math library loaded successfully!");