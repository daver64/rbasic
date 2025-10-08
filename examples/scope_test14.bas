// Test 14: Basic function parameter access without for loop
print("=== Test 14: Basic function parameter access ===");

function test_params_only(start as integer, end as integer) as integer {
    print("start: " + start);
    print("end: " + end);
    return start + end;
}

var result = test_params_only(5, 10);
print("Result: " + result);