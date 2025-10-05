// Simple file I/O test
print("Testing file I/O...");

write_text_file("test_output.txt", "Hello World\nThis is a test");
print("Text file written");

if (file_exists("test_output.txt")) {
    var size = file_size("test_output.txt");
    print("File exists, size:", size);
    
    var content = read_text_file("test_output.txt");
    print("Content:", content);
} else {
    print("File does not exist!");
}

print("Test complete");