// Raspberry Pi PWM Servo Control Example
// Demonstrates hardware PWM for servo motor control
//
// Hardware Setup:
// - Servo signal wire to GPIO 18 (hardware PWM0, pin 12)
// - Servo power (5V) to pin 2 or 4
// - Servo ground to any GND pin
//
// Servo angles: 0° = 2.5% duty, 90° = 7.5% duty, 180° = 12.5% duty
// PWM frequency: 50Hz (20ms period)

var PWM_CHANNEL = 0;  // PWM0 on GPIO 18
var PWM_FREQUENCY = 50;  // 50Hz for servos

print("=== Raspberry Pi Servo Control ===");
print("GPIO 18 (PWM0) will control servo position");
print("");

// Initialize PWM
print("Initializing PWM...");
if (pwm_init(PWM_CHANNEL) == 0) {
    print("Failed to initialize PWM!");
    print("Make sure PWM is not in use by another process");
    exit(1);
}

// Set frequency to 50Hz (servo standard)
print("Setting PWM frequency to 50Hz...");
if (pwm_set_frequency(PWM_CHANNEL, PWM_FREQUENCY) == 0) {
    print("Failed to set PWM frequency!");
    pwm_cleanup(PWM_CHANNEL);
    exit(1);
}

// Enable PWM
if (pwm_enable(PWM_CHANNEL) == 0) {
    print("Failed to enable PWM!");
    pwm_cleanup(PWM_CHANNEL);
    exit(1);
}

print("Servo ready!");
print("");

// Sweep servo through positions
print("Sweeping servo: 0° -> 90° -> 180° -> 90° -> 0°");
print("");

// 0 degrees (2.5% duty cycle)
print("Position: 0 degrees");
pwm_set_duty_cycle(PWM_CHANNEL, 2.5);
sleep(1000);

// 45 degrees (5% duty cycle)
print("Position: 45 degrees");
pwm_set_duty_cycle(PWM_CHANNEL, 5.0);
sleep(1000);

// 90 degrees (7.5% duty cycle)
print("Position: 90 degrees (center)");
pwm_set_duty_cycle(PWM_CHANNEL, 7.5);
sleep(1000);

// 135 degrees (10% duty cycle)
print("Position: 135 degrees");
pwm_set_duty_cycle(PWM_CHANNEL, 10.0);
sleep(1000);

// 180 degrees (12.5% duty cycle)
print("Position: 180 degrees");
pwm_set_duty_cycle(PWM_CHANNEL, 12.5);
sleep(1000);

// Back to center
print("Position: 90 degrees (center)");
pwm_set_duty_cycle(PWM_CHANNEL, 7.5);
sleep(1000);

// Back to 0
print("Position: 0 degrees");
pwm_set_duty_cycle(PWM_CHANNEL, 2.5);
sleep(1000);

// Cleanup
print("");
print("Disabling PWM...");
pwm_disable(PWM_CHANNEL);
pwm_cleanup(PWM_CHANNEL);
print("Done!");
