// Simple test for new SDL2_gfx functions// Test FFI loading directly// FFI Phase 3 Test - MessageBoxA with "type" keyword fix

// Tests just the FFI declarations without full SDL setup

function main() {// This test demonstrates that the parser now accepts keywords as parameter names

import "blib/sdl2.bas";

    print("Testing FFI system...");

print("Testing SDL2_gfx FFI declarations...");

    print("Testing FFI declaration with 'type' keyword...");

// For testing, we'll call the functions with null renderer to see if FFI works

// These should fail gracefully rather than crash if FFI is working    // Test loading SQLite library



var null_renderer = 0;    print("Loading SQLite3 library...");// This declaration was previously failing because "type" is a reserved keyword



print("Testing lineRGBA...");    var lib_result = load_library("sqlite3.dll");// Now it works thanks to the parser fix

// This should work if the 9-parameter FFI pattern is correct

var result1 = lineRGBA(null_renderer, 0, 0, 100, 100, 255, 0, 0, 255);    print("Load result: " + lib_result);declare function MessageBoxA lib "user32" (hwnd as int, text as string, title as string, type as int) as int;

print("lineRGBA result: " + str(result1));

    

print("Testing rectangleRGBA...");

var result2 = rectangleRGBA(null_renderer, 0, 0, 100, 100, 0, 255, 0, 255);      if (lib_result != "error:Failed to load sqlite3.dll") {print("MessageBoxA declaration successful!");

print("rectangleRGBA result: " + str(result2));

        print("Library loaded successfully!");print("The 'type' keyword can now be used as a parameter name in FFI declarations.");

print("Testing boxRGBA...");        

var result3 = boxRGBA(null_renderer, 0, 0, 100, 100, 0, 0, 255, 255);        // Test constants

print("boxRGBA result: " + str(result3));        print("Testing constants...");

        print("NULL = " + val(is_null(NULL)));

print("FFI test complete!");        print("SQLITE_OK = " + val(get_constant("SQLITE_OK")));
        
        // Test buffer allocation
        print("Testing buffer allocation...");
        var int_buf = alloc_int_buffer();
        var ptr_buf = alloc_pointer_buffer();
        
        print("Int buffer: " + val(not_null(int_buf)));
        print("Pointer buffer: " + val(not_null(ptr_buf)));
        
    } else {
        print("Failed to load SQLite library");
    }
    
    print("FFI test completed");
}

main();