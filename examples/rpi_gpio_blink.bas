// Raspberry Pi GPIO LED Blink Example
// Demonstrates GPIO output control on Raspberry Pi Zero 2W
//
// Hardware Setup:
// - LED connected to GPIO 17 (pin 11) with 330Ω resistor to ground
// - LED anode (long leg) to GPIO 17
// - LED cathode (short leg) through resistor to ground
//
// Note: This example only works on ARM platforms (Raspberry Pi)

var GPIO_LED = 17;
var GPIO_OUTPUT = 1;
var LED_ON = 1;
var LED_OFF = 0;

// Initialize GPIO system
print("Initializing GPIO...");
if (gpio_init() == 0) {
    print("Failed to initialize GPIO!");
    print("Make sure you are running on a Raspberry Pi");
    print("and user is in 'gpio' group");
    exit(1);
}

// Configure GPIO 17 as output
print("Configuring GPIO " + str(GPIO_LED) + " as output...");
if (gpio_set_mode(GPIO_LED, GPIO_OUTPUT) == 0) {
    print("Failed to set GPIO mode!");
    gpio_cleanup();
    exit(1);
}

// Blink LED 10 times
print("Blinking LED 10 times...");
var i = 0;
for (i = 0; i < 10; i = i + 1) {
    print("Blink " + str(i + 1));
    
    // Turn LED on
    gpio_write(GPIO_LED, LED_ON);
    sleep(500); // 500ms delay
    
    // Turn LED off
    gpio_write(GPIO_LED, LED_OFF);
    sleep(500);
}

// Cleanup GPIO
print("Cleaning up GPIO...");
gpio_cleanup();
print("Done!");
