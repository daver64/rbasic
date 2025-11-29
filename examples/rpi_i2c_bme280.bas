// Raspberry Pi I2C Example - Read from BME280 Temperature/Humidity/Pressure Sensor
// Demonstrates I2C communication on Raspberry Pi Zero 2W
//
// Hardware Setup:
// - BME280 sensor connected to I2C bus 1
// - VCC to 3.3V
// - GND to GND
// - SCL to GPIO 3 (pin 5)
// - SDA to GPIO 2 (pin 3)
//
// Note: Make sure I2C is enabled in raspi-config
// Note: This example only works on ARM platforms (Raspberry Pi)

var I2C_BUS = 1;
var BME280_ADDR = 0x76; // Default I2C address (some use 0x77)

// BME280 registers
var BME280_REG_CHIP_ID = 0xD0;
var BME280_REG_CTRL_MEAS = 0xF4;
var BME280_REG_TEMP_MSB = 0xFA;

// Open I2C bus
print("Opening I2C bus " + str(I2C_BUS) + "...");
var i2c = i2c_open(I2C_BUS);
if (i2c < 0) {
    print("Failed to open I2C bus!");
    print("Make sure I2C is enabled in raspi-config");
    exit(1);
}

// Set slave address
print("Setting I2C address to 0x" + hex(BME280_ADDR) + "...");
if (i2c_set_address(i2c, BME280_ADDR) == 0) {
    print("Failed to set I2C address!");
    print("Make sure BME280 is connected and address is correct");
    i2c_close(i2c);
    exit(1);
}

// Read chip ID to verify communication
print("Reading chip ID...");
var chip_id = i2c_read_reg(i2c, BME280_REG_CHIP_ID);
if (chip_id < 0) {
    print("Failed to read chip ID!");
    i2c_close(i2c);
    exit(1);
}

print("Chip ID: 0x" + hex(chip_id));
if (chip_id != 0x60) {
    print("Warning: Expected chip ID 0x60 for BME280");
    print("Got: 0x" + hex(chip_id));
}

// Configure sensor for normal mode
print("Configuring sensor...");
// 0x27 = 001 001 11 = oversampling x1 temp, x1 pressure, normal mode
if (i2c_write_reg(i2c, BME280_REG_CTRL_MEAS, 0x27) == 0) {
    print("Failed to configure sensor!");
    i2c_close(i2c);
    exit(1);
}

// Read temperature (simplified, without calibration)
print("\nReading raw temperature value...");
var temp_msb = i2c_read_reg(i2c, BME280_REG_TEMP_MSB);
var temp_lsb = i2c_read_reg(i2c, BME280_REG_TEMP_MSB + 1);
var temp_xlsb = i2c_read_reg(i2c, BME280_REG_TEMP_MSB + 2);

print("Raw temp MSB: 0x" + hex(temp_msb));
print("Raw temp LSB: 0x" + hex(temp_lsb));
print("Raw temp XLSB: 0x" + hex(temp_xlsb));

// Combine into 20-bit value
var raw_temp = (temp_msb * 4096) + (temp_lsb * 16) + (temp_xlsb / 16);
print("Raw temperature value: " + str(raw_temp));
print("\nNote: Full calibration required for accurate temperature reading");

// Cleanup
i2c_close(i2c);
print("\nI2C closed successfully");
