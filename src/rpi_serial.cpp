#ifdef RPI_SUPPORT_ENABLED

#include "rpi_serial.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstring>
#include <iostream>

namespace rbasic {
namespace rpi {

// Global serial port instances
Serial* g_serial_ports[4] = {nullptr, nullptr, nullptr, nullptr};

Serial::Serial() 
    : fd_(-1) {
}

Serial::~Serial() {
    close();
}

void Serial::setError(const std::string& error) {
    lastError_ = error;
    std::cerr << "Serial Error: " << error << std::endl;
}

bool Serial::open(const std::string& device) {
    if (fd_ >= 0) {
        close();
    }
    
    device_ = device;
    
    // Open serial port
    fd_ = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd_ < 0) {
        setError("Failed to open " + device + ". Make sure the device exists and user has permissions");
        return false;
    }
    
    // Make reads blocking
    fcntl(fd_, F_SETFL, 0);
    
    // Configure with default settings
    if (!configure()) {
        ::close(fd_);
        fd_ = -1;
        return false;
    }
    
    return true;
}

void Serial::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    device_.clear();
}

bool Serial::configure() {
    if (fd_ < 0) {
        setError("Serial port not open");
        return false;
    }
    
    struct termios options;
    
    // Get current options
    if (tcgetattr(fd_, &options) < 0) {
        setError("Failed to get terminal attributes");
        return false;
    }
    
    // Set default baud rate (115200)
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    
    // 8N1 (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    
    // Enable receiver, ignore modem control lines
    options.c_cflag |= (CLOCAL | CREAD);
    
    // Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;
    
    // Raw mode (disable canonical mode, echo, signals)
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    // Disable software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    
    // Raw output mode
    options.c_oflag &= ~OPOST;
    
    // Set timeout (1 second)
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;
    
    // Apply options
    if (tcsetattr(fd_, TCSANOW, &options) < 0) {
        setError("Failed to set terminal attributes");
        return false;
    }
    
    // Flush buffers
    tcflush(fd_, TCIOFLUSH);
    
    return true;
}

bool Serial::setBaudRate(BaudRate baud) {
    if (fd_ < 0) {
        setError("Serial port not open");
        return false;
    }
    
    struct termios options;
    
    if (tcgetattr(fd_, &options) < 0) {
        setError("Failed to get terminal attributes");
        return false;
    }
    
    speed_t speed;
    switch (baud) {
        case BaudRate::BAUD_9600: speed = B9600; break;
        case BaudRate::BAUD_19200: speed = B19200; break;
        case BaudRate::BAUD_38400: speed = B38400; break;
        case BaudRate::BAUD_57600: speed = B57600; break;
        case BaudRate::BAUD_115200: speed = B115200; break;
        case BaudRate::BAUD_230400: speed = B230400; break;
        case BaudRate::BAUD_460800: speed = B460800; break;
        case BaudRate::BAUD_921600: speed = B921600; break;
        default:
            setError("Invalid baud rate");
            return false;
    }
    
    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);
    
    if (tcsetattr(fd_, TCSANOW, &options) < 0) {
        setError("Failed to set baud rate");
        return false;
    }
    
    return true;
}

bool Serial::setDataBits(int bits) {
    if (fd_ < 0) {
        setError("Serial port not open");
        return false;
    }
    
    if (bits < 5 || bits > 8) {
        setError("Invalid data bits: " + std::to_string(bits) + " (valid: 5-8)");
        return false;
    }
    
    struct termios options;
    
    if (tcgetattr(fd_, &options) < 0) {
        setError("Failed to get terminal attributes");
        return false;
    }
    
    options.c_cflag &= ~CSIZE;
    
    switch (bits) {
        case 5: options.c_cflag |= CS5; break;
        case 6: options.c_cflag |= CS6; break;
        case 7: options.c_cflag |= CS7; break;
        case 8: options.c_cflag |= CS8; break;
    }
    
    if (tcsetattr(fd_, TCSANOW, &options) < 0) {
        setError("Failed to set data bits");
        return false;
    }
    
    return true;
}

bool Serial::setParity(Parity parity) {
    if (fd_ < 0) {
        setError("Serial port not open");
        return false;
    }
    
    struct termios options;
    
    if (tcgetattr(fd_, &options) < 0) {
        setError("Failed to get terminal attributes");
        return false;
    }
    
    switch (parity) {
        case Parity::NONE:
            options.c_cflag &= ~PARENB;
            break;
        case Parity::ODD:
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            break;
        case Parity::EVEN:
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            break;
    }
    
    if (tcsetattr(fd_, TCSANOW, &options) < 0) {
        setError("Failed to set parity");
        return false;
    }
    
    return true;
}

bool Serial::setStopBits(StopBits stop_bits) {
    if (fd_ < 0) {
        setError("Serial port not open");
        return false;
    }
    
    struct termios options;
    
    if (tcgetattr(fd_, &options) < 0) {
        setError("Failed to get terminal attributes");
        return false;
    }
    
    switch (stop_bits) {
        case StopBits::ONE:
            options.c_cflag &= ~CSTOPB;
            break;
        case StopBits::TWO:
            options.c_cflag |= CSTOPB;
            break;
    }
    
    if (tcsetattr(fd_, TCSANOW, &options) < 0) {
        setError("Failed to set stop bits");
        return false;
    }
    
    return true;
}

int Serial::write(const uint8_t* data, size_t length) {
    if (fd_ < 0) {
        setError("Serial port not open");
        return -1;
    }
    
    ssize_t written = ::write(fd_, data, length);
    if (written < 0) {
        setError("Write failed");
        return -1;
    }
    
    return written;
}

int Serial::read(uint8_t* data, size_t length) {
    if (fd_ < 0) {
        setError("Serial port not open");
        return -1;
    }
    
    ssize_t bytes_read = ::read(fd_, data, length);
    if (bytes_read < 0) {
        setError("Read failed");
        return -1;
    }
    
    return bytes_read;
}

int Serial::available() {
    if (fd_ < 0) {
        return 0;
    }
    
    int bytes_available = 0;
    ioctl(fd_, FIONREAD, &bytes_available);
    
    return bytes_available;
}

void Serial::flush() {
    if (fd_ >= 0) {
        tcflush(fd_, TCIOFLUSH);
    }
}

bool Serial::writeByte(uint8_t byte) {
    return write(&byte, 1) == 1;
}

int Serial::readByte() {
    uint8_t byte = 0;
    int result = read(&byte, 1);
    
    if (result == 1) {
        return byte;
    }
    
    return -1;
}

bool Serial::writeString(const std::string& str) {
    return write(reinterpret_cast<const uint8_t*>(str.c_str()), str.length()) == static_cast<int>(str.length());
}

std::string Serial::readString(size_t max_length) {
    std::vector<uint8_t> buffer(max_length);
    int bytes_read = read(buffer.data(), max_length);
    
    if (bytes_read <= 0) {
        return "";
    }
    
    return std::string(reinterpret_cast<char*>(buffer.data()), bytes_read);
}

std::string Serial::readLine(size_t max_length) {
    std::string line;
    line.reserve(max_length);
    
    while (line.length() < max_length) {
        int byte = readByte();
        
        if (byte < 0) {
            break; // Timeout or error
        }
        
        if (byte == '\n') {
            break; // End of line
        }
        
        if (byte != '\r') {
            line += static_cast<char>(byte);
        }
    }
    
    return line;
}

// C-style interface implementation
extern "C" {

int serial_open(const char* device) {
    // Find free slot
    int handle = -1;
    for (int i = 0; i < 4; i++) {
        if (g_serial_ports[i] == nullptr) {
            handle = i;
            break;
        }
    }
    
    if (handle < 0) {
        return -1; // No free slots
    }
    
    g_serial_ports[handle] = new Serial();
    
    if (!g_serial_ports[handle]->open(device)) {
        delete g_serial_ports[handle];
        g_serial_ports[handle] = nullptr;
        return -1;
    }
    
    return handle;
}

void serial_close(int handle) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return;
    }
    
    g_serial_ports[handle]->close();
    delete g_serial_ports[handle];
    g_serial_ports[handle] = nullptr;
}

int serial_set_baud(int handle, int baud) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return 0;
    }
    
    BaudRate baud_rate;
    switch (baud) {
        case 9600: baud_rate = BaudRate::BAUD_9600; break;
        case 19200: baud_rate = BaudRate::BAUD_19200; break;
        case 38400: baud_rate = BaudRate::BAUD_38400; break;
        case 57600: baud_rate = BaudRate::BAUD_57600; break;
        case 115200: baud_rate = BaudRate::BAUD_115200; break;
        case 230400: baud_rate = BaudRate::BAUD_230400; break;
        case 460800: baud_rate = BaudRate::BAUD_460800; break;
        case 921600: baud_rate = BaudRate::BAUD_921600; break;
        default: return 0;
    }
    
    return g_serial_ports[handle]->setBaudRate(baud_rate) ? 1 : 0;
}

int serial_set_config(int handle, int data_bits, int parity, int stop_bits) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return 0;
    }
    
    if (!g_serial_ports[handle]->setDataBits(data_bits)) {
        return 0;
    }
    
    Parity parity_mode;
    switch (parity) {
        case 0: parity_mode = Parity::NONE; break;
        case 1: parity_mode = Parity::ODD; break;
        case 2: parity_mode = Parity::EVEN; break;
        default: return 0;
    }
    
    if (!g_serial_ports[handle]->setParity(parity_mode)) {
        return 0;
    }
    
    StopBits stop_bits_mode;
    switch (stop_bits) {
        case 1: stop_bits_mode = StopBits::ONE; break;
        case 2: stop_bits_mode = StopBits::TWO; break;
        default: return 0;
    }
    
    return g_serial_ports[handle]->setStopBits(stop_bits_mode) ? 1 : 0;
}

int serial_write_byte(int handle, int byte) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return 0;
    }
    
    return g_serial_ports[handle]->writeByte(byte) ? 1 : 0;
}

int serial_read_byte(int handle) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return -1;
    }
    
    return g_serial_ports[handle]->readByte();
}

int serial_write(int handle, const void* data, int length) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return -1;
    }
    
    return g_serial_ports[handle]->write(static_cast<const uint8_t*>(data), length);
}

int serial_read(int handle, void* data, int length) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return -1;
    }
    
    return g_serial_ports[handle]->read(static_cast<uint8_t*>(data), length);
}

int serial_write_string(int handle, const char* str) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return 0;
    }
    
    return g_serial_ports[handle]->writeString(str) ? 1 : 0;
}

const char* serial_read_line(int handle) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return "";
    }
    
    static std::string line;
    line = g_serial_ports[handle]->readLine();
    return line.c_str();
}

int serial_available(int handle) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return 0;
    }
    
    return g_serial_ports[handle]->available();
}

void serial_flush(int handle) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return;
    }
    
    g_serial_ports[handle]->flush();
}

const char* serial_get_error(int handle) {
    if (handle < 0 || handle >= 4 || g_serial_ports[handle] == nullptr) {
        return "Invalid serial handle";
    }
    
    static std::string error;
    error = g_serial_ports[handle]->getLastError();
    return error.c_str();
}

} // extern "C"

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED
