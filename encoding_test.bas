// Test exact binary encoding pattern from data pipeline
print("=== Binary Encoding Test ===");

// Create test data similar to filtered_data
var test_data = double_array(100);
for (var i = 0; i < 100; i = i + 1) {
    test_data[i] = 20.0 + i * 0.1;  // Simple test data
}

// Create binary package exactly like the pipeline
var binary_package = byte_array(400);

print("About to encode binary data...");
for (var i = 0; i < 10; i = i + 1) {
    var temp_int = int(test_data[i] * 100);  // Store as centidegrees
    print("Processing i =", i, "value =", test_data[i], "temp_int =", temp_int);
    
    // Simple big-endian encoding (4 bytes per value)
    binary_package[i * 4 + 0] = (temp_int / 16777216) mod 256;  // MSB
    binary_package[i * 4 + 1] = (temp_int / 65536) mod 256;
    binary_package[i * 4 + 2] = (temp_int / 256) mod 256;
    binary_package[i * 4 + 3] = temp_int mod 256;              // LSB
}

print("Binary data encoded, writing file...");
write_binary_file("encoding_test.bin", binary_package);
print("File written");

if (file_exists("encoding_test.bin")) {
    var size = file_size("encoding_test.bin");
    print("File exists, size:", size);
} else {
    print("File does NOT exist");
}

print("Test complete");