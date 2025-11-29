#ifndef RPI_I2C_H
#define RPI_I2C_H

#ifdef RPI_SUPPORT_ENABLED

#include <cstdint>
#include <string>
#include <vector>

namespace rbasic {
namespace rpi {

// I2C class for managing I2C communication
class I2C {
public:
    I2C();
    ~I2C();

    // Open I2C bus (bus 0 or 1)
    bool open(int bus);
    
    // Close I2C bus
    void close();
    
    // Check if I2C is open
    bool isOpen() const { return fd_ >= 0; }
    
    // Set slave address
    bool setSlaveAddress(uint8_t address);
    
    // Read/write operations
    bool write(const uint8_t* data, size_t length);
    bool read(uint8_t* data, size_t length);
    bool writeByte(uint8_t value);
    int readByte();
    
    // Register operations (write register address, then read/write data)
    bool writeReg(uint8_t reg, uint8_t value);
    int readReg(uint8_t reg);
    bool writeBlockData(uint8_t reg, const uint8_t* data, size_t length);
    bool readBlockData(uint8_t reg, uint8_t* data, size_t length);
    
    // Get last error message
    std::string getLastError() const { return lastError_; }

private:
    int fd_;
    uint8_t address_;
    std::string lastError_;
    
    void setError(const std::string& error);
};

// Global I2C instances (up to 2 buses)
extern I2C* g_i2c_devices[2];

// C-style interface functions for BASIC runtime
extern "C" {
    // Open I2C bus (returns handle 0-1, or -1 on error)
    int i2c_open(int bus);
    
    // Close I2C bus
    void i2c_close(int handle);
    
    // Set slave address
    int i2c_set_address(int handle, int address);
    
    // Read/write bytes
    int i2c_write_byte(int handle, int value);
    int i2c_read_byte(int handle);
    
    // Register operations
    int i2c_write_reg(int handle, int reg, int value);
    int i2c_read_reg(int handle, int reg);
    
    // Block operations
    int i2c_write_block(int handle, int reg, void* data, int length);
    int i2c_read_block(int handle, int reg, void* data, int length);
    
    // Get last error
    const char* i2c_get_error(int handle);
}

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED

#endif // RPI_I2C_H
