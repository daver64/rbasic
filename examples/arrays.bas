// Example showing arrays in rbasic with modern C-style syntax

// Declare arrays using dim statement
dim numbers(10) as integer;
dim names(5) as string;

// Initialize array with numbers
print("Initializing array with squares:");
for(var i = 1; i <= 10; i = i + 1) {
    var numbers[i] = i * i;
    print("numbers[", i, "] = ", numbers[i]);
}

print("");

// Initialize string array
var names[1] = "Alice";
var names[2] = "Bob";
var names[3] = "Charlie";
var names[4] = "Diana";
var names[5] = "Eve";

print("Names in the array:");
for(var i = 1; i <= 5; i = i + 1) {
    print(i, ": ", names[i]);
}

print("");

// Find sum and average
var sum = 0;
for(var i = 1; i <= 10; i = i + 1) {
    sum = sum + numbers[i];
}

var average = sum / 10;
print("Sum of squares: ", sum);
print("Average: ", average);

// Find maximum
var max = numbers[1];
for(var i = 2; i <= 10; i = i + 1) {
    if (numbers[i] > max) {
        max = numbers[i];
    }
}

print("Maximum value: ", max);