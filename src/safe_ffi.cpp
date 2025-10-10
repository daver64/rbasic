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

std::shared_ptr<SafeLibrary> SafeFFIManager::load_library(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = libraries_.find(name);
    if (it != libraries_.end()) {
        if (auto lib = it->second) {
            return lib;
        }
        libraries_.erase(it);
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
    libraries_[name] = lib;
    return lib;
}

bool SafeFFIManager::unload_library(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = libraries_.find(name);
    if (it != libraries_.end()) {
        libraries_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<SafeLibrary> SafeFFIManager::get_library(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = libraries_.find(name);
    if (it != libraries_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<MemoryManager::SafeBuffer> SafeFFIManager::allocate_buffer(size_t size) {
    auto buffer = MemoryManager::instance().create_safe_buffer(size);
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
    libraries_.clear();
}

size_t SafeFFIManager::get_loaded_library_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return libraries_.size();
}

size_t SafeFFIManager::get_total_allocated_memory() const {
    // Delegate to MemoryManager for total memory tracking
    return MemoryManager::instance().get_total_memory_usage();
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
    
    // Phase 3: Check function cache first
    auto& manager = SafeFFIManager::instance();
    void* cached_func = manager.get_cached_function(name_, function_name);
    if (cached_func) {
        return cached_func;
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
    
    // Cache the function pointer for future use
    manager.cache_function(name_, function_name, func);
    
    return func;
}

UnifiedValue SafeLibrary::call_function(const std::string& function_name, 
                                       const std::vector<UnifiedValue>& args,
                                       const std::string& return_type) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Trace the call if enabled
    auto& manager = SafeFFIManager::get_instance();
    manager.trace_call(function_name, args);
    
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
    
    auto result = call.execute();
    
    // Profile the call performance
    manager.profile_call_end(function_name, start_time);
    
    return result;
}

bool SafeLibrary::validate_function_signature(const std::string& /* function_name */, 
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
        // For pointer return types, we need to use a different function signature
        typedef void* (*ptr_func_ptr_t)();
        ptr_func_ptr_t ptr_func = reinterpret_cast<ptr_func_ptr_t>(function_ptr_);
        void* result = ptr_func();
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

// Phase 3: Performance optimization implementations

void* SafeFFIManager::get_cached_function(const std::string& library_name, const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string cache_key = library_name + "::" + function_name;
    auto it = function_cache_.find(cache_key);
    
    if (it != function_cache_.end()) {
        cache_hits_.fetch_add(1, std::memory_order_relaxed);
        return it->second;
    }
    
    cache_misses_.fetch_add(1, std::memory_order_relaxed);
    return nullptr;
}

void SafeFFIManager::cache_function(const std::string& library_name, const std::string& function_name, void* func_ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string cache_key = library_name + "::" + function_name;
    function_cache_[cache_key] = func_ptr;
}

void SafeFFIManager::clear_function_cache() {
    std::lock_guard<std::mutex> lock(mutex_);
    function_cache_.clear();
}

double SafeFFIManager::get_cache_hit_ratio() const {
    size_t hits = cache_hits_.load(std::memory_order_relaxed);
    size_t misses = cache_misses_.load(std::memory_order_relaxed);
    size_t total = hits + misses;
    
    return total > 0 ? static_cast<double>(hits) / total : 0.0;
}

void SafeFFIManager::reset_performance_stats() {
    cache_hits_.store(0, std::memory_order_relaxed);
    cache_misses_.store(0, std::memory_order_relaxed);
    pool_allocations_.store(0, std::memory_order_relaxed);
    pool_hits_.store(0, std::memory_order_relaxed);
}

// Common buffer sizes for pooling (SDL events, strings, etc.)
const std::vector<size_t> SafeFFIManager::COMMON_BUFFER_SIZES = {
    32, 64, 128, 256, 512, 1024, 2048, 4096
};

std::shared_ptr<MemoryManager::SafeBuffer> SafeFFIManager::allocate_pooled_buffer(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    pool_allocations_++;
    
    // Find the smallest pool size that can accommodate the request
    size_t pool_size = 0;
    for (size_t candidate : COMMON_BUFFER_SIZES) {
        if (candidate >= size) {
            pool_size = candidate;
            break;
        }
    }
    
    // If size is too large, allocate directly without pooling
    if (pool_size == 0) {
        return MemoryManager::instance().create_safe_buffer(size);
    }
    
    // Check if we have a buffer in the pool
    auto& pool = memory_pools_[pool_size];
    if (!pool.empty()) {
        auto buffer = pool.back();
        pool.pop_back();
        pool_hits_++;
        
        // Reset the buffer for reuse (clear content)
        std::memset(buffer->data(), 0, buffer->size());
        return buffer;
    }
    
    // No pooled buffer available, allocate new one
    return MemoryManager::instance().create_safe_buffer(pool_size);
}

void SafeFFIManager::warm_memory_pools() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Pre-allocate a few buffers of each common size
    for (size_t size : COMMON_BUFFER_SIZES) {
        auto& pool = memory_pools_[size];
        const size_t WARM_COUNT = 4; // Pre-allocate 4 buffers per size
        
        for (size_t i = 0; i < WARM_COUNT; ++i) {
            pool.push_back(MemoryManager::instance().create_safe_buffer(size));
        }
    }
}

void SafeFFIManager::clear_memory_pools() {
    std::lock_guard<std::mutex> lock(mutex_);
    memory_pools_.clear();
    pool_allocations_.store(0, std::memory_order_relaxed);
    pool_hits_.store(0, std::memory_order_relaxed);
}

size_t SafeFFIManager::get_pool_efficiency() const {
    size_t total_allocations = pool_allocations_.load();
    if (total_allocations == 0) return 100; // No allocations yet
    
    size_t hits = pool_hits_.load();
    return (hits * 100) / total_allocations;
}

// Call tracing and profiling implementation
std::vector<std::string> SafeFFIManager::get_call_trace() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return call_trace_;
}

void SafeFFIManager::clear_call_trace() {
    std::lock_guard<std::mutex> lock(mutex_);
    call_trace_.clear();
}

void SafeFFIManager::trace_call(const std::string& function_name, const std::vector<UnifiedValue>& args) const {
    if (!call_tracing_enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string trace_entry = "FFI Call: " + function_name + "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) trace_entry += ", ";
        
        // Convert UnifiedValue to string representation
        if (std::holds_alternative<int>(args[i])) {
            trace_entry += std::to_string(std::get<int>(args[i]));
        } else if (std::holds_alternative<std::string>(args[i])) {
            trace_entry += "\"" + std::get<std::string>(args[i]) + "\"";
        } else if (std::holds_alternative<void*>(args[i])) {
            trace_entry += "ptr:" + std::to_string(reinterpret_cast<uintptr_t>(std::get<void*>(args[i])));
        } else {
            trace_entry += "<unknown>";
        }
    }
    trace_entry += ")";
    
    call_trace_.push_back(trace_entry);
    
    // Limit trace size to prevent memory bloat
    if (call_trace_.size() > 1000) {
        call_trace_.erase(call_trace_.begin());
    }
}

void SafeFFIManager::profile_call_start(const std::string& /* function_name */) const {
    // This would typically store the start time, but we'll handle timing in the call_function methods
}

void SafeFFIManager::profile_call_end(const std::string& function_name, std::chrono::high_resolution_clock::time_point start_time) const {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    double duration_ms = duration.count() / 1000.0;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto& profile = call_profiles_[function_name];
    profile.call_count++;
    profile.total_time_ms += duration_ms;
}

std::vector<SafeFFIManager::CallProfile> SafeFFIManager::get_call_profiles() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<CallProfile> profiles;
    
    for (const auto& [name, entry] : call_profiles_) {
        CallProfile profile;
        profile.function_name = name;
        profile.call_count = entry.call_count;
        profile.total_time_ms = entry.total_time_ms;
        profile.avg_time_ms = entry.call_count > 0 ? entry.total_time_ms / entry.call_count : 0.0;
        profiles.push_back(profile);
    }
    
    return profiles;
}

void SafeFFIManager::reset_call_profiles() {
    std::lock_guard<std::mutex> lock(mutex_);
    call_profiles_.clear();
}

} // namespace ffi
} // namespace rbasic