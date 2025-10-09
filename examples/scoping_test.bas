// Minimal scoping test
print("Testing variable scoping...");

var test_var = 42;
print("test_var defined: " + str(test_var));

while (test_var > 0) {
    print("In while loop, test_var = " + str(test_var));
    var loop_var = test_var * 2;
    print("loop_var = " + str(loop_var));
    test_var = test_var - 1;
    
    if (test_var <= 40) {
        break;
    }
}

print("After while loop, test_var = " + str(test_var));
print("Scoping test completed.");