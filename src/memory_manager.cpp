#include "memory_manager.h"
#include "safe_ffi.h"
#include <stdexcept>
#include <algorithm>
#include <string>
#include <sstream>

namespace rbasic {

// MemoryManager implementation
MemoryManager& MemoryManager::instance() {
    static MemoryManager instance;
    return instance;
}

MemoryManager::~MemoryManager() {
    cleanup();
}

template<typename T>
MemoryManager::managed_ptr<T> MemoryManager::allocate_sdl_resource(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto* ptr = new uint8_t[size];
    auto* typed_ptr = reinterpret_cast<T*>(ptr);
    
    track_resource(ptr, size);
    
    return managed_ptr<T>(typed_ptr, [this, size](T* p) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto* raw_ptr = reinterpret_cast<uint8_t*>(p);
        untrack_resource(raw_ptr, size);
        delete[] raw_ptr;
    });
}

MemoryManager::managed_ptr<int> MemoryManager::allocate_int_buffer() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto* ptr = new int(0);
    track_resource(ptr, sizeof(int));
    
    return managed_ptr<int>(ptr, [this](int* p) {
        std::lock_guard<std::mutex> lock(mutex_);
        untrack_resource(p, sizeof(int));
        delete p;
    });
}

MemoryManager::managed_ptr<void*> MemoryManager::allocate_pointer_buffer() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto* ptr = new void*(nullptr);
    track_resource(ptr, sizeof(void*));
    
    return managed_ptr<void*>(ptr, [this](void** p) {
        std::lock_guard<std::mutex> lock(mutex_);
        untrack_resource(p, sizeof(void*));
        delete p;
    });
}

MemoryManager::managed_ptr<uint8_t[]> MemoryManager::allocate_buffer(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto ptr = std::make_unique<uint8_t[]>(size);
    auto* raw_ptr = ptr.get();
    track_resource(raw_ptr, size);
    
    // Create the deleter with correct signature for array type
    auto deleter = [this, size](uint8_t* p) {
        std::lock_guard<std::mutex> lock(mutex_);
        untrack_resource(p, size);
        delete[] p;
    };
    
    return managed_ptr<uint8_t[]>(ptr.release(), deleter);
}

std::shared_ptr<MemoryManager::SafeBuffer> MemoryManager::create_safe_buffer(size_t size) {
    auto data = std::make_unique<uint8_t[]>(size);
    return std::make_shared<SafeBuffer>(std::move(data), size);
}

void MemoryManager::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Resources will be automatically cleaned up by their destructors
    // Just clear our tracking
    tracked_resources_.clear();
    total_allocated_bytes_ = 0;
}

size_t MemoryManager::get_allocated_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tracked_resources_.size();
}

size_t MemoryManager::get_total_memory_usage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_allocated_bytes_;
}

void MemoryManager::track_resource(void* ptr, size_t size) {
    tracked_resources_.insert(ptr);
    total_allocated_bytes_ += size;
}

void MemoryManager::untrack_resource(void* ptr, size_t size) {
    tracked_resources_.erase(ptr);
    total_allocated_bytes_ -= size;
}

// SafeBuffer implementation
MemoryManager::SafeBuffer::SafeBuffer(std::unique_ptr<uint8_t[]> data, size_t size)
    : data_(std::move(data)), size_(size) {}

uint8_t& MemoryManager::SafeBuffer::operator[](size_t index) {
    if (index >= size_) {
        std::ostringstream oss;
        oss << "Buffer index out of bounds: " << index << " >= " << size_;
        throw std::out_of_range(oss.str());
    }
    return data_[index];
}

const uint8_t& MemoryManager::SafeBuffer::operator[](size_t index) const {
    if (index >= size_) {
        std::ostringstream oss;
        oss << "Buffer index out of bounds: " << index << " >= " << size_;
        throw std::out_of_range(oss.str());
    }
    return data_[index];
}

template<typename T>
T* MemoryManager::SafeBuffer::as(size_t offset) {
    if (offset + sizeof(T) > size_) {
        std::ostringstream oss;
        oss << "Buffer access out of bounds: offset " << offset << " + size " << sizeof(T) << " > " << size_;
        throw std::out_of_range(oss.str());
    }
    return reinterpret_cast<T*>(data_.get() + offset);
}

template<typename T>
const T* MemoryManager::SafeBuffer::as(size_t offset) const {
    if (offset + sizeof(T) > size_) {
        std::ostringstream oss;
        oss << "Buffer access out of bounds: offset " << offset << " + size " << sizeof(T) << " > " << size_;
        throw std::out_of_range(oss.str());
    }
    return reinterpret_cast<const T*>(data_.get() + offset);
}

// FFIScope implementation
FFIScope::FFIScope() = default;

FFIScope::~FFIScope() {
    // temp_buffers_ will be automatically cleaned up
}

template<typename T>
T* FFIScope::allocate_temp(size_t count) {
    size_t size = sizeof(T) * count;
    auto buffer = std::make_unique<uint8_t[]>(size);
    T* ptr = reinterpret_cast<T*>(buffer.get());
    temp_buffers_.push_back(std::move(buffer));
    return ptr;
}

char* FFIScope::allocate_string_buffer(size_t size) {
    return allocate_temp<char>(size);
}

} // namespace rbasic

// Explicit template instantiations for SDL types
template rbasic::ffi::SafeSDLRect* rbasic::MemoryManager::SafeBuffer::as<rbasic::ffi::SafeSDLRect>(size_t);
template rbasic::ffi::SafeSDLEvent* rbasic::MemoryManager::SafeBuffer::as<rbasic::ffi::SafeSDLEvent>(size_t);