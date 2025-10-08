// Test 10: Check if for loop can access function parameters
print("=== Test 10: For loop parameter access ===");

function test_param_access(max_val as integer) as integer {
    print("Parameter max_val: " + max_val);
    
    for (var i = 1; i <= max_val; i = i + 1) {
        print("Loop " + i + ", max_val: " + max_val);
    }
    
    return 42;
}

test_param_access(3);