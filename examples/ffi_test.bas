// FFI Phase 3 Test - MessageBoxA with "type" keyword fix
// This test demonstrates that the parser now accepts keywords as parameter names

print("Testing FFI declaration with 'type' keyword...");

// This declaration was previously failing because "type" is a reserved keyword
// Now it works thanks to the parser fix
declare function MessageBoxA lib "user32" (hwnd as int, text as string, title as string, type as int) as int;

print("MessageBoxA declaration successful!");
print("The 'type' keyword can now be used as a parameter name in FFI declarations.");