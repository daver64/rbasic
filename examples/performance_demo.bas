// Comprehensive test showcasing typed arrays for future file I/O
print("=== Typed Arrays Performance Test ===");

// Image processing simulation with byte arrays
var imageBuffer = byte_array(64);  // Small 8x8 image
print("Created 64-byte image buffer");

// Fill with a gradient pattern
for (var i = 0; i < 64; i = i + 1) {
    imageBuffer[i] = i * 4; // 0, 4, 8, 12... up to 252
}
print("Image buffer filled with gradient");

// Sample a few pixels
print("Sample pixels:");
print("Pixel 0:", imageBuffer[0]);   // Should be 0
print("Pixel 15:", imageBuffer[15]); // Should be 60
print("Pixel 31:", imageBuffer[31]); // Should be 124
print("Pixel 63:", imageBuffer[63]); // Should be 252

// Mathematical operations with int arrays
var fibonacci = int_array(10);
fibonacci[0] = 1;
fibonacci[1] = 1;

for (var i = 2; i < 10; i = i + 1) {
    fibonacci[i] = fibonacci[i-1] + fibonacci[i-2];
}

print("Fibonacci sequence:");
for (var i = 0; i < 10; i = i + 1) {
    print("fib[", i, "] =", fibonacci[i]);
}

// Scientific calculations with double arrays
var coords = double_array(6); // 2 3D points
coords[0] = 1.5;  coords[1] = 2.7;  coords[2] = 3.14; // Point 1
coords[3] = 4.8;  coords[4] = 5.9;  coords[5] = 6.28; // Point 2

print("3D Coordinates:");
print("Point 1: (", coords[0], ",", coords[1], ",", coords[2], ")");
print("Point 2: (", coords[3], ",", coords[4], ",", coords[5], ")");

// Calculate distance between points
var dx = coords[3] - coords[0];
var dy = coords[4] - coords[1]; 
var dz = coords[5] - coords[2];
var distance = sqrt(dx*dx + dy*dy + dz*dz);
print("Distance between points:", distance);

print("=== Test Complete ===");