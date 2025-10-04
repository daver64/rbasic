// Simple calculator with modern C-style syntax
print("Simple Calculator");
print("Enter first number:");
var a = input();
print("Enter second number:");
var b = input();

print("Choose operation:");
print("1. Addition");
print("2. Subtraction");
print("3. Multiplication");
print("4. Division");
var choice = input();

if (choice == 1) {
    var result = a + b;
    print("Result:", a, "+", b, "=", result);
} else if (choice == 2) {
    var result = a - b;
    print("Result:", a, "-", b, "=", result);
} else if (choice == 3) {
    var result = a * b;
    print("Result:", a, "*", b, "=", result);
} else if (choice == 4) {
    if (b != 0) {
        var result = a / b;
        print("Result:", a, "/", b, "=", result);
    } else {
        print("Error: Division by zero!");
    }
} else {
    print("Invalid choice!");
}