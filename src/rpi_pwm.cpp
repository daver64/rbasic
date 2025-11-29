#ifdef RPI_SUPPORT_ENABLED

#include "rpi_pwm.h"
#include <fstream>
#include <iostream>
#include <unistd.h>

namespace rbasic {
namespace rpi {

// Global PWM channel instances
PWM* g_pwm_channels[2] = {nullptr, nullptr};

PWM::PWM() 
    : channel_(-1), initialized_(false), exported_(false), 
      enabled_(false), period_ns_(1000000), duty_cycle_ns_(0) {
}

PWM::~PWM() {
    cleanup();
}

void PWM::setError(const std::string& error) {
    lastError_ = error;
    std::cerr << "PWM Error: " << error << std::endl;
}

bool PWM::writeToSysfs(const std::string& filename, const std::string& value) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        setError("Failed to open " + filename);
        return false;
    }
    
    file << value;
    file.close();
    
    if (file.fail()) {
        setError("Failed to write to " + filename);
        return false;
    }
    
    return true;
}

std::string PWM::readFromSysfs(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        setError("Failed to open " + filename);
        return "";
    }
    
    std::string value;
    std::getline(file, value);
    
    return value;
}

bool PWM::initialize(int channel) {
    if (initialized_) {
        cleanup();
    }
    
    // Validate channel
    if (channel < 0 || channel > 1) {
        setError("Invalid PWM channel: " + std::to_string(channel) + " (valid: 0-1)");
        return false;
    }
    
    channel_ = channel;
    
    // PWM chip is usually pwmchip0 on Raspberry Pi
    std::string chip_path = "/sys/class/pwm/pwmchip0";
    
    // Check if chip exists
    std::ifstream chip_check(chip_path + "/npwm");
    if (!chip_check.is_open()) {
        setError("PWM chip not found. Make sure PWM is enabled in device tree");
        return false;
    }
    chip_check.close();
    
    // Export PWM channel if not already exported
    std::string pwm_path = chip_path + "/pwm" + std::to_string(channel_);
    std::ifstream pwm_check(pwm_path + "/period");
    
    if (!pwm_check.is_open()) {
        // Need to export
        if (!writeToSysfs(chip_path + "/export", std::to_string(channel_))) {
            setError("Failed to export PWM channel " + std::to_string(channel_));
            return false;
        }
        
        // Wait for export to complete
        usleep(100000); // 100ms
        exported_ = true;
    }
    pwm_check.close();
    
    // Set default period (1ms = 1kHz)
    if (!setPeriod(1000000)) {
        return false;
    }
    
    // Set duty cycle to 0
    if (!setDutyCycle(0)) {
        return false;
    }
    
    initialized_ = true;
    return true;
}

void PWM::cleanup() {
    if (initialized_) {
        disable();
        
        // Unexport if we exported it
        if (exported_ && channel_ >= 0) {
            std::string chip_path = "/sys/class/pwm/pwmchip0";
            writeToSysfs(chip_path + "/unexport", std::to_string(channel_));
            exported_ = false;
        }
        
        initialized_ = false;
        channel_ = -1;
    }
}

bool PWM::setPeriod(uint32_t period_ns) {
    if (!initialized_) {
        setError("PWM not initialized");
        return false;
    }
    
    if (period_ns == 0) {
        setError("Period cannot be zero");
        return false;
    }
    
    // Disable PWM before changing period
    bool was_enabled = enabled_;
    if (was_enabled) {
        disable();
    }
    
    std::string pwm_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel_);
    
    if (!writeToSysfs(pwm_path + "/period", std::to_string(period_ns))) {
        return false;
    }
    
    period_ns_ = period_ns;
    
    // Adjust duty cycle if it exceeds new period
    if (duty_cycle_ns_ > period_ns_) {
        setDutyCycle(period_ns_);
    }
    
    // Re-enable if it was enabled
    if (was_enabled) {
        enable();
    }
    
    return true;
}

bool PWM::setDutyCycle(uint32_t duty_cycle_ns) {
    if (!initialized_) {
        setError("PWM not initialized");
        return false;
    }
    
    if (duty_cycle_ns > period_ns_) {
        setError("Duty cycle (" + std::to_string(duty_cycle_ns) + 
                ") cannot exceed period (" + std::to_string(period_ns_) + ")");
        return false;
    }
    
    std::string pwm_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel_);
    
    if (!writeToSysfs(pwm_path + "/duty_cycle", std::to_string(duty_cycle_ns))) {
        return false;
    }
    
    duty_cycle_ns_ = duty_cycle_ns;
    return true;
}

bool PWM::setFrequency(uint32_t frequency_hz) {
    if (frequency_hz == 0) {
        setError("Frequency cannot be zero");
        return false;
    }
    
    // Convert frequency to period in nanoseconds
    uint32_t period_ns = 1000000000 / frequency_hz;
    
    return setPeriod(period_ns);
}

bool PWM::setDutyCyclePercent(float percent) {
    if (percent < 0.0f || percent > 100.0f) {
        setError("Duty cycle percent must be between 0 and 100");
        return false;
    }
    
    uint32_t duty_cycle_ns = static_cast<uint32_t>((percent / 100.0f) * period_ns_);
    
    return setDutyCycle(duty_cycle_ns);
}

bool PWM::enable() {
    if (!initialized_) {
        setError("PWM not initialized");
        return false;
    }
    
    std::string pwm_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel_);
    
    if (!writeToSysfs(pwm_path + "/enable", "1")) {
        return false;
    }
    
    enabled_ = true;
    return true;
}

bool PWM::disable() {
    if (!initialized_) {
        return true; // Already disabled
    }
    
    std::string pwm_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel_);
    
    if (!writeToSysfs(pwm_path + "/enable", "0")) {
        return false;
    }
    
    enabled_ = false;
    return true;
}

// C-style interface implementation
extern "C" {

int pwm_init(int channel) {
    if (channel < 0 || channel >= 2) {
        return 0;
    }
    
    // Create PWM instance if needed
    if (g_pwm_channels[channel] == nullptr) {
        g_pwm_channels[channel] = new PWM();
    }
    
    return g_pwm_channels[channel]->initialize(channel) ? 1 : 0;
}

void pwm_cleanup(int channel) {
    if (channel < 0 || channel >= 2) {
        return;
    }
    
    if (g_pwm_channels[channel] != nullptr) {
        g_pwm_channels[channel]->cleanup();
        delete g_pwm_channels[channel];
        g_pwm_channels[channel] = nullptr;
    }
}

int pwm_set_frequency(int channel, int frequency_hz) {
    if (channel < 0 || channel >= 2 || g_pwm_channels[channel] == nullptr) {
        return 0;
    }
    
    return g_pwm_channels[channel]->setFrequency(frequency_hz) ? 1 : 0;
}

int pwm_set_duty_cycle(int channel, float percent) {
    if (channel < 0 || channel >= 2 || g_pwm_channels[channel] == nullptr) {
        return 0;
    }
    
    return g_pwm_channels[channel]->setDutyCyclePercent(percent) ? 1 : 0;
}

int pwm_enable(int channel) {
    if (channel < 0 || channel >= 2 || g_pwm_channels[channel] == nullptr) {
        return 0;
    }
    
    return g_pwm_channels[channel]->enable() ? 1 : 0;
}

int pwm_disable(int channel) {
    if (channel < 0 || channel >= 2 || g_pwm_channels[channel] == nullptr) {
        return 0;
    }
    
    return g_pwm_channels[channel]->disable() ? 1 : 0;
}

const char* pwm_get_error(int channel) {
    if (channel < 0 || channel >= 2 || g_pwm_channels[channel] == nullptr) {
        return "Invalid PWM channel";
    }
    
    static std::string error;
    error = g_pwm_channels[channel]->getLastError();
    return error.c_str();
}

} // extern "C"

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED
