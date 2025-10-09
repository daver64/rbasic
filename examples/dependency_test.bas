// Test variable dependencies in loop
print("Testing variable dependencies...");

var counter = 0;
while (counter < 3) {
    var base_val = counter * 10;
    var derived_val = base_val + 5;
    print("counter = " + str(counter) + ", base_val = " + str(base_val) + ", derived_val = " + str(derived_val));
    counter = counter + 1;
}
print("Test completed.");