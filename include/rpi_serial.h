#ifndef RPI_SERIAL_H
#define RPI_SERIAL_H

#ifdef RPI_SUPPORT_ENABLED

#include <cstdint>
#include <string>
#include <vector>

namespace rbasic {
namespace rpi {

// Serial baud rates
enum class BaudRate {
    BAUD_9600 = 9600,
    BAUD_19200 = 19200,
    BAUD_38400 = 38400,
    BAUD_57600 = 57600,
    BAUD_115200 = 115200,
    BAUD_230400 = 230400,
    BAUD_460800 = 460800,
    BAUD_921600 = 921600
};

// Parity modes
enum class Parity {
    NONE = 0,
    ODD = 1,
    EVEN = 2
};

// Stop bits
enum class StopBits {
    ONE = 1,
    TWO = 2
};

// Serial class for managing UART communication
class Serial {
public:
    Serial();
    ~Serial();

    // Open serial port
    bool open(const std::string& device);
    
    // Close serial port
    void close();
    
    // Check if serial port is open
    bool isOpen() const { return fd_ >= 0; }
    
    // Configure serial port
    bool setBaudRate(BaudRate baud);
    bool setDataBits(int bits);
    bool setParity(Parity parity);
    bool setStopBits(StopBits stop_bits);
    
    // Read/write operations
    int write(const uint8_t* data, size_t length);
    int read(uint8_t* data, size_t length);
    int available();
    void flush();
    
    // Convenience methods
    bool writeByte(uint8_t byte);
    int readByte();
    bool writeString(const std::string& str);
    std::string readString(size_t max_length);
    std::string readLine(size_t max_length = 1024);
    
    // Get last error message
    std::string getLastError() const { return lastError_; }

private:
    int fd_;
    std::string device_;
    std::string lastError_;
    
    void setError(const std::string& error);
    bool configure();
};

// Global serial instances
extern Serial* g_serial_ports[4];

// C-style interface functions for BASIC runtime
extern "C" {
    // Open serial port (returns handle 0-3, or -1 on error)
    int serial_open(const char* device);
    
    // Close serial port
    void serial_close(int handle);
    
    // Configure serial port
    int serial_set_baud(int handle, int baud);
    int serial_set_config(int handle, int data_bits, int parity, int stop_bits);
    
    // Read/write bytes
    int serial_write_byte(int handle, int byte);
    int serial_read_byte(int handle);
    int serial_write(int handle, const void* data, int length);
    int serial_read(int handle, void* data, int length);
    
    // String operations
    int serial_write_string(int handle, const char* str);
    const char* serial_read_line(int handle);
    
    // Check available data
    int serial_available(int handle);
    
    // Flush buffer
    void serial_flush(int handle);
    
    // Get last error
    const char* serial_get_error(int handle);
}

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED

#endif // RPI_SERIAL_H
