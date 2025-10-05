// FFI Example - Windows System Information
// Demonstrates Foreign Function Interface capabilities

// Windows API function declarations
ffi "kernel32.dll" GetCurrentProcessId() as integer;
ffi "kernel32.dll" GetTickCount() as integer;
ffi "kernel32.dll" Sleep(milliseconds as integer) as integer;

function main() {
    print("=== rbasic FFI Demo ===");
    print("");
    
    // Get system information
    var pid = GetCurrentProcessId();
    var uptime = GetTickCount();
    
    print("Current Process ID:", pid);
    print("System Uptime:", uptime, "milliseconds");
    print("");
    
    // Interactive timing demonstration
    print("Demonstrating Sleep function...");
    print("Sleeping for 2 seconds...");
    
    Sleep(2000);  // Sleep for 2000 milliseconds
    
    print("Sleep completed!");
    print("FFI demo finished successfully.");
    
    return 0;
}