// Test just binary file operation
print("=== Binary File Test ===");

var binary_test = byte_array(40);
for (var i = 0; i < 10; i = i + 1) {
    binary_test[i * 4 + 0] = 42;
    binary_test[i * 4 + 1] = 43; 
    binary_test[i * 4 + 2] = 44;
    binary_test[i * 4 + 3] = 45;
}

print("About to write binary file...");
write_binary_file("binary_only.bin", binary_test);
print("Binary file written");

if (file_exists("binary_only.bin")) {
    var size = file_size("binary_only.bin");
    print("Binary file exists, size:", size);
} else {
    print("Binary file does NOT exist");
}

print("Test complete");