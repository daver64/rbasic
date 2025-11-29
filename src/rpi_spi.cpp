#ifdef RPI_SUPPORT_ENABLED

#include "rpi_spi.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <iostream>

namespace rbasic {
namespace rpi {

// Global SPI device instances
SPI* g_spi_devices[4] = {nullptr, nullptr, nullptr, nullptr};

SPI::SPI() 
    : fd_(-1), mode_(0), bits_per_word_(8), speed_hz_(1000000) {
}

SPI::~SPI() {
    close();
}

void SPI::setError(const std::string& error) {
    lastError_ = error;
    std::cerr << "SPI Error: " << error << std::endl;
}

bool SPI::open(int bus, int cs) {
    if (fd_ >= 0) {
        close();
    }
    
    // Validate parameters
    if (bus < 0 || bus > 1) {
        setError("Invalid SPI bus: " + std::to_string(bus) + " (valid: 0-1)");
        return false;
    }
    
    if (cs < 0 || cs > 1) {
        setError("Invalid chip select: " + std::to_string(cs) + " (valid: 0-1)");
        return false;
    }
    
    // Construct device path
    std::string device = "/dev/spidev" + std::to_string(bus) + "." + std::to_string(cs);
    
    // Open device
    fd_ = ::open(device.c_str(), O_RDWR);
    if (fd_ < 0) {
        setError("Failed to open " + device + ". Make sure SPI is enabled in raspi-config");
        return false;
    }
    
    // Set default configuration
    if (!setMode(SPIMode::MODE_0)) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }
    
    if (!setBitsPerWord(8)) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }
    
    if (!setSpeed(1000000)) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }
    
    return true;
}

void SPI::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool SPI::setMode(SPIMode mode) {
    if (fd_ < 0) {
        setError("SPI device not open");
        return false;
    }
    
    mode_ = static_cast<uint8_t>(mode);
    
    if (ioctl(fd_, SPI_IOC_WR_MODE, &mode_) < 0) {
        setError("Failed to set SPI mode");
        return false;
    }
    
    return true;
}

bool SPI::setBitsPerWord(uint8_t bits) {
    if (fd_ < 0) {
        setError("SPI device not open");
        return false;
    }
    
    if (bits != 8 && bits != 16) {
        setError("Invalid bits per word: " + std::to_string(bits) + " (valid: 8, 16)");
        return false;
    }
    
    bits_per_word_ = bits;
    
    if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word_) < 0) {
        setError("Failed to set bits per word");
        return false;
    }
    
    return true;
}

bool SPI::setSpeed(uint32_t speed_hz) {
    if (fd_ < 0) {
        setError("SPI device not open");
        return false;
    }
    
    if (speed_hz == 0 || speed_hz > 125000000) {
        setError("Invalid speed: " + std::to_string(speed_hz) + " (valid: 1-125000000 Hz)");
        return false;
    }
    
    speed_hz_ = speed_hz;
    
    if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz_) < 0) {
        setError("Failed to set speed");
        return false;
    }
    
    return true;
}

bool SPI::setBitOrder(SPIBitOrder order) {
    if (fd_ < 0) {
        setError("SPI device not open");
        return false;
    }
    
    uint8_t lsb = static_cast<uint8_t>(order);
    
    if (ioctl(fd_, SPI_IOC_WR_LSB_FIRST, &lsb) < 0) {
        setError("Failed to set bit order");
        return false;
    }
    
    return true;
}

bool SPI::transfer(const uint8_t* tx_data, uint8_t* rx_data, size_t length) {
    if (fd_ < 0) {
        setError("SPI device not open");
        return false;
    }
    
    if (length == 0) {
        return true;
    }
    
    struct spi_ioc_transfer transfer;
    memset(&transfer, 0, sizeof(transfer));
    
    transfer.tx_buf = reinterpret_cast<uint64_t>(tx_data);
    transfer.rx_buf = reinterpret_cast<uint64_t>(rx_data);
    transfer.len = length;
    transfer.speed_hz = speed_hz_;
    transfer.bits_per_word = bits_per_word_;
    
    if (ioctl(fd_, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        setError("SPI transfer failed");
        return false;
    }
    
    return true;
}

bool SPI::write(const uint8_t* data, size_t length) {
    return transfer(data, nullptr, length);
}

std::vector<uint8_t> SPI::read(size_t length) {
    std::vector<uint8_t> data(length);
    
    if (!transfer(nullptr, data.data(), length)) {
        return std::vector<uint8_t>();
    }
    
    return data;
}

// C-style interface implementation
extern "C" {

int spi_open(int bus, int cs) {
    // Calculate handle
    int handle = bus * 2 + cs;
    
    if (handle < 0 || handle >= 4) {
        return -1;
    }
    
    // Create SPI instance if needed
    if (g_spi_devices[handle] == nullptr) {
        g_spi_devices[handle] = new SPI();
    }
    
    if (!g_spi_devices[handle]->open(bus, cs)) {
        return -1;
    }
    
    return handle;
}

void spi_close(int handle) {
    if (handle < 0 || handle >= 4) {
        return;
    }
    
    if (g_spi_devices[handle] != nullptr) {
        g_spi_devices[handle]->close();
        delete g_spi_devices[handle];
        g_spi_devices[handle] = nullptr;
    }
}

int spi_set_mode(int handle, int mode) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return 0;
    }
    
    SPIMode spi_mode;
    switch (mode) {
        case 0: spi_mode = SPIMode::MODE_0; break;
        case 1: spi_mode = SPIMode::MODE_1; break;
        case 2: spi_mode = SPIMode::MODE_2; break;
        case 3: spi_mode = SPIMode::MODE_3; break;
        default: return 0;
    }
    
    return g_spi_devices[handle]->setMode(spi_mode) ? 1 : 0;
}

int spi_set_speed(int handle, int speed_hz) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_spi_devices[handle]->setSpeed(speed_hz) ? 1 : 0;
}

int spi_set_bits(int handle, int bits) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_spi_devices[handle]->setBitsPerWord(bits) ? 1 : 0;
}

int spi_transfer(int handle, void* tx_data, void* rx_data, int length) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return 0;
    }
    
    return g_spi_devices[handle]->transfer(
        static_cast<uint8_t*>(tx_data),
        static_cast<uint8_t*>(rx_data),
        length
    ) ? 1 : 0;
}

int spi_write_byte(int handle, int byte) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return 0;
    }
    
    uint8_t data = static_cast<uint8_t>(byte);
    return g_spi_devices[handle]->write(&data, 1) ? 1 : 0;
}

int spi_read_byte(int handle) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return -1;
    }
    
    uint8_t data = 0;
    if (!g_spi_devices[handle]->transfer(nullptr, &data, 1)) {
        return -1;
    }
    
    return data;
}

const char* spi_get_error(int handle) {
    if (handle < 0 || handle >= 4 || g_spi_devices[handle] == nullptr) {
        return "Invalid SPI handle";
    }
    
    static std::string error;
    error = g_spi_devices[handle]->getLastError();
    return error.c_str();
}

} // extern "C"

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED
