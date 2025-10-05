// Simple FFI test
print("Declaring FFI function...");
ffi int GetCurrentProcessId() from "kernel32";
print("FFI function declared successfully!");

print("About to call function...");
// Don't call it yet, just see if declaration works
print("Test complete.");