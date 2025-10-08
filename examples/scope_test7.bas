// Test 7: For loop with break statement
print("=== Test 7: For loop with break statement ===");

function test_break() as integer {
    var total = 0;
    
    for (var i = 1; i <= 10; i = i + 1) {
        total = total + i;
        if (i >= 3) {
            break;
        }
    }
    
    return total;
}

var result = test_break();
print("Result: " + result);