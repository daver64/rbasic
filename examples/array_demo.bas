// Comprehensive Array Demo for rbasic
print("=== Array Demo ===");

// Test 1: Integer array
print("Test 1: Integer Array");
dim numbers(5) as integer;

print("Setting values...");
var numbers[1] = 10;
var numbers[2] = 20;
var numbers[3] = 30;
var numbers[4] = 40;
var numbers[5] = 50;

print("Reading values:");
for(var i = 1; i <= 5; i = i + 1) {
    print("numbers[", i, "] =", numbers[i]);
}

// Test 2: String array
print("");
print("Test 2: String Array");
dim names(3) as string;

var names[1] = "Alice";
var names[2] = "Bob";
var names[3] = "Charlie";

print("Names:");
for(var i = 1; i <= 3; i = i + 1) {
    print("names[", i, "] =", names[i]);
}

// Test 3: Array calculations
print("");
print("Test 3: Array Calculations");
var sum = 0;
for(var i = 1; i <= 5; i = i + 1) {
    sum = sum + numbers[i];
}
print("Sum of numbers array:", sum);

var average = sum / 5;
print("Average:", average);

// Test 4: Find maximum
var max = numbers[1];
for(var i = 2; i <= 5; i = i + 1) {
    if (numbers[i] > max) {
        max = numbers[i];
    }
}
print("Maximum value:", max);

print("");
print("=== Array Demo Complete ===");