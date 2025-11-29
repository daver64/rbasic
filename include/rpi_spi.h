#ifndef RPI_SPI_H
#define RPI_SPI_H

#ifdef RPI_SUPPORT_ENABLED

#include <cstdint>
#include <string>
#include <vector>

namespace rbasic {
namespace rpi {

// SPI mode definitions
enum class SPIMode {
    MODE_0 = 0,  // CPOL=0, CPHA=0
    MODE_1 = 1,  // CPOL=0, CPHA=1
    MODE_2 = 2,  // CPOL=1, CPHA=0
    MODE_3 = 3   // CPOL=1, CPHA=1
};

// SPI bit order
enum class SPIBitOrder {
    MSB_FIRST = 0,
    LSB_FIRST = 1
};

// SPI class for managing SPI communication
class SPI {
public:
    SPI();
    ~SPI();

    // Open SPI device (bus 0 or 1, chip select 0 or 1)
    bool open(int bus, int cs);
    
    // Close SPI device
    void close();
    
    // Check if SPI is open
    bool isOpen() const { return fd_ >= 0; }
    
    // Configure SPI
    bool setMode(SPIMode mode);
    bool setBitsPerWord(uint8_t bits);
    bool setSpeed(uint32_t speed_hz);
    bool setBitOrder(SPIBitOrder order);
    
    // Data transfer
    bool transfer(const uint8_t* tx_data, uint8_t* rx_data, size_t length);
    bool write(const uint8_t* data, size_t length);
    std::vector<uint8_t> read(size_t length);
    
    // Get last error message
    std::string getLastError() const { return lastError_; }

private:
    int fd_;
    uint8_t mode_;
    uint8_t bits_per_word_;
    uint32_t speed_hz_;
    std::string lastError_;
    
    void setError(const std::string& error);
};

// Global SPI instances (up to 2 buses, 2 CS each = 4 total)
extern SPI* g_spi_devices[4];

// C-style interface functions for BASIC runtime
extern "C" {
    // Open SPI device (returns handle 0-3, or -1 on error)
    int spi_open(int bus, int cs);
    
    // Close SPI device
    void spi_close(int handle);
    
    // Configure SPI
    int spi_set_mode(int handle, int mode);
    int spi_set_speed(int handle, int speed_hz);
    int spi_set_bits(int handle, int bits);
    
    // Transfer data
    int spi_transfer(int handle, void* tx_data, void* rx_data, int length);
    int spi_write_byte(int handle, int byte);
    int spi_read_byte(int handle);
    
    // Get last error
    const char* spi_get_error(int handle);
}

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED

#endif // RPI_SPI_H
