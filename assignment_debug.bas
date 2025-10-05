// Test exact assignment pattern with debug
print("=== Assignment Debug Test ===");

// Create the files
write_text_file("analysis_report.txt", "TEMPERATURE ANALYSIS REPORT\nTest content");
var binary_data = byte_array(400);
write_binary_file("sensor_data.bin", binary_data);

// Test the exact pattern from data pipeline
var report_size = 0;
var binary_size = 0;

print("Initial values: report_size =", report_size, "binary_size =", binary_size);

if (file_exists("analysis_report.txt") && file_exists("sensor_data.bin")) {
    print("Files exist, calling file_size...");
    
    print("Before assignment: report_size =", report_size);
    report_size = file_size("analysis_report.txt");
    print("After assignment: report_size =", report_size);
    
    print("Before assignment: binary_size =", binary_size);
    binary_size = file_size("sensor_data.bin");
    print("After assignment: binary_size =", binary_size);
    
    print("Report file:", report_size, "bytes");
    print("Binary file:", binary_size, "bytes");
} else {
    print("Files missing!");
}

print("Final values: report_size =", report_size, "binary_size =", binary_size);
print("Test complete");