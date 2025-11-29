// Raspberry Pi Temperature Data Logger
// Reads CPU temperature and logs to file
// Demonstrates file I/O and system monitoring

var LOG_FILE = "temperature_log.txt";
var LOG_INTERVAL = 2000;  // 2 seconds
var NUM_SAMPLES = 10;

print("=== Raspberry Pi Temperature Logger ===");
print("Logging CPU temperature to: " + LOG_FILE);
print("Samples: " + str(NUM_SAMPLES));
print("Interval: " + str(LOG_INTERVAL / 1000) + " seconds");
print("");

// Create log file header
var header = "Raspberry Pi Temperature Log\n";
header = header + "================================\n";
write_text_file(LOG_FILE, header);

print("Starting temperature monitoring...");
print("");

var i = 0;
for (i = 0; i < NUM_SAMPLES; i = i + 1) {
    // Read CPU temperature from system file
    var temp_str = read_text_file("/sys/class/thermal/thermal_zone0/temp");
    
    // Convert to Celsius (file contains millidegrees)
    var temp_milli = val(temp_str);
    var temp_c = temp_milli / 1000.0;
    var temp_f = (temp_c * 9.0 / 5.0) + 32.0;
    
    // Format timestamp (simple counter for now)
    var log_entry = "Sample " + str(i + 1) + ": ";
    log_entry = log_entry + str(temp_c) + "°C / ";
    log_entry = log_entry + str(temp_f) + "°F\n";
    
    // Append to log file
    var current_log = read_text_file(LOG_FILE);
    current_log = current_log + log_entry;
    write_text_file(LOG_FILE, current_log);
    
    // Display to console
    print("Sample " + str(i + 1) + ": " + str(temp_c) + "°C (" + str(temp_f) + "°F)");
    
    // Wait before next sample
    if (i < NUM_SAMPLES - 1) {
        sleep(LOG_INTERVAL);
    }
}

print("");
print("Logging complete!");
print("View log with: cat " + LOG_FILE);
print("");

// Display summary
var final_log = read_text_file(LOG_FILE);
print("=== Log File Contents ===");
print(final_log);
