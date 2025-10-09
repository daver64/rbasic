// Test complex expressions in variable initialization
print("Testing complex expressions");

var base_val = 120;
var counter = 0;

while (counter < 2) {
    print("Loop iteration " + str(counter));
    var simple_calc = base_val + counter;
    print("simple_calc = " + str(simple_calc));
    
    var division_calc = base_val / 20;
    print("division_calc = " + str(division_calc));
    
    var modulo_calc = base_val % 400;
    print("modulo_calc = " + str(modulo_calc));
    
    var complex_calc = 120 + (counter / 20) % 400;
    print("complex_calc = " + str(complex_calc));
    
    counter = counter + 1;
}

print("Complex expression test complete");