# Raspberry Pi Hardware Support

rbasic is designed for Raspberry Pi and single-board computer development, providing native hardware access without external libraries. Control GPIO, SPI, I2C, PWM, and serial interfaces directly from BASIC code.

## Why rbasic for Raspberry Pi?

- ⚡ **Native Performance**: Compile to ARM executables that run at near-C speeds
- 🔧 **No Dependencies**: Direct hardware access via `/dev` interfaces, no pigpio or WiringPi needed
- 🎮 **Interactive Development**: Test hardware in real-time with REPL mode
- 📝 **Simple Syntax**: BASIC's clarity with modern C-style control structures
- 🚀 **Rapid Prototyping**: Interpret mode for quick hardware testing, compile for deployment
- 💾 **Small Footprint**: Minimal memory usage, perfect for Pi Zero and embedded systems

## Supported Hardware

- **Raspberry Pi Zero 2W** (BCM2837, ARM Cortex-A53, 64-bit)
- **Raspberry Pi 3** (BCM2837)
- **Raspberry Pi 4** (BCM2711)
- **Raspberry Pi 5** (BCM2712)
- Compatible ARM single-board computers

## Complete Examples

### 📍 GPIO Examples
- **`rpi_gpio_blink.bas`** - Basic LED control (start here!)
- **`rpi_button_led.bas`** - Interactive button input with debouncing
- **`rpi_act_led_blink.bas`** - Control the green Activity LED on the board
- **`rpi_distance_sensor.bas`** - HC-SR04 ultrasonic sensor

### 🎛️ PWM Examples
- **`rpi_pwm_servo.bas`** - Servo motor control (0-180 degrees with hardware PWM)

### 📡 SPI Examples
- **`rpi_spi_sensor.bas`** - MCP3008 8-channel ADC (analog input)

### 🔌 I2C Examples
- **`rpi_i2c_bme280.bas`** - BME280 temperature/pressure/humidity sensor

### 📊 System Integration
- **`rpi_temperature_logger.bas`** - CPU temperature monitoring with file logging

All examples work in both **interpreter mode** (fast testing) and **compiled mode** (production deployment).

## Platform Detection

The build system automatically detects ARM architecture:
```bash
# Native build on Raspberry Pi
cd /home/dave/Documents/Projects/rbasic/build
cmake ..
make

# Cross-compilation from x86/x64
cmake -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake ..
make
```

On x86/x64 systems, RPI functions are **not available** and programs will compile without them.

## GPIO Functions

### Initialization
```basic
// Initialize GPIO system (required before using GPIO)
var result = gpio_init();
if (result == 0) {
    print("Failed to initialize GPIO!");
}

// Cleanup when done
gpio_cleanup();
```

### Pin Configuration
```basic
// Pin modes (second parameter to gpio_set_mode)
var GPIO_INPUT = 0;
var GPIO_OUTPUT = 1;
var GPIO_ALT0 = 4;
var GPIO_ALT1 = 5;
var GPIO_ALT2 = 6;
var GPIO_ALT3 = 7;
var GPIO_ALT4 = 3;
var GPIO_ALT5 = 2;

// Configure pin 17 as output
gpio_set_mode(17, GPIO_OUTPUT);

// Pull-up/down modes (second parameter to gpio_set_pull)
var GPIO_PULL_OFF = 0;
var GPIO_PULL_DOWN = 1;
var GPIO_PULL_UP = 2;

// Enable pull-up on pin 4
gpio_set_pull(4, GPIO_PULL_UP);
```

### Digital I/O
```basic
// Write to output pin
gpio_write(17, 1); // Set high
gpio_write(17, 0); // Set low

// Read from input pin
var value = gpio_read(4);
if (value == 1) {
    print("Pin is HIGH");
} else {
    print("Pin is LOW");
}
```

### Example: LED Blink
```basic
var LED_PIN = 17;
gpio_init();
gpio_set_mode(LED_PIN, 1); // OUTPUT

var i = 0;
for (i = 0; i < 10; i = i + 1) {
    gpio_write(LED_PIN, 1);
    sleep(500);
    gpio_write(LED_PIN, 0);
    sleep(500);
}

gpio_cleanup();
```

## SPI Functions

SPI (Serial Peripheral Interface) for high-speed communication with sensors, displays, ADCs, etc.

### Opening SPI Device
```basic
// Open SPI bus 0, chip select 0
var spi = spi_open(0, 0);
if (spi < 0) {
    print("Failed to open SPI!");
}

// Close when done
spi_close(spi);
```

### Configuration
```basic
// Set SPI mode (0-3)
spi_set_mode(spi, 0); // Mode 0: CPOL=0, CPHA=0

// Set speed (Hz)
spi_set_speed(spi, 1000000); // 1 MHz

// Set bits per word
spi_set_bits(spi, 8); // 8 bits
```

### Data Transfer
```basic
// Write single byte
spi_write_byte(spi, 0xAA);

// Read single byte
var data = spi_read_byte(spi);
print("Read: 0x" + hex(data));
```

## I2C Functions

I2C (Inter-Integrated Circuit) for communication with sensors and peripherals.

### Opening I2C Bus
```basic
// Open I2C bus 1 (default on Raspberry Pi)
var i2c = i2c_open(1);
if (i2c < 0) {
    print("Failed to open I2C!");
}

// Close when done
i2c_close(i2c);
```

### Setting Slave Address
```basic
// Set 7-bit I2C address
i2c_set_address(i2c, 0x76); // BME280 sensor
```

### Basic I/O
```basic
// Write single byte
i2c_write_byte(i2c, 0x42);

// Read single byte
var data = i2c_read_byte(i2c);
```

### Register Access
```basic
// Write to register
i2c_write_reg(i2c, 0xF4, 0x27); // Write 0x27 to register 0xF4

// Read from register
var value = i2c_read_reg(i2c, 0xD0); // Read register 0xD0
print("Chip ID: 0x" + hex(value));
```

### Example: BME280 Sensor
```basic
var i2c = i2c_open(1);
i2c_set_address(i2c, 0x76);

// Read chip ID
var chip_id = i2c_read_reg(i2c, 0xD0);
print("BME280 Chip ID: 0x" + hex(chip_id));

// Configure sensor
i2c_write_reg(i2c, 0xF4, 0x27);

i2c_close(i2c);
```

## PWM Functions

Hardware PWM for servo control, LED dimming, motor control, etc.

### Initialization
```basic
// Initialize PWM channel (0 or 1)
var result = pwm_init(0);
if (result == 0) {
    print("Failed to initialize PWM!");
}

// Cleanup when done
pwm_cleanup(0);
```

### Configuration
```basic
// Set frequency (Hz)
pwm_set_frequency(0, 50); // 50 Hz for servo control

// Set duty cycle (0.0 - 100.0%)
pwm_set_duty_cycle(0, 50.0); // 50% duty cycle
```

### Control
```basic
// Enable PWM output
pwm_enable(0);

// Disable PWM output
pwm_disable(0);
```

### Example: Servo Control
```basic
pwm_init(0);
pwm_set_frequency(0, 50); // 50 Hz for servos

// Move servo to center position (1.5ms pulse)
pwm_set_duty_cycle(0, 7.5); // 1.5ms / 20ms = 7.5%
pwm_enable(0);

sleep(2000);

// Move to left position (1.0ms pulse)
pwm_set_duty_cycle(0, 5.0); // 1.0ms / 20ms = 5.0%
sleep(2000);

// Move to right position (2.0ms pulse)
pwm_set_duty_cycle(0, 10.0); // 2.0ms / 20ms = 10.0%
sleep(2000);

pwm_cleanup(0);
```

## Serial Functions

UART serial communication for GPS, sensors, Arduino, etc.

### Opening Serial Port
```basic
// Open serial port
var serial = serial_open("/dev/serial0");
if (serial < 0) {
    print("Failed to open serial port!");
}

// Close when done
serial_close(serial);
```

### Configuration
```basic
// Set baud rate
serial_set_baud(serial, 115200);

// Set configuration (data bits, parity, stop bits)
// Parity: 0=none, 1=odd, 2=even
serial_set_config(serial, 8, 0, 1); // 8N1
```

### Data Transfer
```basic
// Write byte
serial_write_byte(serial, 0x42);

// Read byte
var data = serial_read_byte(serial);

// Write string
serial_write_string(serial, "Hello, Arduino!\n");

// Check available data
var available = serial_available(serial);
print("Bytes available: " + str(available));
```

### Example: Serial Echo
```basic
var serial = serial_open("/dev/serial0");
serial_set_baud(serial, 115200);

serial_write_string(serial, "Ready!\n");

while (1) {
    if (serial_available(serial) > 0) {
        var byte = serial_read_byte(serial);
        serial_write_byte(serial, byte); // Echo back
    }
    sleep(10);
}

serial_close(serial);
```

## Hardware Setup Notes

### GPIO Pins
- User must be in `gpio` group for /dev/gpiomem access
- No root required when using /dev/gpiomem
- Valid pin numbers: 0-53 (board-dependent)

### SPI
- Enable in raspi-config: `Interface Options → SPI`
- Devices: /dev/spidev0.0, /dev/spidev0.1
- Maximum speed: 125 MHz (practical: 1-50 MHz)

### I2C
- Enable in raspi-config: `Interface Options → I2C`
- Devices: /dev/i2c-0, /dev/i2c-1
- Bus 1 is standard for user applications
- Pull-up resistors required (1.8kΩ - 10kΩ)

### PWM
- Hardware PWM channels: 0 and 1
- GPIO 18 (channel 0), GPIO 19 (channel 1)
- Accessed via /sys/class/pwm/pwmchip0
- Must enable PWM in device tree

### Serial
- Default: /dev/serial0 → GPIO 14 (TXD), GPIO 15 (RXD)
- Disable serial console in raspi-config
- 3.3V logic levels (DO NOT connect 5V devices directly)

## Cross-Compilation

To build for Raspberry Pi from an x86/x64 system:

```bash
# Install cross-compiler
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Set up sysroot (copy from target Pi)
mkdir -p ~/pi-sysroot
rsync -avz pi@raspberrypi.local:/{lib,usr} ~/pi-sysroot/

# Build with toolchain file
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../pi-toolchain.cmake ..
make

# Copy to Raspberry Pi
scp rbasic pi@raspberrypi.local:~/
```

## Error Handling

All functions return status codes:
- **Success**: 1 (or positive value)
- **Failure**: 0 or -1

Check return values:
```basic
if (gpio_init() == 0) {
    print("GPIO initialization failed!");
    exit(1);
}
```

## Examples

See the `examples/` directory:
- `rpi_gpio_blink.bas` - LED blink example
- `rpi_i2c_bme280.bas` - I2C sensor reading

## Limitations

- Hardware functions only available on ARM platforms
- Root or gpio group membership required
- Hardware access requires enabled interfaces (raspi-config)
- Real-time constraints depend on Linux kernel scheduler

## Safety Notes

- **Never exceed 3.3V** on GPIO pins (5V will damage the Pi)
- Use current-limiting resistors for LEDs (330Ω typical)
- Add flyback diodes when controlling motors/relays
- Maximum GPIO current: 16mA per pin, 50mA total
- Use level shifters for 5V devices
- Disconnect hardware before reflashing SD card
