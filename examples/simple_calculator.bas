// Simple calculator test with nested if
print("Simple Calculator Test");

var a = 10;
var b = 5;
var choice = 1;

print("First number:", a);
print("Second number:", b);
print("Operation: 1 (Addition)");

var result = 0;

if (choice == 1) {
    result = a + b;
    print("Result:", a, "+", b, "=", result);
} else {
    if (choice == 2) {
        result = a - b;
        print("Result:", a, "-", b, "=", result);
    } else {
        if (choice == 3) {
            result = a * b;
            print("Result:", a, "*", b, "=", result);
        } else {
            if (choice == 4) {
                if (b != 0) {
                    result = a / b;
                    print("Result:", a, "/", b, "=", result);
                } else {
                    print("Error: Division by zero!");
                }
            } else {
                print("Invalid choice!");
            }
        }
    }
}