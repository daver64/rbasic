// Test 1: Simple function with local variables
print("=== Test 1: Function with local variables ===");

function test_local_vars() as integer {
    var x = 5;
    var y = 10;
    return x + y;
}

var result = test_local_vars();
print("Result: " + result);