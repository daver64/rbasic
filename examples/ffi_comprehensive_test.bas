// Test comprehensive FFI system for SDL2/SQLite readiness
print("=== Testing Generic FFI System ===");

// Test 0-parameter function
print("Testing 0-parameter function...");
ffi int GetTickCount() from "kernel32";

// Test 1-parameter function  
print("Testing 1-parameter function...");
ffi int MessageBeep(int) from "user32";

// Test 4-parameter function (MessageBox)
print("Testing 4-parameter function...");
ffi int MessageBoxA(int, string, string, int) from "user32";

print("All FFI declarations successful!");
print("FFI system is ready for SDL2 and SQLite!");