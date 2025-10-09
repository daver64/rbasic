// Isolate the exact scoping issue
print("Isolating scoping issue");

var counter = 0;

while (counter < 1) {
    print("Loop iteration " + str(counter));
    
    print("About to declare simple variable...");
    var simple_var = 42;
    print("simple_var = " + str(simple_var));
    
    print("About to declare variable with division...");
    var division_var = 120 / 20;
    print("division_var declared, value = " + str(division_var));
    
    counter = counter + 1;
}

print("Test complete");