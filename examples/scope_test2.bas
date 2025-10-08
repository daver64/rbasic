// Test 2: Function with parameters
print("=== Test 2: Function with parameters ===");

function test_params(a as integer, b as integer) as integer {
    var sum = a + b;
    return sum;
}

var result = test_params(7, 3);
print("Result: " + result);