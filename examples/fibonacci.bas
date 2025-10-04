// Fibonacci sequence calculator
print "Fibonacci Sequence Calculator";
print "Enter the number of terms:";
input n;

var a = 0;
var b = 1;

print "Fibonacci sequence:";
print a;
print b;

for i = 3 to n
    var c = a + b;
    print c;
    var a = b;
    var b = c;
next i;