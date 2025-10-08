// Test 4: For loop using function parameters
print("=== Test 4: For loop using function parameters ===");

function test_for_with_params(max_value as integer) as integer {
    var total = 0;
    
    for (var i = 1; i <= max_value; i = i + 1) {
        total = total + i;
    }
    
    return total;
}

var result = test_for_with_params(5);
print("Result: " + result);