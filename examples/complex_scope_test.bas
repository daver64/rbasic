// Test more complex variable scoping
print("Testing complex variable scoping...");

var outer_var = 42;

while (outer_var > 0) {
    print("Outer loop, outer_var = " + str(outer_var));
    
    var mid_var = outer_var * 2;
    print("mid_var = " + str(mid_var));
    
    var inner_counter = 0;
    while (inner_counter < 2) {
        print("Inner loop " + str(inner_counter));
        var inner_var = mid_var + inner_counter;
        print("inner_var = " + str(inner_var));
        inner_counter = inner_counter + 1;
    }
    
    outer_var = outer_var - 1;
    if (outer_var <= 40) {
        break;
    }
}

print("Complex scoping test complete");