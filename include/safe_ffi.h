#pragma once

#include "unified_value.h"
#include "memory_manager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibraryHandle;
#else
    #include <dlfcn.h>
    typedef void* LibraryHandle;
#endif

namespace rbasic {
namespace ffi {

// Forward declarations
class SafeLibrary;
class SafeFunctionCall;

// Enhanced FFI manager with memory safety
class SafeFFIManager {
public:
    static SafeFFIManager& instance();
    
    // Library management with RAII
    std::shared_ptr<SafeLibrary> load_library(const std::string& name);
    bool unload_library(const std::string& name);
    std::shared_ptr<SafeLibrary> get_library(const std::string& name);
    
    // Safe buffer operations
    std::shared_ptr<MemoryManager::SafeBuffer> allocate_buffer(size_t size);
    MemoryManager::managed_ptr<int> allocate_int_buffer();
    MemoryManager::managed_ptr<void*> allocate_pointer_buffer();
    
    // Bounds-checked memory access
    UnifiedValue deref_int_safe(const UnifiedValue& ptr, size_t offset = 0);
    UnifiedValue deref_pointer_safe(const UnifiedValue& ptr, size_t offset = 0);
    UnifiedValue deref_string_safe(const UnifiedValue& ptr, size_t max_length = 1024);
    
    // Set operations with bounds checking
    void set_int_safe(const UnifiedValue& ptr, const UnifiedValue& value, size_t offset = 0);
    void set_pointer_safe(const UnifiedValue& ptr, const UnifiedValue& value, size_t offset = 0);
    
    // Null pointer safety
    bool is_null_safe(const UnifiedValue& ptr);
    bool not_null_safe(const UnifiedValue& ptr);
    
    // Cleanup
    void cleanup();
    
    // Statistics
    size_t get_loaded_library_count() const;
    size_t get_total_allocated_memory() const;
    
    // Helper methods
    void validate_pointer(const UnifiedValue& ptr) const;
    void* extract_raw_pointer(const UnifiedValue& ptr) const;
    
private:
    SafeFFIManager() = default;
    ~SafeFFIManager();
    
    std::unordered_map<std::string, std::shared_ptr<SafeLibrary>> loaded_libraries_;
    std::vector<std::shared_ptr<MemoryManager::SafeBuffer>> allocated_buffers_;
    mutable std::mutex mutex_;
};

// RAII library wrapper with automatic cleanup
class SafeLibrary {
public:
    SafeLibrary(const std::string& name, LibraryHandle handle);
    ~SafeLibrary();
    
    // No copy semantics
    SafeLibrary(const SafeLibrary&) = delete;
    SafeLibrary& operator=(const SafeLibrary&) = delete;
    
    // Move semantics
    SafeLibrary(SafeLibrary&& other) noexcept;
    SafeLibrary& operator=(SafeLibrary&& other) noexcept;
    
    // Safe function address retrieval
    void* get_function_address(const std::string& function_name) const;
    
    // Library info
    const std::string& name() const { return name_; }
    bool is_valid() const { return handle_ != nullptr; }
    
    // Function call with bounds checking
    UnifiedValue call_function(const std::string& function_name, 
                              const std::vector<UnifiedValue>& args,
                              const std::string& return_type = "int");
    
private:
    std::string name_;
    LibraryHandle handle_;
    
    // Function signature validation
    bool validate_function_signature(const std::string& function_name, 
                                   const std::vector<UnifiedValue>& args) const;
};

// Safe function call wrapper with automatic parameter conversion
class SafeFunctionCall {
public:
    SafeFunctionCall(void* function_ptr, const std::string& return_type);
    
    // Add parameters with type validation
    SafeFunctionCall& add_int(const UnifiedValue& value);
    SafeFunctionCall& add_double(const UnifiedValue& value);
    SafeFunctionCall& add_string(const UnifiedValue& value);
    SafeFunctionCall& add_pointer(const UnifiedValue& value);
    
    // Execute with bounds checking and exception safety
    UnifiedValue execute();
    
    // Parameter count validation
    bool validate_parameter_count(size_t expected_count) const;
    
private:
    void* function_ptr_;
    std::string return_type_;
    std::vector<UnifiedValue> parameters_;
    FFIScope scope_;  // Automatic cleanup of temporary allocations
    
    // Type conversion with validation
    int convert_to_int(const UnifiedValue& value);
    double convert_to_double(const UnifiedValue& value);
    const char* convert_to_string(const UnifiedValue& value);
    void* convert_to_pointer(const UnifiedValue& value);
    
    // Platform-specific calling convention handling
    UnifiedValue call_with_signature();
};

// SDL resource wrapper with automatic cleanup
template<typename T>
class SafeSDLResource {
public:
    SafeSDLResource() : resource_(nullptr), size_(0) {}
    
    template<typename... Args>
    SafeSDLResource(size_t size, Args&&... args) : size_(size) {
        auto buffer = MemoryManager::instance().create_safe_buffer(size);
        resource_ = buffer->template as<T>();
        buffer_ = buffer;
        
        // Initialize with constructor arguments if provided
        if constexpr (sizeof...(args) > 0) {
            new (resource_) T(std::forward<Args>(args)...);
        } else {
            std::memset(resource_, 0, size);
        }
    }
    
    ~SafeSDLResource() = default;
    
    // Move semantics
    SafeSDLResource(SafeSDLResource&& other) noexcept 
        : resource_(other.resource_), size_(other.size_), buffer_(std::move(other.buffer_)) {
        other.resource_ = nullptr;
        other.size_ = 0;
    }
    
    SafeSDLResource& operator=(SafeSDLResource&& other) noexcept {
        if (this != &other) {
            resource_ = other.resource_;
            size_ = other.size_;
            buffer_ = std::move(other.buffer_);
            other.resource_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    // No copy semantics
    SafeSDLResource(const SafeSDLResource&) = delete;
    SafeSDLResource& operator=(const SafeSDLResource&) = delete;
    
    T* get() const { return resource_; }
    T* operator->() const { return resource_; }
    T& operator*() const { return *resource_; }
    
    explicit operator bool() const { return resource_ != nullptr; }
    
    size_t size() const { return size_; }
    
    // Bounds-checked field access
    template<typename FieldType>
    FieldType& get_field(size_t offset) {
        if (offset + sizeof(FieldType) > size_) {
            throw std::out_of_range("SDL resource field access out of bounds");
        }
        return *reinterpret_cast<FieldType*>(reinterpret_cast<uint8_t*>(resource_) + offset);
    }
    
    template<typename FieldType>
    const FieldType& get_field(size_t offset) const {
        if (offset + sizeof(FieldType) > size_) {
            throw std::out_of_range("SDL resource field access out of bounds");
        }
        return *reinterpret_cast<const FieldType*>(reinterpret_cast<const uint8_t*>(resource_) + offset);
    }
    
private:
    T* resource_;
    size_t size_;
    std::shared_ptr<MemoryManager::SafeBuffer> buffer_;
};

// SDL structure definitions with bounds checking
struct SafeSDLRect {
    int x, y, w, h;
    
    SafeSDLRect(int x = 0, int y = 0, int w = 0, int h = 0) : x(x), y(y), w(w), h(h) {}
};

struct SafeSDLEvent {
    uint32_t type;
    uint8_t padding[52];  // Total 56 bytes to match SDL_Event
    
    SafeSDLEvent() : type(0) {
        std::memset(padding, 0, sizeof(padding));
    }
    
    // Safe access to keyboard event data
    struct KeyboardEvent {
        uint32_t type;
        uint32_t timestamp;
        uint32_t windowID;
        uint8_t state;
        uint8_t repeat;
        uint8_t padding2[2];
        uint32_t scancode;
        uint32_t sym;
        uint16_t mod;
        uint32_t unused;
    };
    
    KeyboardEvent* as_keyboard() {
        if (type != 768 && type != 769) {  // SDL_KEYDOWN = 768, SDL_KEYUP = 769
            return nullptr;
        }
        return reinterpret_cast<KeyboardEvent*>(this);
    }
};

// Helper functions for safe SDL operations
UnifiedValue create_safe_sdl_rect(int x, int y, int w, int h);
UnifiedValue create_safe_sdl_event();
UnifiedValue get_safe_event_type(const UnifiedValue& event);
UnifiedValue get_safe_key_code(const UnifiedValue& event);
UnifiedValue get_safe_rect_field(const UnifiedValue& rect, const std::string& field);

// Error handling for FFI operations
class SafeFFIError : public std::runtime_error {
public:
    explicit SafeFFIError(const std::string& message) : std::runtime_error("Safe FFI Error: " + message) {}
};

class BufferOverflowError : public SafeFFIError {
public:
    explicit BufferOverflowError(const std::string& message) : SafeFFIError("Buffer overflow: " + message) {}
};

class NullPointerError : public SafeFFIError {
public:
    explicit NullPointerError(const std::string& message) : SafeFFIError("Null pointer access: " + message) {}
};

class LibraryLoadError : public SafeFFIError {
public:
    explicit LibraryLoadError(const std::string& message) : SafeFFIError("Library load failed: " + message) {}
};

// Utility functions
std::string get_platform_library_name(const std::string& base_name);
std::string get_last_system_error();
bool validate_library_exists(const std::string& library_path);

} // namespace ffi
} // namespace rbasic