// Test variable scoping similar to SDL demo structure
print("Testing SDL-like structure...");

// Simulate SDL functions
function get_time() as integer {
    print("get_time() called");
    return 42;
}

function is_running() as integer {
    print("is_running() called");
    return 1;
}

print("About to call get_time()...");
var start_time = get_time();
print("start_time = " + str(start_time));

var frame_count = 0;
print("frame_count initialized");

var loop_counter = 0;
print("About to enter while loop...");
print("loop_counter = " + str(loop_counter));
print("Condition check: loop_counter < 3 = " + str(loop_counter < 3));

while (loop_counter < 3) {  // Use counter instead of is_running() to avoid infinite loop
    print("ENTERED LOOP - iteration " + str(loop_counter));
    var current_time = get_time();
    print("current_time = " + str(current_time));
    var elapsed_ms = current_time - start_time;
    frame_count = frame_count + 1;
    
    print("Frame " + str(frame_count) + ", elapsed = " + str(elapsed_ms));
    
    // This is similar to the SDL demo structure
    var anim_x = 120 + (elapsed_ms / 20) % 400;
    print("anim_x = " + str(anim_x));
    
    loop_counter = loop_counter + 1;
}

print("SDL-like structure test complete");