// Simple non-graphics program
print("Hello from RBASIC!");

var x = 10;
var y = 20;
var sum = x + y;

print("x = " + x);
print("y = " + y);
print("sum = " + sum);

function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

var fact5 = factorial(5);
print("factorial(5) = " + fact5);

print("Program completed successfully!");