// Raspberry Pi GPIO Test (No LED Required)
// Tests GPIO functionality by reading and writing pins

var GPIO_PIN = 17;
var GPIO_OUTPUT = 1;
var GPIO_INPUT = 0;

print("=== Raspberry Pi GPIO Test ===");
print("");

// Initialize GPIO
print("1. Initializing GPIO system...");
var result = gpio_init();
if (result == 0) {
    print("   FAILED - Are you on a Raspberry Pi?");
    exit(1);
}
print("   SUCCESS");
print("");

// Test output mode
print("2. Setting GPIO " + str(GPIO_PIN) + " as OUTPUT...");
result = gpio_set_mode(GPIO_PIN, GPIO_OUTPUT);
if (result == 0) {
    print("   FAILED");
    gpio_cleanup();
    exit(1);
}
print("   SUCCESS");
print("");

// Test writing HIGH
print("3. Writing HIGH to GPIO " + str(GPIO_PIN) + "...");
result = gpio_write(GPIO_PIN, 1);
if (result == 0) {
    print("   FAILED");
} else {
    print("   SUCCESS - Pin should be at 3.3V");
}
print("");

sleep(1000);

// Test writing LOW
print("4. Writing LOW to GPIO " + str(GPIO_PIN) + "...");
result = gpio_write(GPIO_PIN, 0);
if (result == 0) {
    print("   FAILED");
} else {
    print("   SUCCESS - Pin should be at 0V");
}
print("");

// Test input mode
print("5. Setting GPIO " + str(GPIO_PIN) + " as INPUT...");
result = gpio_set_mode(GPIO_PIN, GPIO_INPUT);
if (result == 0) {
    print("   FAILED");
} else {
    print("   SUCCESS");
}
print("");

// Read the pin
print("6. Reading GPIO " + str(GPIO_PIN) + "...");
var value = gpio_read(GPIO_PIN);
print("   Value: " + str(value) + " (floating, may be random)");
print("");

// Cleanup
print("7. Cleaning up GPIO...");
gpio_cleanup();
print("   Done!");
print("");
print("=== All GPIO tests completed ===");
print("");
print("Hardware test:");
print("- Connect an LED (with 330Ω resistor) between GPIO 17 (pin 11) and GND");
print("- Then run: ./rbasic -i examples/rpi_gpio_blink.bas");
