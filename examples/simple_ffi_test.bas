// Simple FFI test that should work// Simple FFI test

print("Testing simple FFI call...");ffi "kernel32.dll" GetCurrentProcessId() as integer;

ffi int GetTickCount() from "kernel32";

print("About to call GetTickCount...");print("Starting FFI test...");

var result = GetTickCount();

print("GetTickCount returned: " + result);function main() {
    print("Testing FFI...");
    var pid = GetCurrentProcessId();
    print("Process ID retrieved successfully!");
    print("PID value: ");
    print(pid);
    return 0;
}

// Call main directly
main();