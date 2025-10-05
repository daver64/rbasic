// Test for new typed arrays
var bytes = byte_array(10);
var numbers = int_array(5);
var decimals = double_array(3);

// Test assignment and access
bytes[1] = 255;
numbers[1] = 42;
decimals[1] = 3.14159;

print("Byte array element 1:", bytes[1]);
print("Int array element 1:", numbers[1]);
print("Double array element 1:", decimals[1]);

// Test more elements
bytes[2] = 128;
numbers[2] = -100;
decimals[2] = 2.71828;

print("Byte array element 2:", bytes[2]);
print("Int array element 2:", numbers[2]);
print("Double array element 2:", decimals[2]);