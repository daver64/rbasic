// Raspberry Pi Button and LED Control
// Button press toggles LED state
//
// Hardware Setup:
// - LED with 330Ω resistor: GPIO 17 (pin 11) to GND
// - Push button: GPIO 27 (pin 13) to GND (with internal pull-up)

var GPIO_LED = 17;
var GPIO_BUTTON = 27;
var GPIO_OUTPUT = 1;
var GPIO_INPUT = 0;
var GPIO_PULL_UP = 2;

print("=== Raspberry Pi Button & LED ===");
print("Press button to toggle LED");
print("Press Ctrl+C to exit (run for 30 seconds)");
print("");

// Initialize GPIO
if (gpio_init() == 0) {
    print("Failed to initialize GPIO!");
    exit(1);
}

// Configure LED pin
gpio_set_mode(GPIO_LED, GPIO_OUTPUT);
gpio_write(GPIO_LED, 0);

// Configure button pin with pull-up resistor
gpio_set_mode(GPIO_BUTTON, GPIO_INPUT);
gpio_set_pull(GPIO_BUTTON, GPIO_PULL_UP);

print("Hardware configured!");
print("Button: GPIO 27 (active LOW)");
print("LED: GPIO 17");
print("");

var led_state = 0;
var last_button = 1;  // Released (pulled high)
var samples = 0;
var max_samples = 300;  // 30 seconds at 100ms intervals

print("Monitoring button (30 seconds)...");

while (samples < max_samples) {
    // Read button state (LOW when pressed due to pull-up)
    var button = gpio_read(GPIO_BUTTON);
    
    // Detect button press (transition from HIGH to LOW)
    if (button == 0 and last_button == 1) {
        // Toggle LED state
        if (led_state == 0) {
            led_state = 1;
            gpio_write(GPIO_LED, 1);
            print("Button pressed - LED ON");
        } else {
            led_state = 0;
            gpio_write(GPIO_LED, 0);
            print("Button pressed - LED OFF");
        }
        
        // Debounce delay
        sleep(200);
    }
    
    last_button = button;
    samples = samples + 1;
    sleep(100);  // Check every 100ms
}

print("");
print("Monitoring complete!");

// Turn off LED and cleanup
gpio_write(GPIO_LED, 0);
gpio_cleanup();
print("GPIO cleaned up. Done!");
