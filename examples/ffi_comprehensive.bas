// FFI Test for both interpreted and compiled modes
print("=== FFI System Test ===");

// Test 0-parameter function
ffi int GetCurrentProcessId() from "kernel32";
var pid = GetCurrentProcessId();
print("Process ID: ");
print(pid);

// Test 1-parameter function
ffi int MessageBeep(int) from "user32";
var result = MessageBeep(0);
print("MessageBeep result: ");
print(result);

// Test 4-parameter function (MessageBox)
ffi int MessageBoxA(int, string, string, int) from "user32";
var mbResult = MessageBoxA(0, "FFI Test", "rbasic FFI Working!", 0);
print("MessageBox result: ");
print(mbResult);

print("=== FFI Test Complete ===");