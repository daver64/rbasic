// Test 11: Complex for loop without global variable assignment
print("=== Test 11: Complex for loop without global variable assignment ===");

function test_multiple_vars(start as integer, end as integer, step as integer) as integer {
    var total = 0;
    var count = 0;
    
    for (var i = start; i <= end; i = i + step) {
        total = total + i;
        count = count + 1;
        if (count > 3) {
            return total;
        }
    }
    
    print("Processed " + count + " items");
    return total;
}

print("Calling function...");
var func_result = test_multiple_vars(2, 10, 2);
print("Function returned: " + func_result);