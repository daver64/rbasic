// Test 3: For loop inside function
print("=== Test 3: For loop inside function ===");

function test_for_loop() as integer {
    var total = 0;
    
    for (var i = 1; i <= 5; i = i + 1) {
        total = total + i;
    }
    
    return total;
}

var result = test_for_loop();
print("Result: " + result);