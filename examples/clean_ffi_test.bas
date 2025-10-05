// Clean FFI test
print("Testing FFI call...");
ffi int GetCurrentProcessId() from "kernel32";
print("Declared GetCurrentProcessId");
var result = GetCurrentProcessId();
print("Success!");