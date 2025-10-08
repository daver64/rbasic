// Test 5: Complex parameter usage in for loop
print("=== Test 5: Complex parameter usage in for loop ===");

function test_complex_for(start as integer, end as integer, step as integer) as integer {
    var total = 0;
    var count = 0;
    
    for (var i = start; i <= end; i = i + step) {
        total = total + i;
        count = count + 1;
        if (count > 10) {
            break; // Safety check
        }
    }
    
    print("Processed " + count + " items");
    return total;
}

var result = test_complex_for(2, 10, 2);
print("Result: " + result);