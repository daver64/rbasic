// Test with exact same filenames as data pipeline
print("=== Exact Filename Test ===");

// Create test files with exact same names as data pipeline
write_text_file("analysis_report.txt", "Test report content");
var binary_package = byte_array(400);
for (var i = 0; i < 10; i = i + 1) {
    binary_package[i * 4 + 0] = 42;
    binary_package[i * 4 + 1] = 43;
    binary_package[i * 4 + 2] = 44;
    binary_package[i * 4 + 3] = 45;
}
write_binary_file("sensor_data.bin", binary_package);

print("Files created");

// Test the EXACT same code as data pipeline
var report_size = 0;
var binary_size = 0;

if (file_exists("analysis_report.txt") && file_exists("sensor_data.bin")) {
    report_size = file_size("analysis_report.txt");
    binary_size = file_size("sensor_data.bin");
    
    print("  Report file: ", report_size, " bytes");
    print("  Binary file: ", binary_size, " bytes");
    print("  [OK] All files created successfully");
} else {
    print("  [ERROR] Some files missing");
}

print("Test complete");