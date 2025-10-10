#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <functional>
#include <type_traits>

namespace rbasic {

// RAII-based memory manager for FFI and SDL resources
class MemoryManager {
public:
    // Smart pointer types for managed resources
    template<typename T>
    using managed_ptr = std::unique_ptr<T, std::function<void(std::remove_extent_t<T>*)>>;
    
    // Singleton access
    static MemoryManager& instance();
    
    // SDL resource management
    template<typename T>
    managed_ptr<T> allocate_sdl_resource(size_t size);
    
    // FFI buffer management
    managed_ptr<int> allocate_int_buffer();
    managed_ptr<void*> allocate_pointer_buffer();
    managed_ptr<uint8_t[]> allocate_buffer(size_t size);
    
    // Bounds-checked buffer access
    class SafeBuffer {
    public:
        SafeBuffer(std::unique_ptr<uint8_t[]> data, size_t size);
        
        // Bounds-checked access
        uint8_t& operator[](size_t index);
        const uint8_t& operator[](size_t index) const;
        
        // Safe pointer access with bounds checking
        template<typename T>
        T* as(size_t offset = 0);
        
        template<typename T>
        const T* as(size_t offset = 0) const;
        
        size_t size() const { return size_; }
        uint8_t* data() { return data_.get(); }
        const uint8_t* data() const { return data_.get(); }
        
    private:
        std::unique_ptr<uint8_t[]> data_;
        size_t size_;
    };
    
    // Create safe buffer
    std::shared_ptr<SafeBuffer> create_safe_buffer(size_t size);
    
    // Cleanup all managed resources
    void cleanup();
    
    // Get statistics
    size_t get_allocated_count() const;
    size_t get_total_memory_usage() const;
    
private:
    MemoryManager() = default;
    ~MemoryManager();
    
    // Thread-safe resource tracking
    mutable std::mutex mutex_;
    std::unordered_set<void*> tracked_resources_;
    size_t total_allocated_bytes_;
    
    // Private allocation helpers
    void track_resource(void* ptr, size_t size);
    void untrack_resource(void* ptr, size_t size);
};

// RAII wrapper for FFI function calls with automatic cleanup
class FFIScope {
public:
    FFIScope();
    ~FFIScope();
    
    // Allocate temporary buffer that gets cleaned up automatically
    template<typename T>
    T* allocate_temp(size_t count = 1);
    
    // Create temporary string buffer
    char* allocate_string_buffer(size_t size);
    
private:
    std::vector<std::unique_ptr<uint8_t[]>> temp_buffers_;
};

// Exception-safe SDL resource wrapper
template<typename T>
class SDLResource {
public:
    SDLResource() : resource_(nullptr), deleter_(nullptr) {}
    
    template<typename Deleter>
    SDLResource(T* resource, Deleter deleter) 
        : resource_(resource), deleter_([deleter](void* ptr) { deleter(static_cast<T*>(ptr)); }) {}
    
    ~SDLResource() {
        if (resource_ && deleter_) {
            deleter_(resource_);
        }
    }
    
    // Move semantics
    SDLResource(SDLResource&& other) noexcept 
        : resource_(other.resource_), deleter_(std::move(other.deleter_)) {
        other.resource_ = nullptr;
        other.deleter_ = nullptr;
    }
    
    SDLResource& operator=(SDLResource&& other) noexcept {
        if (this != &other) {
            reset();
            resource_ = other.resource_;
            deleter_ = std::move(other.deleter_);
            other.resource_ = nullptr;
            other.deleter_ = nullptr;
        }
        return *this;
    }
    
    // No copy semantics
    SDLResource(const SDLResource&) = delete;
    SDLResource& operator=(const SDLResource&) = delete;
    
    T* get() const { return resource_; }
    T* operator->() const { return resource_; }
    T& operator*() const { return *resource_; }
    
    explicit operator bool() const { return resource_ != nullptr; }
    
    T* release() {
        T* ptr = resource_;
        resource_ = nullptr;
        deleter_ = nullptr;
        return ptr;
    }
    
    void reset() {
        if (resource_ && deleter_) {
            deleter_(resource_);
        }
        resource_ = nullptr;
        deleter_ = nullptr;
    }
    
private:
    T* resource_;
    std::function<void(void*)> deleter_;
};

// Type-safe buffer access helpers
template<typename T>
class TypedBuffer {
public:
    TypedBuffer(std::shared_ptr<MemoryManager::SafeBuffer> buffer, size_t offset = 0)
        : buffer_(buffer), offset_(offset) {
        if (offset_ + sizeof(T) > buffer_->size()) {
            throw std::out_of_range("TypedBuffer offset exceeds buffer bounds");
        }
    }
    
    T& get() { return *buffer_->template as<T>(offset_); }
    const T& get() const { return *buffer_->template as<T>(offset_); }
    
    T& operator*() { return get(); }
    const T& operator*() const { return get(); }
    
    T* operator->() { return &get(); }
    const T* operator->() const { return &get(); }
    
private:
    std::shared_ptr<MemoryManager::SafeBuffer> buffer_;
    size_t offset_;
};

} // namespace rbasic