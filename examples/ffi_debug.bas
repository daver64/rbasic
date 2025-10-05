// FFI Test - Phase 1 Foundation Test with Debug
// This tests the basic library loading functionality

print("Starting FFI test...");
var lib = load_library("kernel32.dll");
print("Load library returned: " + lib);

if (len(lib) > 15) {  // Check if it's a valid handle string
    print("Successfully loaded library: " + lib);
    
    var result = unload_library(lib);
    print("Unload library returned: " + result);
    if (result == 1.0) {
        print("Successfully unloaded library");
    } else {
        print("Failed to unload library");
    }
} else {
    print("Failed to load library: " + lib);
}
print("FFI test completed.");