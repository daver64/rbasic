// Real-world example showing how constants make code readable
print("=== Constants Make Code Self-Documenting ===");
print("");

print("Without constants (hard to understand):");
print("SDL_CreateWindow(\"Game\", 536805376, 536805376, 800, 600, 4);");
print("");

print("With constants (self-documenting):");
print("SDL_CreateWindow(\"Game\", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);");
print("");

print("=== Constant Values for SDL2 ===");
print("SDL_WINDOWPOS_UNDEFINED = " + get_constant("SDL_WINDOWPOS_UNDEFINED"));
print("SDL_WINDOWPOS_CENTERED = " + get_constant("SDL_WINDOWPOS_CENTERED"));
print("SDL_WINDOW_SHOWN = " + get_constant("SDL_WINDOW_SHOWN"));
print("SDL_WINDOW_RESIZABLE = " + get_constant("SDL_WINDOW_RESIZABLE"));
print("SDL_WINDOW_FULLSCREEN = " + get_constant("SDL_WINDOW_FULLSCREEN"));
print("");

print("=== Constant Values for SQLite ===");
print("SQLITE_OK = " + get_constant("SQLITE_OK"));
print("SQLITE_ERROR = " + get_constant("SQLITE_ERROR"));
print("SQLITE_BUSY = " + get_constant("SQLITE_BUSY"));
print("SQLITE_DONE = " + get_constant("SQLITE_DONE"));
print("SQLITE_ROW = " + get_constant("SQLITE_ROW"));
print("");

print("=== Event Handling Constants ===");
print("SDL_QUIT = " + get_constant("SDL_QUIT"));
print("SDL_KEYDOWN = " + get_constant("SDL_KEYDOWN"));
print("SDL_KEYUP = " + get_constant("SDL_KEYUP"));
print("SDLK_ESCAPE = " + get_constant("SDLK_ESCAPE"));
print("SDLK_SPACE = " + get_constant("SDLK_SPACE"));
print("SDLK_RETURN = " + get_constant("SDLK_RETURN"));
print("");

print("=== NULL and Boolean Constants ===");
var null_value = get_constant("NULL");
print("NULL is null: " + is_null(null_value));
print("TRUE = " + get_constant("TRUE"));
print("FALSE = " + get_constant("FALSE"));
print("");

print("=== Practical Example Patterns ===");
print("");
print("1. Window creation:");
print("   var window = call_ffi_func(\"SDL2.dll\", \"SDL_CreateWindow\",");
print("                              \"My Game\",");
print("                              get_constant(\"SDL_WINDOWPOS_CENTERED\"),");
print("                              get_constant(\"SDL_WINDOWPOS_CENTERED\"),");
print("                              800, 600,");
print("                              get_constant(\"SDL_WINDOW_SHOWN\"));");
print("");

print("2. Database result checking:");
print("   var result = call_ffi_func(\"sqlite3.dll\", \"sqlite3_open\", \"db.sqlite\", ptr);");
print("   if (result == get_constant(\"SQLITE_OK\")) {");
print("       print(\"Database opened successfully\");");
print("   }");
print("");

print("3. Event handling:");
print("   var event_type = get_event_type(event);");
print("   if (event_type == get_constant(\"SDL_QUIT\")) {");
print("       running = false;");
print("   } else if (event_type == get_constant(\"SDL_KEYDOWN\")) {");
print("       var key = get_key_code(event);");
print("       if (key == get_constant(\"SDLK_ESCAPE\")) {");
print("           running = false;");
print("       }");
print("   }");
print("");

print("This system makes FFI code much more maintainable!");
print("No more magic numbers - everything is self-documenting.");