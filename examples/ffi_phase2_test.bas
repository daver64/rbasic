// FFI Phase 2 Test - Function Declarations
print("Testing FFI Phase 2 - Function Declarations");

// Declare external functions from Windows API
declare function GetTickCount lib "kernel32" as int;
declare function MessageBoxA lib "user32" (hwnd as int, text as string, caption as string, type as int) as int;

print("FFI function declarations completed successfully!");