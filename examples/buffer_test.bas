// Buffer Allocation and Output Parameter Test
print("=== Buffer Allocation Test ===");

// Test basic buffer allocation
print("Testing alloc_int_buffer...");
var intBuffer = alloc_int_buffer();
print("Int buffer allocated: ");
print(intBuffer);

print("Testing alloc_pointer_buffer...");
var ptrBuffer = alloc_pointer_buffer();
print("Pointer buffer allocated: ");
print(ptrBuffer);

print("Testing alloc_buffer(32)...");
var buffer32 = alloc_buffer(32);
print("32-byte buffer allocated: ");
print(buffer32);

// Test dereferencing (should read initial values)
print("Testing deref_int (should be 0)...");
var intValue = deref_int(intBuffer);
print("Int value: ");
print(intValue);

// Test SDL struct creation
print("Testing create_sdl_rect(10, 20, 100, 200)...");
var rect = create_sdl_rect(10, 20, 100, 200);
print("SDL_Rect created: ");
print(rect);

print("Testing get_rect_field(rect, 'x')...");
var rectX = get_rect_field(rect, "x");
print("Rect X: ");
print(rectX);

print("Testing get_rect_field(rect, 'w')...");
var rectW = get_rect_field(rect, "w");
print("Rect W: ");
print(rectW);

// Test SDL event buffer
print("Testing create_sdl_event...");
var event = create_sdl_event();
print("SDL_Event created: ");
print(event);

print("Testing get_event_type (should be 0)...");
var eventType = get_event_type(event);
print("Event type: ");
print(eventType);

print("=== Buffer Test Complete ===");