// Raspberry Pi ACT LED Blink Example
// Controls the green Activity LED on the board
//
// Note: This requires root privileges or proper permissions
// Run with: sudo ./rbasic -i examples/rpi_act_led_blink.bas

print("=== Raspberry Pi ACT LED Control ===");
print("This will blink the green Activity LED on your Pi");
print("");

// LED control paths (different for Pi models)
var led_trigger = "/sys/class/leds/ACT/trigger";
var led_brightness = "/sys/class/leds/ACT/brightness";

// Alternative path for some Pi models
var led_trigger_alt = "/sys/class/leds/led0/trigger";
var led_brightness_alt = "/sys/class/leds/led0/brightness";

// Disable default trigger (set to 'none')
print("Disabling default LED trigger...");
write_text_file(led_trigger, "none");
print("LED trigger set to manual control");
print("");

// Blink 10 times
print("Blinking ACT LED 10 times...");
var i = 0;
for (i = 0; i < 10; i = i + 1) {
    print("Blink " + str(i + 1));
    
    // Turn LED on (brightness 1)
    write_text_file(led_brightness, "1");
    sleep(500);
    
    // Turn LED off (brightness 0)
    write_text_file(led_brightness, "0");
    sleep(500);
}

// Restore default trigger
print("");
print("Restoring default LED trigger...");
write_text_file(led_trigger, "mmc0");
print("Done!");
