// Example showing arrays in rbasic with modern C-style syntax

// Declare arrays using modern syntax
var numbers[10];
var names[5];

// Initialize array with numbers
print("Initializing array with squares:");
for(var i = 0; i < 10; i = i + 1) {
    numbers[i] = (i + 1) * (i + 1);
    print("numbers[", i, "] = ", numbers[i]);
}

print("");

// Initialize string array  
names[0] = "Alice";
names[1] = "Bob";
names[2] = "Charlie";
names[3] = "Diana";
names[4] = "Eve";

print("Names in the array:");
for(var i = 0; i < 5; i = i + 1) {
    print(i, ": ", names[i]);
}

print("");

// Find sum and average
var sum = 0;
for(var i = 0; i < 10; i = i + 1) {
    sum = sum + numbers[i];
}

var average = sum / 10;
print("Sum of squares: ", sum);
print("Average: ", average);

// Find maximum
var max = numbers[0];
for(var i = 1; i < 10; i = i + 1) {
    if (numbers[i] > max) {
        max = numbers[i];
    }
}

print("Maximum value: ", max);