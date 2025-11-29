// Raspberry Pi SPI Communication Example
// Demonstrates SPI communication with MCP3008 ADC chip
//
// Hardware Setup:
// - MCP3008 VDD to 3.3V (pin 1)
// - MCP3008 VREF to 3.3V (pin 15)
// - MCP3008 AGND to GND (pin 14)
// - MCP3008 DGND to GND (pin 9)
// - MCP3008 CLK to GPIO 11 (SPI0 SCLK, pin 23)
// - MCP3008 DOUT to GPIO 9 (SPI0 MISO, pin 21)
// - MCP3008 DIN to GPIO 10 (SPI0 MOSI, pin 19)
// - MCP3008 CS to GPIO 8 (SPI0 CE0, pin 24)
//
// MCP3008: 8-channel 10-bit ADC, reads 0-1023 (0-3.3V)

var SPI_BUS = 0;
var SPI_CS = 0;
var SPI_SPEED = 1000000;  // 1 MHz

print("=== Raspberry Pi SPI ADC Reader ===");
print("Reading analog values from MCP3008");
print("");

// Open SPI device
print("Opening SPI device...");
var spi = spi_open(SPI_BUS, SPI_CS);
if (spi < 0) {
    print("Failed to open SPI device!");
    print("Make sure SPI is enabled: sudo raspi-config -> Interface Options -> SPI");
    exit(1);
}

// Set SPI speed
print("Setting SPI speed to 1 MHz...");
if (spi_set_speed(spi, SPI_SPEED) == 0) {
    print("Failed to set SPI speed!");
    spi_close(spi);
    exit(1);
}

print("MCP3008 ready!");
print("Reading from channel 0 (10 samples)...");
print("");

// Read 10 samples from channel 0
var i = 0;
for (i = 0; i < 10; i = i + 1) {
    // MCP3008 protocol: 3 bytes
    // Byte 1: Start bit (0x01)
    // Byte 2: Config (0x80 = channel 0, single-ended)
    // Byte 3: Don't care
    
    // Send start bit
    spi_write_byte(spi, 1);
    
    // Send config byte and read high bits
    var high = spi_write_byte(spi, 128);
    high = high and 3;  // Mask to 2 bits
    
    // Read low byte
    var low = spi_read_byte(spi);
    
    // Combine into 10-bit value
    var value = (high * 256) + low;
    
    // Convert to voltage (0-3.3V)
    var voltage = (value * 3.3) / 1023.0;
    
    print("Sample " + str(i + 1) + ": " + str(value) + " (" + str(voltage) + "V)");
    
    sleep(500);
}

// Cleanup
print("");
print("Closing SPI device...");
spi_close(spi);
print("Done!");
print("");
print("Try connecting a potentiometer to channel 0:");
print("  - Left pin to GND");
print("  - Middle pin to CH0 (pin 1 of MCP3008)");
print("  - Right pin to 3.3V");
