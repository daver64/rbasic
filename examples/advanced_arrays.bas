// Comprehensive typed array test with multi-dimensional arrays
var matrix = int_array(3, 3);  // 3x3 matrix
var buffer = byte_array(256);  // Large byte buffer
var coords = double_array(100, 3);  // 100 3D coordinates

// Test 2D array access (matrix multiplication setup)
matrix[1] = 1; matrix[2] = 2; matrix[3] = 3;
matrix[4] = 4; matrix[5] = 5; matrix[6] = 6;
matrix[7] = 7; matrix[8] = 8; matrix[9] = 9;

print("3x3 Matrix:");
print(matrix[1], matrix[2], matrix[3]);
print(matrix[4], matrix[5], matrix[6]);
print(matrix[7], matrix[8], matrix[9]);

// Test byte buffer (useful for file I/O later)
for (var i = 1; i <= 10; i = i + 1) {
    buffer[i] = i * 25;
}

print("First 10 bytes in buffer:");
for (var i = 1; i <= 10; i = i + 1) {
    print("buffer[" + i + "] =", buffer[i]);
}

// Test 3D coordinates
coords[1] = 1.0;  // x1
coords[2] = 2.0;  // y1
coords[3] = 3.0;  // z1
coords[4] = 4.5;  // x2
coords[5] = 5.5;  // y2
coords[6] = 6.5;  // z2

print("First two 3D coordinates:");
print("Point 1: (", coords[1], ",", coords[2], ",", coords[3], ")");
print("Point 2: (", coords[4], ",", coords[5], ",", coords[6], ")");