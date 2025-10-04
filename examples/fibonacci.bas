// Fibonacci sequence calculator with modern C-style syntax
print("Fibonacci Sequence Calculator");
print("Enter the number of terms:");
var n = input();

var a = 0;
var b = 1;

print("Fibonacci sequence:");
print(a);
print(b);

for(var i = 3; i <= n; i = i + 1) {
    var c = a + b;
    print(c);
    a = b;
    b = c;
}