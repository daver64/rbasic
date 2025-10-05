// Test just the declaration
print("Before FFI declaration");
ffi int GetCurrentProcessId() from "kernel32";
print("After FFI declaration");
print("Test complete");