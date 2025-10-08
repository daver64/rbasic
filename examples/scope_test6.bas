// Test 6: Minimal reproduction of the scoping bug
print("=== Test 6: Minimal scoping bug reproduction ===");

function test_minimal_for() as integer {
    for (var i = 1; i <= 2; i = i + 1) {
        print("Loop iteration: " + i);
    }
    return 42;
}

print("Before function call");
var result = test_minimal_for();
print("After function call, result: " + result);