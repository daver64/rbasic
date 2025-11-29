// Raspberry Pi HC-SR04 Ultrasonic Distance Sensor
// Measures distance using GPIO timing
//
// Hardware Setup:
// - HC-SR04 VCC to 5V (pin 2)
// - HC-SR04 GND to GND (pin 6)
// - HC-SR04 TRIG to GPIO 23 (pin 16)
// - HC-SR04 ECHO to GPIO 24 (pin 18) *through voltage divider*
//
// IMPORTANT: Use voltage divider for ECHO pin (5V -> 3.3V)
// - 1kΩ resistor from ECHO to GPIO 24
// - 2kΩ resistor from GPIO 24 to GND

var GPIO_TRIG = 23;
var GPIO_ECHO = 24;
var GPIO_OUTPUT = 1;
var GPIO_INPUT = 0;

print("=== Raspberry Pi Distance Sensor ===");
print("HC-SR04 Ultrasonic Sensor");
print("");

// Initialize GPIO
print("Initializing GPIO...");
if (gpio_init() == 0) {
    print("Failed to initialize GPIO!");
    exit(1);
}

// Configure TRIG as output
gpio_set_mode(GPIO_TRIG, GPIO_OUTPUT);
gpio_write(GPIO_TRIG, 0);

// Configure ECHO as input
gpio_set_mode(GPIO_ECHO, GPIO_INPUT);

print("Sensor ready!");
print("Taking 10 distance measurements...");
print("");

// Take 10 measurements
var i = 0;
for (i = 0; i < 10; i = i + 1) {
    // Send 10μs trigger pulse
    gpio_write(GPIO_TRIG, 1);
    sleep(1);  // 1ms is more than enough (only need 10μs)
    gpio_write(GPIO_TRIG, 0);
    
    // Wait for echo to start (simplified timing)
    // In real application, would use precise microsecond timing
    // For demonstration, we'll use a simplified approach
    sleep(10);
    
    // Read echo state
    var echo = gpio_read(GPIO_ECHO);
    
    // Simplified distance calculation
    // Real implementation would time the echo pulse duration
    // Distance = (pulse_duration * 34300) / 2 (in cm)
    // For demonstration, show state only
    
    if (echo == 1) {
        print("Measurement " + str(i + 1) + ": Object detected (ECHO HIGH)");
    } else {
        print("Measurement " + str(i + 1) + ": No echo or out of range");
    }
    
    sleep(500);
}

print("");
print("Note: This is a simplified example.");
print("For accurate distance measurement, use precise microsecond timing.");
print("Consider using Python's RPi.GPIO or pigpio library for production use.");

// Cleanup
gpio_cleanup();
print("Done!");
