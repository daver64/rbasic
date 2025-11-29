#ifdef RPI_SUPPORT_ENABLED

#include "rpi_gpio.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>

namespace rbasic {
namespace rpi {

// Global GPIO instance
GPIO* g_gpio = nullptr;

GPIO::GPIO() 
    : gpio_map_(nullptr), mem_fd_(-1), initialized_(false), peri_base_(0) {
}

GPIO::~GPIO() {
    cleanup();
}

bool GPIO::detectPeripheralBase() {
    // Try to read from device tree
    std::ifstream ranges("/proc/device-tree/soc/ranges", std::ios::binary);
    if (ranges.is_open()) {
        // Skip the first 4 bytes (address cells)
        ranges.seekg(4);
        
        // Read the peripheral base address (big endian)
        uint32_t base = 0;
        ranges.read(reinterpret_cast<char*>(&base), sizeof(base));
        
        if (!ranges.fail()) {
            // Convert from big endian to host byte order
            peri_base_ = __builtin_bswap32(base);
            return true;
        }
    }
    
    // Fallback: try to detect based on /proc/cpuinfo
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("Hardware") != std::string::npos) {
            if (line.find("BCM2711") != std::string::npos) {
                peri_base_ = BCM2711_PERI_BASE; // Pi 4
                return true;
            } else if (line.find("BCM2837") != std::string::npos || 
                       line.find("BCM2836") != std::string::npos) {
                peri_base_ = BCM2836_PERI_BASE; // Pi 2/3/Zero 2W
                return true;
            } else if (line.find("BCM2835") != std::string::npos) {
                peri_base_ = BCM2835_PERI_BASE; // Pi Zero/1
                return true;
            }
        }
    }
    
    // Default to BCM2836 (most common for Pi Zero 2W)
    peri_base_ = BCM2836_PERI_BASE;
    return true;
}

bool GPIO::initialize() {
    if (initialized_) {
        return true;
    }
    
    // Detect peripheral base address
    if (!detectPeripheralBase()) {
        setError("Failed to detect peripheral base address");
        return false;
    }
    
    // Open /dev/gpiomem for direct register access (no root required)
    mem_fd_ = open("/dev/gpiomem", O_RDWR | O_SYNC);
    if (mem_fd_ < 0) {
        // Fallback to /dev/mem (requires root)
        mem_fd_ = open("/dev/mem", O_RDWR | O_SYNC);
        if (mem_fd_ < 0) {
            setError("Failed to open /dev/gpiomem or /dev/mem. "
                    "Make sure user is in 'gpio' group or run as root");
            return false;
        }
    }
    
    // Map GPIO registers into virtual memory
    void* gpio_map = mmap(
        nullptr,                    // Let kernel choose address
        BLOCK_SIZE,                 // Map one page
        PROT_READ | PROT_WRITE,    // Enable read/write
        MAP_SHARED,                 // Share with other processes
        mem_fd_,                    // File descriptor
        peri_base_ + GPIO_OFFSET    // Offset to GPIO registers
    );
    
    if (gpio_map == MAP_FAILED) {
        close(mem_fd_);
        mem_fd_ = -1;
        setError("Failed to map GPIO memory");
        return false;
    }
    
    gpio_map_ = static_cast<volatile uint32_t*>(gpio_map);
    initialized_ = true;
    
    return true;
}

void GPIO::cleanup() {
    if (gpio_map_ != nullptr) {
        munmap(const_cast<uint32_t*>(gpio_map_), BLOCK_SIZE);
        gpio_map_ = nullptr;
    }
    
    if (mem_fd_ >= 0) {
        close(mem_fd_);
        mem_fd_ = -1;
    }
    
    initialized_ = false;
}

bool GPIO::validatePin(int pin) {
    if (pin < 0 || pin > 53) {
        setError("Invalid pin number: " + std::to_string(pin) + 
                 " (valid range: 0-53)");
        return false;
    }
    return true;
}

void GPIO::setError(const std::string& error) {
    lastError_ = error;
    std::cerr << "GPIO Error: " << error << std::endl;
}

bool GPIO::setPinMode(int pin, PinMode mode) {
    if (!initialized_) {
        setError("GPIO not initialized");
        return false;
    }
    
    if (!validatePin(pin)) {
        return false;
    }
    
    // Each GPFSEL register controls 10 pins (3 bits per pin)
    int reg_index = pin / 10;
    int bit_offset = (pin % 10) * 3;
    
    // Read current value
    uint32_t value = gpio_map_[GPFSEL0 + reg_index];
    
    // Clear the 3 bits for this pin
    value &= ~(0x7 << bit_offset);
    
    // Set new mode
    value |= (static_cast<uint32_t>(mode) << bit_offset);
    
    // Write back
    gpio_map_[GPFSEL0 + reg_index] = value;
    
    return true;
}

bool GPIO::setPullMode(int pin, PullMode mode) {
    if (!initialized_) {
        setError("GPIO not initialized");
        return false;
    }
    
    if (!validatePin(pin)) {
        return false;
    }
    
    // BCM2835/BCM2837 pull up/down procedure:
    // 1. Write to GPPUD to set required control signal
    gpio_map_[GPPUD] = static_cast<uint32_t>(mode);
    
    // 2. Wait 150 cycles (datasheet requirement)
    usleep(1);
    
    // 3. Write to GPPUDCLK to clock the control signal into the pin
    int reg_index = pin / 32;
    int bit_offset = pin % 32;
    gpio_map_[GPPUDCLK0 + reg_index] = (1 << bit_offset);
    
    // 4. Wait 150 cycles
    usleep(1);
    
    // 5. Remove control signal and clock
    gpio_map_[GPPUD] = 0;
    gpio_map_[GPPUDCLK0 + reg_index] = 0;
    
    return true;
}

bool GPIO::digitalWrite(int pin, bool value) {
    if (!initialized_) {
        setError("GPIO not initialized");
        return false;
    }
    
    if (!validatePin(pin)) {
        return false;
    }
    
    int reg_index = pin / 32;
    int bit_offset = pin % 32;
    
    if (value) {
        // Set pin high
        gpio_map_[GPSET0 + reg_index] = (1 << bit_offset);
    } else {
        // Set pin low
        gpio_map_[GPCLR0 + reg_index] = (1 << bit_offset);
    }
    
    return true;
}

int GPIO::digitalRead(int pin) {
    if (!initialized_) {
        setError("GPIO not initialized");
        return -1;
    }
    
    if (!validatePin(pin)) {
        return -1;
    }
    
    int reg_index = pin / 32;
    int bit_offset = pin % 32;
    
    uint32_t value = gpio_map_[GPLEV0 + reg_index];
    
    return (value & (1 << bit_offset)) ? 1 : 0;
}

// C-style interface implementation
extern "C" {

int gpio_init() {
    if (g_gpio == nullptr) {
        g_gpio = new GPIO();
    }
    
    if (!g_gpio->initialize()) {
        return 0; // failure
    }
    
    return 1; // success
}

void gpio_cleanup() {
    if (g_gpio != nullptr) {
        g_gpio->cleanup();
        delete g_gpio;
        g_gpio = nullptr;
    }
}

int gpio_set_mode(int pin, int mode) {
    if (g_gpio == nullptr || !g_gpio->isInitialized()) {
        return 0;
    }
    
    // Map integer to PinMode enum
    PinMode pin_mode;
    switch (mode) {
        case 0: pin_mode = PinMode::INPUT; break;
        case 1: pin_mode = PinMode::OUTPUT; break;
        case 2: pin_mode = PinMode::ALT5; break;
        case 3: pin_mode = PinMode::ALT4; break;
        case 4: pin_mode = PinMode::ALT0; break;
        case 5: pin_mode = PinMode::ALT1; break;
        case 6: pin_mode = PinMode::ALT2; break;
        case 7: pin_mode = PinMode::ALT3; break;
        default: return 0;
    }
    
    return g_gpio->setPinMode(pin, pin_mode) ? 1 : 0;
}

int gpio_set_pull(int pin, int pull) {
    if (g_gpio == nullptr || !g_gpio->isInitialized()) {
        return 0;
    }
    
    // Map integer to PullMode enum
    PullMode pull_mode;
    switch (pull) {
        case 0: pull_mode = PullMode::OFF; break;
        case 1: pull_mode = PullMode::DOWN; break;
        case 2: pull_mode = PullMode::UP; break;
        default: return 0;
    }
    
    return g_gpio->setPullMode(pin, pull_mode) ? 1 : 0;
}

int gpio_write(int pin, int value) {
    if (g_gpio == nullptr || !g_gpio->isInitialized()) {
        return 0;
    }
    
    return g_gpio->digitalWrite(pin, value != 0) ? 1 : 0;
}

int gpio_read(int pin) {
    if (g_gpio == nullptr || !g_gpio->isInitialized()) {
        return -1;
    }
    
    return g_gpio->digitalRead(pin);
}

const char* gpio_get_error() {
    if (g_gpio == nullptr) {
        return "GPIO not initialized";
    }
    
    static std::string error;
    error = g_gpio->getLastError();
    return error.c_str();
}

} // extern "C"

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED
