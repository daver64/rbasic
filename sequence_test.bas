// Test exact sequence from data pipeline
print("=== Exact Sequence Test ===");

// Create the exact same report content
report = "TEMPERATURE ANALYSIS REPORT\n";
report = report + "============================\n\n";
report = report + "Data Points: 100\n";
report = report + "Sample minimum: 19.814420C\n";
report = report + "Sample maximum: 19.814420C\n";
report = report + "Sample average: 20.914310C\n\n";
report = report + "Processing: 3-point moving average applied\n";

write_text_file("test_report.txt", report);
print("Text file written");

// Create a simple binary package  
dim binary_test(40);
for (var i = 0; i < 10; i = i + 1) {
    binary_test[i * 4 + 0] = 42;
    binary_test[i * 4 + 1] = 43; 
    binary_test[i * 4 + 2] = 44;
    binary_test[i * 4 + 3] = 45;
}

write_binary_file("test_data.bin", binary_test);
print("Binary file written");

// Now check sizes like the data pipeline does
var report_size = 0;
var binary_size = 0;

if (file_exists("test_report.txt") && file_exists("test_data.bin")) {
    report_size = file_size("test_report.txt");
    binary_size = file_size("test_data.bin");
    
    print("Report file:", report_size, "bytes");
    print("Binary file:", binary_size, "bytes");
} else {
    print("Files missing");
}

print("Test complete");