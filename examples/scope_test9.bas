// Test 9: Check global variable access after for loop
print("=== Test 9: Check global variable access after for loop ===");

var global_var = 100;

function test_scope_corruption() as integer {
    print("Global var before for loop: " + global_var);
    
    for (var i = 1; i <= 2; i = i + 1) {
        print("In loop, global var: " + global_var);
    }
    
    print("Global var after for loop: " + global_var);
    return 42;
}

print("Calling function...");
test_scope_corruption();
print("Global var after function: " + global_var);