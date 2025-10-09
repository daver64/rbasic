// Test basic variable scoping in while loop
print("Testing basic variable scoping...");

var counter = 0;
print("counter defined outside loop: " + str(counter));

while (counter < 3) {
    print("Start of loop iteration " + str(counter));
    var loop_var = counter * 10;
    print("loop_var defined: " + str(loop_var));
    print("counter accessible: " + str(counter));
    counter = counter + 1;
    print("End of loop iteration");
}

print("After loop, counter = " + str(counter));
print("Basic scoping test complete");