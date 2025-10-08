// Test 8: Multiple variables without break
print("=== Test 8: Multiple variables without break ===");

function test_multiple_vars(start as integer, end as integer, step as integer) as integer {
    var total = 0;
    var count = 0;
    
    for (var i = start; i <= end; i = i + step) {
        total = total + i;
        count = count + 1;
        if (count > 3) {
            return total; // Use return instead of break
        }
    }
    
    print("Processed " + count + " items");
    return total;
}

var result = test_multiple_vars(2, 10, 2);
print("Result: " + result);