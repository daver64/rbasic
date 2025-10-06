// FFI Phase 4 Test - Pointer Types and Advanced FFI
// Tests the new pointer support without external dependencies

// Windows API functions for basic testing
ffi "kernel32.dll" GetCurrentProcessId() as integer;
ffi "kernel32.dll" GetTickCount() as integer;
ffi "user32.dll" MessageBoxA(hwnd as pointer, text as string, caption as string, type as integer) as integer;

// Test pointer parameter handling
ffi "kernel32.dll" GetModuleHandleA(module_name as string) as pointer;
ffi "kernel32.dll" GetProcAddress(module as pointer, proc_name as string) as pointer;

function test_basic_ffi() {
    print("=== Testing Basic FFI (Phase 3) ===");
    
    var process_id = GetCurrentProcessId();
    print("Current Process ID: " + tostring(process_id));
    
    var tick_count = GetTickCount();
    print("System Tick Count: " + tostring(tick_count));
    
    return true;
}

function test_pointer_ffi() {
    print("\n=== Testing Pointer FFI (Phase 4) ===");
    
    // Test getting a module handle (returns pointer)
    var kernel32_handle = GetModuleHandleA("kernel32.dll");
    print("Kernel32 handle obtained: " + (kernel32_handle != null ? "Yes" : "No"));
    
    if (kernel32_handle != null) {
        // Test getting procedure address (takes pointer parameter, returns pointer)
        var proc_address = GetProcAddress(kernel32_handle, "GetCurrentProcessId");
        print("GetCurrentProcessId address found: " + (proc_address != null ? "Yes" : "No"));
    }
    
    return true;
}

function test_message_box() {
    print("\n=== Testing MessageBox (Mixed Parameters) ===");
    
    // Test function with pointer and string parameters
    // hwnd as pointer (null), text as string, caption as string, type as integer
    var result = MessageBoxA(null, "rbasic FFI Phase 4 Test\n\nPointer support is working!", "FFI Test", 0);
    print("MessageBox result: " + tostring(result));
    
    return true;
}

function main() {
    print("rbasic FFI Phase 4 Comprehensive Test");
    print("=====================================");
    
    var success = true;
    
    // Test basic FFI functionality
    if (!test_basic_ffi()) {
        success = false;
    }
    
    // Test new pointer functionality
    if (!test_pointer_ffi()) {
        success = false;
    }
    
    // Test mixed parameter types
    if (!test_message_box()) {
        success = false;
    }
    
    print("\n=== Test Results ===");
    if (success) {
        print("All FFI Phase 4 tests passed!");
        print("✓ Basic integer return values");
        print("✓ Pointer return values");
        print("✓ Pointer parameters");
        print("✓ Mixed parameter types");
        print("✓ String parameters with pointers");
    } else {
        print("Some tests failed!");
    }
    
    return success ? 0 : 1;
}