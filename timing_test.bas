// Test timing issue with file_size
print("=== File Size Timing Test ===");

write_text_file("timing_test.txt", "This is a test file for timing");
print("File written");

var size1 = file_size("timing_test.txt");
print("Size immediately after write:", size1);

// Let's also check if the file exists
if (file_exists("timing_test.txt")) {
    print("File exists immediately after write");
} else {
    print("File does NOT exist immediately after write");
}

print("Test complete");