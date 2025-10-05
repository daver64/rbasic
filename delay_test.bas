// Test with delay to allow file system sync
print("=== File Size with Delay Test ===");

write_text_file("delay_test.txt", "Test content for delay");
write_binary_file("delay_test.bin", byte_array(100));

print("Files written, checking existence...");
print("Text exists:", file_exists("delay_test.txt"));
print("Binary exists:", file_exists("delay_test.bin"));

print("Getting sizes immediately...");
var size1 = file_size("delay_test.txt");
var size2 = file_size("delay_test.bin");
print("Immediate sizes - text:", size1, "binary:", size2);

// Add a small delay (simulate with a loop)
print("Adding delay...");
for (var i = 0; i < 1000; i = i + 1) {
    // Small delay loop
    var dummy = i * 2;
}

print("Getting sizes after delay...");
var size3 = file_size("delay_test.txt");
var size4 = file_size("delay_test.bin");
print("Delayed sizes - text:", size3, "binary:", size4);

print("Test complete");