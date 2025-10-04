// Example showing arrays in rbasic

// Declare arrays
dim numbers(10) as integer;
dim names(5) as string;

// Initialize array with numbers
print "Initializing array with squares:";
for i = 1 to 10
    var numbers(i) = i * i;
    print "numbers("; i; ") = "; numbers(i);
next i;

print;

// Initialize string array
var names(1) = "Alice";
var names(2) = "Bob";
var names(3) = "Charlie";
var names(4) = "Diana";
var names(5) = "Eve";

print "Names in the array:";
for i = 1 to 5
    print i; ": "; names(i);
next i;

print;

// Find sum and average
var sum = 0;
for i = 1 to 10
    var sum = sum + numbers(i);
next i;

var average = sum / 10;
print "Sum of squares: "; sum;
print "Average: "; average;

// Find maximum
var max = numbers(1);
for i = 2 to 10
    if numbers(i) > max then
        var max = numbers(i);
    end if;
next i;

print "Maximum value: "; max;