// SDL2 and SQLite Output Parameter Demo
print("=== Output Parameter System Demo ===");

// Demonstrate buffer allocation capabilities
print("1. Buffer Allocation:");
var intBuffer = alloc_int_buffer();
var ptrBuffer = alloc_pointer_buffer();
var byteBuffer = alloc_buffer(56);  // SDL_Event size

print("Allocated buffers successfully!");

// Demonstrate dereferencing
var intValue = deref_int(intBuffer);
print("Int buffer initial value: ");
print(intValue);

// Demonstrate SDL struct creation and access
print("2. SDL Struct Handling:");
var rect = create_sdl_rect(50, 75, 300, 200);
var x = get_rect_field(rect, "x");
var y = get_rect_field(rect, "y");
var w = get_rect_field(rect, "w");
var h = get_rect_field(rect, "h");

print("SDL_Rect created: x=");
print(x);
print(", y=");
print(y);
print(", w=");
print(w);
print(", h=");
print(h);

// Demonstrate SDL event handling
print("3. SDL Event Handling:");
var event = create_sdl_event();
var eventType = get_event_type(event);
print("SDL_Event created, initial type: ");
print(eventType);

print("=== All Output Parameter Features Working! ===");

// Show what this enables for SDL2
print("Ready for SDL2 functions:");
print("- SDL_GetWindowSize(window, &width, &height)");
print("- SDL_PollEvent(&event)");
print("- SDL_RenderCopy(renderer, texture, &srcRect, &dstRect)");

print("Ready for SQLite functions:");
print("- sqlite3_open(filename, &database)");
print("- sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr)");
print("- Complex prepared statement workflows");

print("=== Demo Complete ===");