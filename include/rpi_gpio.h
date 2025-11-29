#ifndef RPI_GPIO_H
#define RPI_GPIO_H

#ifdef RPI_SUPPORT_ENABLED

#include <cstdint>
#include <string>

namespace rbasic {
namespace rpi {

// GPIO pin modes
enum class PinMode {
    INPUT = 0,
    OUTPUT = 1,
    ALT0 = 4,
    ALT1 = 5,
    ALT2 = 6,
    ALT3 = 7,
    ALT4 = 3,
    ALT5 = 2
};

// GPIO pull up/down modes
enum class PullMode {
    OFF = 0,
    DOWN = 1,
    UP = 2
};

// BCM2835/BCM2837 Peripheral Base Addresses
// These will be detected at runtime based on /proc/device-tree/soc/ranges
constexpr uint32_t BCM2835_PERI_BASE = 0x20000000;  // Pi Zero/1
constexpr uint32_t BCM2836_PERI_BASE = 0x3F000000;  // Pi 2/3
constexpr uint32_t BCM2711_PERI_BASE = 0xFE000000;  // Pi 4

constexpr uint32_t GPIO_OFFSET = 0x200000;
constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t BLOCK_SIZE = 4096;

// GPIO register offsets
constexpr uint32_t GPFSEL0 = 0;    // Function select
constexpr uint32_t GPSET0 = 7;     // Pin output set
constexpr uint32_t GPCLR0 = 10;    // Pin output clear
constexpr uint32_t GPLEV0 = 13;    // Pin level
constexpr uint32_t GPPUD = 37;     // Pull up/down enable
constexpr uint32_t GPPUDCLK0 = 38; // Pull up/down clock

// GPIO class for managing hardware access
class GPIO {
public:
    GPIO();
    ~GPIO();

    // Initialize GPIO access
    bool initialize();
    
    // Cleanup and unmap memory
    void cleanup();
    
    // Check if GPIO is initialized
    bool isInitialized() const { return initialized_; }
    
    // Pin configuration
    bool setPinMode(int pin, PinMode mode);
    bool setPullMode(int pin, PullMode mode);
    
    // Digital I/O
    bool digitalWrite(int pin, bool value);
    int digitalRead(int pin);
    
    // Get last error message
    std::string getLastError() const { return lastError_; }

private:
    volatile uint32_t* gpio_map_;
    int mem_fd_;
    bool initialized_;
    std::string lastError_;
    uint32_t peri_base_;
    
    // Helper functions
    bool detectPeripheralBase();
    void setError(const std::string& error);
    bool validatePin(int pin);
};

// Global GPIO instance for runtime library
extern GPIO* g_gpio;

// C-style interface functions for BASIC runtime
extern "C" {
    // Initialize GPIO system
    int gpio_init();
    
    // Cleanup GPIO system
    void gpio_cleanup();
    
    // Pin configuration
    int gpio_set_mode(int pin, int mode);
    int gpio_set_pull(int pin, int pull);
    
    // Digital I/O
    int gpio_write(int pin, int value);
    int gpio_read(int pin);
    
    // Get last error message
    const char* gpio_get_error();
}

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED

#endif // RPI_GPIO_H
