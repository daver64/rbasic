// Simple FFI debug test// Simple FFI test with guaranteed output// FFI Test - Phase 1 Foundation Test with Debug

ffi "kernel32.dll" GetCurrentProcessId() as integer;

ffi "kernel32.dll" GetCurrentProcessId() as integer;// This tests the basic library loading functionality

function main() {

    print("=== FFI Debug Test ===");

    print("About to call GetCurrentProcessId...");

    print("Starting FFI test...");print("Starting FFI test...");

    var pid = GetCurrentProcessId();

    var lib = load_library("kernel32.dll");

    print("FFI call completed");

    print("Result value:");function main() {print("Load library returned: " + lib);

    print(pid);

    print("=== Test Complete ===");    print("About to call GetCurrentProcessId...");

    

    return 0;    var pid = GetCurrentProcessId();if (len(lib) > 15) {  // Check if it's a valid handle string

}

    print("Process ID:", pid);    print("Successfully loaded library: " + lib);

main();
    print("FFI test completed!");    

    return 0;    var result = unload_library(lib);

}    print("Unload library returned: " + result);

    if (result == 1.0) {

main();        print("Successfully unloaded library");
    } else {
        print("Failed to unload library");
    }
} else {
    print("Failed to load library: " + lib);
}
print("FFI test completed.");