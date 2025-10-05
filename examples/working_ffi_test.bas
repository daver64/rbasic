// Test actual FFI function call
print("Testing FFI function call...");
ffi int GetCurrentProcessId() from "kernel32";
print("FFI declared");

var pid = GetCurrentProcessId();
print("GetCurrentProcessId returned: ");
print(pid);