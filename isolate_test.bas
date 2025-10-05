// Test to isolate the file size issue
print("=== File Size Root Cause Test ===");

// Step 1: Create a file exactly like the data pipeline does
var report = "TEMPERATURE ANALYSIS REPORT\n";
report = report + "============================\n\n";
report = report + "Data Points: 100\n";
report = report + "Sample minimum: 19.814420C\n";
report = report + "Sample maximum: 19.814420C\n";
report = report + "Sample average: 20.914310C\n\n";
report = report + "Processing: 3-point moving average applied\n";

print("Report content length:", len(report));
print("About to write file...");
write_text_file("test_analysis.txt", report);
print("File written");

// Step 2: Immediately check the file
print("Checking file immediately after write:");
print("  Exists:", file_exists("test_analysis.txt"));
print("  Size:", file_size("test_analysis.txt"));

// Step 3: Read the content back
var content = read_text_file("test_analysis.txt");
print("  Content length read back:", len(content));
print("  Content preview:", left(content, 50));

print("Test complete");