// Test 13: Multiple parameters with simple for loop
print("=== Test 13: Multiple parameters with simple for loop ===");

function test_multiple_params(start as integer, end as integer, step as integer) as integer {
    for (var i = start; i <= end; i = i + step) {
        if (i >= 6) {
            return i;
        }
    }
    return -1;
}

print("Before function call");
var func_result = test_multiple_params(2, 10, 2);
print("After function call: " + func_result);