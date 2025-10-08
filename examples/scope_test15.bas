// Test 15: Gradual complexity increase
print("=== Test 15: Gradual complexity ===");

// Test A: Simple function, no parameters, no for loop
function test_a() as integer {
    return 42;
}

print("Test A start");
var result_a = test_a();
print("Test A end: " + result_a);

// Test B: Function with parameters, no for loop  
function test_b(x as integer) as integer {
    return x * 2;
}

print("Test B start");
var result_b = test_b(5);
print("Test B end: " + result_b);

// Test C: Function with simple for loop
function test_c() as integer {
    var total = 0;
    for (var i = 1; i <= 3; i = i + 1) {
        total = total + i;
    }
    return total;
}

print("Test C start");
var result_c = test_c();
print("Test C end: " + result_c);

// Test D: Function with parameters used in for loop
function test_d(max_val as integer) as integer {
    var total = 0;
    for (var i = 1; i <= max_val; i = i + 1) {
        total = total + i;
    }
    return total;
}

print("Test D start");
var result_d = test_d(3);
print("Test D end: " + result_d);

// Test E: Multiple parameters with complex for loop (the failing pattern)
function test_e(start_val as integer, end_val as integer, step_val as integer) as integer {
    var total = 0;
    var count = 0;
    
    for (var i = start_val; i <= end_val; i = i + step_val) {
        total = total + i;
        count = count + 1;
        if (count > 3) {
            return total;
        }
    }
    
    return total;
}

print("Test E start");
var result_e = test_e(2, 10, 2);
print("Test E end: " + result_e);

print("All tests completed");