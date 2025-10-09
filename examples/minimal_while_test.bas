// Minimal while loop test
print("Minimal while loop test");

var counter = 0;
print("counter = " + str(counter));
print("counter < 3 = " + str(counter < 3));

print("Entering while loop...");
while (counter < 1) {
    print("Inside loop, counter = " + str(counter));
    counter = counter + 1;
    print("counter incremented to " + str(counter));
}
print("Exited loop");

print("Test complete");