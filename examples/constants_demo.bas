// Constants and NULL handling demo
print("=== Constants and NULL Handling Demo ===");
print("");

// Test core constants
print("Core constants:");
var null_ptr = get_constant("NULL");
print("NULL pointer obtained");
print("is_null(NULL): " + is_null(null_ptr));
print("not_null(NULL): " + not_null(null_ptr));

var true_val = get_constant("TRUE");
var false_val = get_constant("FALSE");
print("TRUE: " + true_val);
print("FALSE: " + false_val);
print("");

// Test SDL constants
print("SDL2 constants:");
print("SDL_INIT_VIDEO: " + get_constant("SDL_INIT_VIDEO"));
print("SDL_WINDOW_SHOWN: " + get_constant("SDL_WINDOW_SHOWN"));
print("SDL_WINDOWPOS_UNDEFINED: " + get_constant("SDL_WINDOWPOS_UNDEFINED"));
print("SDL_WINDOWPOS_CENTERED: " + get_constant("SDL_WINDOWPOS_CENTERED"));
print("");

// Test SDL event constants
print("SDL Event constants:");
print("SDL_QUIT: " + get_constant("SDL_QUIT"));
print("SDL_KEYDOWN: " + get_constant("SDL_KEYDOWN"));
print("SDLK_ESCAPE: " + get_constant("SDLK_ESCAPE"));
print("SDLK_SPACE: " + get_constant("SDLK_SPACE"));
print("");

// Test SQLite constants
print("SQLite constants:");
print("SQLITE_OK: " + get_constant("SQLITE_OK"));
print("SQLITE_ERROR: " + get_constant("SQLITE_ERROR"));
print("SQLITE_DONE: " + get_constant("SQLITE_DONE"));
print("SQLITE_ROW: " + get_constant("SQLITE_ROW"));
print("");

// Test Windows API constants
print("Windows API constants:");
print("MB_OK: " + get_constant("MB_OK"));
print("MB_YESNO: " + get_constant("MB_YESNO"));
print("MB_ICONERROR: " + get_constant("MB_ICONERROR"));
print("");

// Test NULL checking with actual pointers
print("=== NULL Pointer Testing ===");

// Create some buffers
var int_buf = alloc_int_buffer();
var ptr_buf = alloc_pointer_buffer();

print("int_buf is_null: " + is_null(int_buf));
print("int_buf not_null: " + not_null(int_buf));
print("ptr_buf is_null: " + is_null(ptr_buf));
print("ptr_buf not_null: " + not_null(ptr_buf));

// Test with NULL constant
print("NULL constant is_null: " + is_null(get_constant("NULL")));
print("NULL constant not_null: " + not_null(get_constant("NULL")));

print("");
print("Demo completed successfully!");