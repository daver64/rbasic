// FFI Debug Test 2 - Test library loading first
var lib = load_library("kernel32");
print("Library load result: " + lib);

// Only proceed if library loaded successfully
if (lib.length() > 0 && lib.find("error") == -1) {
    print("Library loaded successfully, now testing function call...");
    
    ffi int GetCurrentProcessId() from "kernel32";
    print("About to call GetCurrentProcessId...");
    var pid = GetCurrentProcessId();
    print("Process ID: " + string(pid));
} else {
    print("Library loading failed: " + lib);
}