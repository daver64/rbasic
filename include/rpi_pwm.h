#ifndef RPI_PWM_H
#define RPI_PWM_H

#ifdef RPI_SUPPORT_ENABLED

#include <cstdint>
#include <string>

namespace rbasic {
namespace rpi {

// PWM channel (0 or 1 for hardware PWM)
enum class PWMChannel {
    CHANNEL_0 = 0,
    CHANNEL_1 = 1
};

// PWM class for managing PWM output
class PWM {
public:
    PWM();
    ~PWM();

    // Initialize PWM channel (hardware PWM via sysfs)
    bool initialize(int channel);
    
    // Cleanup and unexport
    void cleanup();
    
    // Check if PWM is initialized
    bool isInitialized() const { return initialized_; }
    
    // Configure PWM
    bool setPeriod(uint32_t period_ns);
    bool setDutyCycle(uint32_t duty_cycle_ns);
    bool setFrequency(uint32_t frequency_hz);
    bool setDutyCyclePercent(float percent);
    
    // Enable/disable PWM
    bool enable();
    bool disable();
    
    // Get current settings
    uint32_t getPeriod() const { return period_ns_; }
    uint32_t getDutyCycle() const { return duty_cycle_ns_; }
    bool isEnabled() const { return enabled_; }
    
    // Get last error message
    std::string getLastError() const { return lastError_; }

private:
    int channel_;
    bool initialized_;
    bool exported_;
    bool enabled_;
    uint32_t period_ns_;
    uint32_t duty_cycle_ns_;
    std::string lastError_;
    
    void setError(const std::string& error);
    bool writeToSysfs(const std::string& filename, const std::string& value);
    std::string readFromSysfs(const std::string& filename);
};

// Global PWM instances (hardware channels 0 and 1)
extern PWM* g_pwm_channels[2];

// C-style interface functions for BASIC runtime
extern "C" {
    // Initialize PWM channel (0 or 1)
    int pwm_init(int channel);
    
    // Cleanup PWM channel
    void pwm_cleanup(int channel);
    
    // Configure PWM
    int pwm_set_frequency(int channel, int frequency_hz);
    int pwm_set_duty_cycle(int channel, float percent);
    
    // Enable/disable
    int pwm_enable(int channel);
    int pwm_disable(int channel);
    
    // Get last error
    const char* pwm_get_error(int channel);
}

} // namespace rpi
} // namespace rbasic

#endif // RPI_SUPPORT_ENABLED

#endif // RPI_PWM_H
