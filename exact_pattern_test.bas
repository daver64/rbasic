// Test exact same pattern as data pipeline
print("=== Exact Pattern Test ===");

if (file_exists("text_file_test.bas")) {
    var report_size = file_size("text_file_test.bas");
    var binary_size = 42;
    print("Got sizes - report:", report_size, "binary:", binary_size);
    print("Report file:", report_size, "bytes");
    print("Binary file:", binary_size, "bytes");
} else {
    print("File missing");
}

print("=== Test Complete ===");