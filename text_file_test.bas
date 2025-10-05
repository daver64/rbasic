// Test text file writing with fixes
print("=== Text File Test ===");

var content = "Hello, this is a test file!\nLine 2\nLine 3";
write_text_file("test_write.txt", content);
print("File written");

if (file_exists("test_write.txt")) {
    var size = file_size("test_write.txt");
    print("File exists, size:", size, "bytes");
    
    var read_content = read_text_file("test_write.txt");
    print("File content:");
    print(read_content);
} else {
    print("File does not exist");
}

delete_file("test_write.txt");
print("=== Test Complete ===");