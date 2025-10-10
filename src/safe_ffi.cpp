#include "safe_ffi.h"
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#ifndef F_OK
#define F_OK 0
#endif
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

namespace rbasic {
namespace ffi {

// SafeFFIManager implementation
SafeFFIManager& SafeFFIManager::instance() {
    static SafeFFIManager instance;
    return instance;
}

SafeFFIManager::~SafeFFIManager() {
    cleanup();
}

std::shared_ptr<SafeLibrary> SafeFFIManager::load_library(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loaded_libraries_.find(name);
    if (it != loaded_libraries_.end()) {
        if (auto lib = it->second) {
            return lib;
        }
        loaded_libraries_.erase(it);
    }
    
#ifdef _WIN32
    LibraryHandle handle = LoadLibraryA(name.c_str());
#else
    LibraryHandle handle = dlopen(name.c_str(), RTLD_LAZY);
#endif
    
    if (!handle) {
        std::ostringstream oss;
        oss << "Failed to load library: " << name << " - " << get_last_system_error();
        throw LibraryLoadError(oss.str());
    }
    
    auto lib = std::make_shared<SafeLibrary>(name, handle);
    loaded_libraries_[name] = lib;
    return lib;
}

bool SafeFFIManager::unload_library(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = loaded_libraries_.find(name);
    if (it != loaded_libraries_.end()) {
        loaded_libraries_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<SafeLibrary> SafeFFIManager::get_library(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = loaded_libraries_.find(name);
    if (it != loaded_libraries_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<MemoryManager::SafeBuffer> SafeFFIManager::allocate_buffer(size_t size) {
    auto buffer = MemoryManager::instance().create_safe_buffer(size);
    std::lock_guard<std::mutex> lock(mutex_);
    allocated_buffers_.push_back(buffer);
    return buffer;
}

MemoryManager::managed_ptr<int> SafeFFIManager::allocate_int_buffer() {
    return MemoryManager::instance().allocate_int_buffer();
}

MemoryManager::managed_ptr<void*> SafeFFIManager::allocate_pointer_buffer() {
    return MemoryManager::instance().allocate_pointer_buffer();
}

UnifiedValue SafeFFIManager::deref_int_safe(const UnifiedValue& ptr, size_t offset) {
    void* raw_ptr = extract_raw_pointer(ptr);
    if (!raw_ptr) {
        throw NullPointerError("Cannot dereference null pointer");
    }
    
    // Basic bounds checking - in a real implementation, we'd track buffer sizes
    int* int_ptr = static_cast<int*>(raw_ptr);
    return make_int(int_ptr[offset]);
}

UnifiedValue SafeFFIManager::deref_pointer_safe(const UnifiedValue& ptr, size_t offset) {
    void* raw_ptr = extract_raw_pointer(ptr);
    if (!raw_ptr) {
        throw NullPointerError("Cannot dereference null pointer");
    }
    
    void** ptr_ptr = static_cast<void**>(raw_ptr);
    return make_pointer(ptr_ptr[offset]);
}

UnifiedValue SafeFFIManager::deref_string_safe(const UnifiedValue& ptr, size_t max_length) {
    void* raw_ptr = extract_raw_pointer(ptr);
    if (!raw_ptr) {
        throw NullPointerError("Cannot dereference null pointer");
    }
    
    const char* str_ptr = static_cast<const char*>(raw_ptr);
    size_t len = strnlen(str_ptr, max_length);
    return make_string(std::string(str_ptr, len));
}

void SafeFFIManager::set_int_safe(const UnifiedValue& ptr, const UnifiedValue& value, size_t offset) {
    void* raw_ptr = extract_raw_pointer(ptr);
    if (!raw_ptr) {
        throw NullPointerError("Cannot write to null pointer");
    }
    
    int* int_ptr = static_cast<int*>(raw_ptr);
    int_ptr[offset] = TypeConverter::to_int(value);
}

void SafeFFIManager::set_pointer_safe(const UnifiedValue& ptr, const UnifiedValue& value, size_t offset) {
    void* raw_ptr = extract_raw_pointer(ptr);
    if (!raw_ptr) {
        throw NullPointerError("Cannot write to null pointer");
    }
    
    void** ptr_ptr = static_cast<void**>(raw_ptr);
    ptr_ptr[offset] = TypeConverter::to_pointer(value);
}

bool SafeFFIManager::is_null_safe(const UnifiedValue& ptr) {
    try {
        return extract_raw_pointer(ptr) == nullptr;
    } catch (...) {
        return true;  // If we can't extract a pointer, treat as null
    }
}

bool SafeFFIManager::not_null_safe(const UnifiedValue& ptr) {
    return !is_null_safe(ptr);
}

void SafeFFIManager::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    loaded_libraries_.clear();
    allocated_buffers_.clear();
}

size_t SafeFFIManager::get_loaded_library_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return loaded_libraries_.size();
}

size_t SafeFFIManager::get_total_allocated_memory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = 0;
    for (const auto& buffer : allocated_buffers_) {
        if (auto buf = buffer) {
            total += buf->size();
        }
    }
    return total;
}

void SafeFFIManager::validate_pointer(const UnifiedValue& ptr) const {
    if (!holds_type<void*>(ptr) && !holds_type<std::shared_ptr<UnifiedPointer>>(ptr)) {
        throw SafeFFIError("Expected pointer type");
    }
}

void* SafeFFIManager::extract_raw_pointer(const UnifiedValue& ptr) const {
    if (holds_type<void*>(ptr)) {
        return get_value<void*>(ptr);
    } else if (holds_type<std::shared_ptr<UnifiedPointer>>(ptr)) {
        auto unified_ptr = get_value<std::shared_ptr<UnifiedPointer>>(ptr);
        return unified_ptr->get();
    }
    throw SafeFFIError("Cannot extract pointer from value");
}

// SafeLibrary implementation
SafeLibrary::SafeLibrary(const std::string& name, LibraryHandle handle) 
    : name_(name), handle_(handle) {}

SafeLibrary::~SafeLibrary() {
    if (handle_) {
#ifdef _WIN32
        FreeLibrary(handle_);
#else
        dlclose(handle_);
#endif
        handle_ = nullptr;
    }
}

SafeLibrary::SafeLibrary(SafeLibrary&& other) noexcept 
    : name_(std::move(other.name_)), handle_(other.handle_) {
    other.handle_ = nullptr;
}

SafeLibrary& SafeLibrary::operator=(SafeLibrary&& other) noexcept {
    if (this != &other) {
        if (handle_) {
#ifdef _WIN32
            FreeLibrary(handle_);
#else
            dlclose(handle_);
#endif
        }
        name_ = std::move(other.name_);
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

void* SafeLibrary::get_function_address(const std::string& function_name) const {
    if (!handle_) {
        throw SafeFFIError("Library not loaded: " + name_);
    }
    
#ifdef _WIN32
    void* func = GetProcAddress(handle_, function_name.c_str());
#else
    void* func = dlsym(handle_, function_name.c_str());
#endif
    
    if (!func) {
        std::ostringstream oss;
        oss << "Function not found: " << function_name << " in library: " << name_
            << " - " << get_last_system_error();
        throw SafeFFIError(oss.str());
    }
    
    return func;
}

UnifiedValue SafeLibrary::call_function(const std::string& function_name, 
                                       const std::vector<UnifiedValue>& args,
                                       const std::string& return_type) {
    void* func_ptr = get_function_address(function_name);
    if (!validate_function_signature(function_name, args)) {
        throw SafeFFIError("Invalid function signature for: " + function_name);
    }
    
    SafeFunctionCall call(func_ptr, return_type);
    for (const auto& arg : args) {
        if (holds_type<int>(arg)) {
            call.add_int(arg);
        } else if (holds_type<double>(arg)) {
            call.add_double(arg);
        } else if (holds_type<std::string>(arg)) {
            call.add_string(arg);
        } else if (holds_type<void*>(arg) || holds_type<std::shared_ptr<UnifiedPointer>>(arg)) {
            call.add_pointer(arg);
        } else {
            throw SafeFFIError("Unsupported argument type in function call");
        }
    }
    
    return call.execute();
}

bool SafeLibrary::validate_function_signature(const std::string& function_name, 
                                             const std::vector<UnifiedValue>& args) const {
    // Basic validation - in a real implementation, this would check against known signatures
    if (args.size() > 11) {  // Maximum supported parameters
        return false;
    }
    
    // Additional validation could be added here for specific known functions
    return true;
}

// SafeFunctionCall implementation
SafeFunctionCall::SafeFunctionCall(void* function_ptr, const std::string& return_type) 
    : function_ptr_(function_ptr), return_type_(return_type) {}

SafeFunctionCall& SafeFunctionCall::add_int(const UnifiedValue& value) {
    parameters_.push_back(make_int(convert_to_int(value)));
    return *this;
}

SafeFunctionCall& SafeFunctionCall::add_double(const UnifiedValue& value) {
    parameters_.push_back(make_double(convert_to_double(value)));
    return *this;
}

SafeFunctionCall& SafeFunctionCall::add_string(const UnifiedValue& value) {
    const char* str = convert_to_string(value);
    parameters_.push_back(make_pointer(const_cast<void*>(static_cast<const void*>(str))));
    return *this;
}

SafeFunctionCall& SafeFunctionCall::add_pointer(const UnifiedValue& value) {
    parameters_.push_back(make_pointer(convert_to_pointer(value)));
    return *this;
}

UnifiedValue SafeFunctionCall::execute() {
    if (!function_ptr_) {
        throw SafeFFIError("Cannot execute null function pointer");
    }
    
    return call_with_signature();
}

bool SafeFunctionCall::validate_parameter_count(size_t expected_count) const {
    return parameters_.size() == expected_count;
}

int SafeFunctionCall::convert_to_int(const UnifiedValue& value) {
    return TypeConverter::to_int(value);
}

double SafeFunctionCall::convert_to_double(const UnifiedValue& value) {
    return TypeConverter::to_double(value);
}

const char* SafeFunctionCall::convert_to_string(const UnifiedValue& value) {
    if (holds_type<std::string>(value)) {
        // Allocate temporary string buffer in scope
        std::string str = get_value<std::string>(value);
        char* temp_str = scope_.allocate_temp<char>(str.length() + 1);
        #ifdef _MSC_VER
        strcpy_s(temp_str, str.length() + 1, str.c_str());
        #else
        std::strcpy(temp_str, str.c_str());
        #endif
        return temp_str;
    }
    throw SafeFFIError("Cannot convert value to string");
}

void* SafeFunctionCall::convert_to_pointer(const UnifiedValue& value) {
    return TypeConverter::to_pointer(value);
}

UnifiedValue SafeFunctionCall::call_with_signature() {
    // Simplified function calling - in a real implementation, this would use
    // platform-specific calling conventions and handle different parameter counts
    
    typedef int (*func_ptr_t)();
    func_ptr_t func = reinterpret_cast<func_ptr_t>(function_ptr_);
    
    if (return_type_ == "int" || return_type_.empty()) {
        int result = func();
        return make_int(result);
    } else if (return_type_ == "double") {
        // This is simplified - real implementation would need proper type casting
        double result = static_cast<double>(func());
        return make_double(result);
    } else if (return_type_ == "pointer") {
        void* result = reinterpret_cast<void*>(func());
        return make_pointer(result);
    }
    
    // Default to int return
    int result = func();
    return make_int(result);
}

// Helper functions
UnifiedValue create_safe_sdl_rect(int x, int y, int w, int h) {
    auto buffer = SafeFFIManager::instance().allocate_buffer(sizeof(SafeSDLRect));
    SafeSDLRect* rect = buffer->template as<SafeSDLRect>();
    new (rect) SafeSDLRect(x, y, w, h);
    return make_pointer(static_cast<void*>(rect));
}

UnifiedValue create_safe_sdl_event() {
    auto buffer = SafeFFIManager::instance().allocate_buffer(sizeof(SafeSDLEvent));
    SafeSDLEvent* event = buffer->template as<SafeSDLEvent>();
    new (event) SafeSDLEvent();
    return make_pointer(static_cast<void*>(event));
}

UnifiedValue get_safe_event_type(const UnifiedValue& event) {
    void* ptr = SafeFFIManager::instance().extract_raw_pointer(event);
    SafeSDLEvent* sdl_event = static_cast<SafeSDLEvent*>(ptr);
    return make_int(static_cast<int>(sdl_event->type));
}

UnifiedValue get_safe_key_code(const UnifiedValue& event) {
    void* ptr = SafeFFIManager::instance().extract_raw_pointer(event);
    SafeSDLEvent* sdl_event = static_cast<SafeSDLEvent*>(ptr);
    
    if (auto* kb_event = sdl_event->as_keyboard()) {
        return make_int(static_cast<int>(kb_event->scancode));
    }
    
    return make_int(0);  // No key code for non-keyboard events
}

UnifiedValue get_safe_rect_field(const UnifiedValue& rect, const std::string& field) {
    void* ptr = SafeFFIManager::instance().extract_raw_pointer(rect);
    SafeSDLRect* sdl_rect = static_cast<SafeSDLRect*>(ptr);
    
    if (field == "x") return make_int(sdl_rect->x);
    if (field == "y") return make_int(sdl_rect->y);
    if (field == "w") return make_int(sdl_rect->w);
    if (field == "h") return make_int(sdl_rect->h);
    
    throw SafeFFIError("Unknown rect field: " + field);
}

} // namespace ffi
} // namespace rbasic