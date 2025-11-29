#ifdef RPI_SUPPORT_ENABLED

#include "rpi_i2c.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <iostream>

// Compatibility for older kernels
#ifndef I2C_SMBUS_BLOCK_MAX
#define I2C_SMBUS_BLOCK_MAX 32
#endif

namespace rbasic {
namespace rpi {

// Global I2C device instances
I2C* g_i2c_devices[2] = {nullptr, nullptr};

I2C::I2C() 
    : fd_(-1), address_(0) {
}

I2C::~I2C() {
    close();
}

void I2C::setError(const std::string& error) {
    lastError_ = error;
    std::cerr << "I2C Error: " << error << std::endl;
}

bool I2C::open(int bus) {
    if (fd_ >= 0) {
        close();
    }
    
    // Validate bus number
    if (bus < 0 || bus > 1) {
        setError("Invalid I2C bus: " + std::to_string(bus) + " (valid: 0-1)");
        return false;
    }
    
    // Construct device path
    std::string device = "/dev/i2c-" + std::to_string(bus);
    
    // Open device
    fd_ = ::open(device.c_str(), O_RDWR);
    if (fd_ < 0) {
        setError("Failed to open " + device + ". Make sure I2C is enabled in raspi-config");
        return false;
    }
    
    return true;
}

void I2C::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    address_ = 0;
}

bool I2C::setSlaveAddress(uint8_t address) {
    if (fd_ < 0) {
        setError("I2C device not open");
        return false;
    }
    
    if (address > 0x7F) {
        setError("Invalid I2C address: 0x" + std::to_string(address) + " (valid: 0x00-0x7F)");
        return false;
    }
    
    if (ioctl(fd_, I2C_SLAVE, address) < 0) {
        setError("Failed to set I2C slave address to 0x" + std::to_string(address));
        return false;
    }
    
    address_ = address;
    return true;
}

bool I2C::write(const uint8_t* data, size_t length) {
    if (fd_ < 0) {
        setError("I2C device not open");
        return false;
    }
    
    if (address_ == 0) {
        setError("I2C slave address not set");
        return false;
    }
    
    if (length == 0) {
        return true;
    }
    
    ssize_t written = ::write(fd_, data, length);
    if (written != static_cast<ssize_t>(length)) {
        setError("I2C write failed");
        return false;
    }
    
    return true;
}

bool I2C::read(uint8_t* data, size_t length) {
    if (fd_ < 0) {
        setError("I2C device not open");
        return false;
    }
    
    if (address_ == 0) {
        setError("I2C slave address not set");
        return false;
    }
    
    if (length == 0) {
        return true;
    }
    
    ssize_t bytes_read = ::read(fd_, data, length);
    if (bytes_read != static_cast<ssize_t>(length)) {
        setError("I2C read failed");
        return false;
    }
    
    return true;
}

bool I2C::writeByte(uint8_t value) {
    return write(&value, 1);
}

int I2C::readByte() {
    uint8_t value = 0;
    if (!read(&value, 1)) {
        return -1;
    }
    return value;
}

bool I2C::writeReg(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return write(data, 2);
}

int I2C::readReg(uint8_t reg) {
    // Write register address
    if (!write(&reg, 1)) {
        return -1;
    }
    
    // Read value
    uint8_t value = 0;
    if (!read(&value, 1)) {
        return -1;
    }
    
    return value;
}

bool I2C::writeBlockData(uint8_t reg, const uint8_t* data, size_t length) {
    if (length > I2C_SMBUS_BLOCK_MAX) {
        setError("I2C block write too large (max " + 
                std::to_string(I2C_SMBUS_BLOCK_MAX) + " bytes)");
        return false;
    }
    
    // Create buffer with register address + data
    std::vector<uint8_t> buffer(length + 1);
    buffer[0] = reg;
    memcpy(&buffer[1], data, length);
    
    return write(buffer.data(), buffer.size());
}

bool I2C::readBlockData(uint8_t reg, uint8_t* data, size_t length) {
    if (length > I2C_SMBUS_BLOCK_MAX) {
        setError("I2C block read too large (max " + 
                std::to_string(I2C_SMBUS_BLOCK_MAX) + " bytes)");
        return false;
    }
    
    // Write register address
    if (!write(&reg, 1)) {
        return false;
    }
    
    // Read data
    return read(data, length);
}

// C-style interface implementation
extern "C" {

int i2c_open(int bus) {
    if (bus < 0 || bus >= 2) {
        return -1;
    }
    
    // Create I2C instance if needed
    if (g_i2c_devices[bus] == nullptr) {
        g_i2c_devices[bus] = new I2C();
    }
    
    if (!g_i2c_devices[bus]->open(bus)) {
        return -1;
    }
    
    return bus;
}

void i2c_close(int handle) {
    if (handle < 0 || handle >= 2) {
        return;
    }
    
    if (g_i2c_devices[handle] != nullptr) {
        g_i2c_devices[handle]->close();
        delete g_i2c_devices[handle];
        g_i2c_devices[handle] = nullptr;
    }
}

int i2c_set_address(int handle, int address) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_i2c_devices[handle]->setSlaveAddress(address) ? 1 : 0;
}

int i2c_write_byte(int handle, int value) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_i2c_devices[handle]->writeByte(value) ? 1 : 0;
}

int i2c_read_byte(int handle) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return -1;
    }
    
    return g_i2c_devices[handle]->readByte();
}

int i2c_write_reg(int handle, int reg, int value) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_i2c_devices[handle]->writeReg(reg, value) ? 1 : 0;
}

int i2c_read_reg(int handle, int reg) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return -1;
    }
    
    return g_i2c_devices[handle]->readReg(reg);
}

int i2c_write_block(int handle, int reg, void* data, int length) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_i2c_devices[handle]->writeBlockData(
        reg,
        static_cast<uint8_t*>(data),
        length
    ) ? 1 : 0;
}

int i2c_read_block(int handle, int reg, void* data, int length) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_i2c_devices[handle]->readBlockData(
        reg,
        static_cast<uint8_t*>(data),
        length
    ) ? 1 : 0;
}

const char* i2c_get_error(int handle) {
    if (handle < 0 || handle >= 2 || g_i2c_devices[handle] == nullptr) {
        return "Invalid I2C handle";
    }
    
    static std::string error;
    error = g_i2c_devices[handle]->getLastError();
    return error.c_str();
}

} // extern "C"

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED
