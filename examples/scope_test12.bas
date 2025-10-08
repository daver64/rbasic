// Test 12: Return from within for loop
print("=== Test 12: Return from within for loop ===");

function test_early_return() as integer {
    for (var i = 1; i <= 5; i = i + 1) {
        if (i == 3) {
            return i;
        }
    }
    return -1;
}

print("Before function call");
var func_result = test_early_return();
print("After function call: " + func_result);