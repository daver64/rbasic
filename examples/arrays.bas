// Example showing arrays in rbasic with modern C-style syntax

// 1D Arrays - Declare arrays using dim syntax
dim numbers(10);
dim names(5);

// Initialize array with numbers
print("Initializing 1D array with squares:");
for (var i = 0; i < 10; i = i + 1) {
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
for (var i = 0; i < 5; i = i + 1) {
    print(i, ": ", names[i]);
}

print("");

// 2D Arrays - Modern multidimensional syntax
dim matrix(3, 3);

print("Initializing 3x3 matrix:");
for (var i = 0; i < 3; i = i + 1) {
    for (var j = 0; j < 3; j = j + 1) {
        matrix[i, j] = i * 3 + j + 1;
    }
}

print("Matrix contents:");
for (var i = 0; i < 3; i = i + 1) {
    var row = "";
    for (var j = 0; j < 3; j = j + 1) {
        row = row + " " + str(matrix[i, j]);
    }
    print("Row", i, ":", row);
}

print("");

// 3D Arrays - True multidimensional support
dim cube(2, 2, 2);

print("Initializing 2x2x2 cube:");
for (var i = 0; i < 2; i = i + 1) {
    for (var j = 0; j < 2; j = j + 1) {
        for (var k = 0; k < 2; k = k + 1) {
            cube[i, j, k] = i * 4 + j * 2 + k + 1;
        }
    }
}

print("Cube contents:");
for (var i = 0; i < 2; i = i + 1) {
    print("Layer", i, ":");
    for (var j = 0; j < 2; j = j + 1) {
        var row = "";
        for (var k = 0; k < 2; k = k + 1) {
            row = row + " " + str(cube[i, j, k]);
        }
        print("  Row", j, ":", row);
    }
}

print("");

// Array calculations with 1D array
var sum = 0;
for (var i = 0; i < 10; i = i + 1) {
    sum = sum + numbers[i];
}

var average = sum / 10;
print("Sum of squares: ", sum);
print("Average: ", average);

// Find maximum
var max = numbers[0];
for (var i = 1; i < 10; i = i + 1) {
    if (numbers[i] > max) {
        max = numbers[i];
    }
}

print("Maximum value: ", max);

// Demonstrate multidimensional array element access
print("");
print("Individual element access:");
print("matrix[1,1] =", matrix[1, 1]);
print("cube[1,0,1] =", cube[1, 0, 1]);