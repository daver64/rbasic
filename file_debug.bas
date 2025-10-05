// Debug file size issue step by step
print("=== File Size Debug ===");

// Test 1: Simple case
write_text_file("debug1.txt", "Hello");
var size1 = file_size("debug1.txt");
print("Test 1 - Simple file size:", size1);

// Test 2: Variables declared outside if block
var exists = file_exists("debug1.txt");
var size2 = 0;
if (exists) {
    size2 = file_size("debug1.txt");
}
print("Test 2 - Size from if block:", size2);

// Test 3: Variables declared inside if block (like data pipeline)
if (file_exists("debug1.txt")) {
    var size3 = file_size("debug1.txt");
    print("Test 3 - Size inside if:", size3);
}

// Clean up
delete_file("debug1.txt");
print("=== Debug Complete ===");