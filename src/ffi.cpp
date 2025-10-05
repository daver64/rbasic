#include "ffi.h"
#include "common.h"
#include <iostream>
#include <algorithm>

namespace rbasic {
namespace ffi {

// FFIManager implementation
FFIManager& FFIManager::instance() {
    static FFIManager instance;
    return instance;
}

std::shared_ptr<Library> FFIManager::load_library(const std::string& name) {
    // Check if already loaded
    auto it = loaded_libraries_.find(name);
    if (it != loaded_libraries_.end()) {
        return it->second;
    }
    
    // Get platform-specific library name
    std::string platform_name = get_platform_library_name(name);
    
    // Load the library
    LibraryHandle handle = nullptr;
    
#ifdef _WIN32
    handle = LoadLibraryA(platform_name.c_str());
#else
    handle = dlopen(platform_name.c_str(), RTLD_LAZY);
#endif
    
    if (!handle) {
        throw FFIError("Failed to load library '" + name + "': " + get_last_system_error());
    }
    
    // Create Library object
    auto library = std::make_shared<Library>(name, handle);
    loaded_libraries_[name] = library;
    
    return library;
}

bool FFIManager::unload_library(const std::string& name) {
    auto it = loaded_libraries_.find(name);
    if (it == loaded_libraries_.end()) {
        return false;
    }
    
    loaded_libraries_.erase(it);
    return true;
}

std::shared_ptr<Library> FFIManager::get_library(const std::string& name) {
    auto it = loaded_libraries_.find(name);
    if (it != loaded_libraries_.end()) {
        return it->second;
    }
    return nullptr;
}

void FFIManager::cleanup() {
    loaded_libraries_.clear();
}

// Library implementation
Library::Library(const std::string& name, LibraryHandle handle) 
    : name_(name), handle_(handle) {
}

Library::~Library() {
    if (handle_) {
#ifdef _WIN32
        FreeLibrary(handle_);
#else
        dlclose(handle_);
#endif
    }
}

void* Library::get_function_address(const std::string& function_name) {
    if (!handle_) {
        return nullptr;
    }
    
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(handle_, function_name.c_str()));
#else
    return dlsym(handle_, function_name.c_str());
#endif
}

// Utility functions
std::string get_platform_library_name(const std::string& base_name) {
    // If already has extension, use as-is
    if (base_name.find('.') != std::string::npos) {
        return base_name;
    }
    
#ifdef _WIN32
    return base_name + ".dll";
#elif defined(__APPLE__)
    return "lib" + base_name + ".dylib";
#else
    return "lib" + base_name + ".so";
#endif
}

std::string get_last_system_error() {
#ifdef _WIN32
    DWORD error = GetLastError();
    if (error == 0) {
        return "No error";
    }
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, nullptr);
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    
    // Remove trailing newlines
    message.erase(std::find_if(message.rbegin(), message.rend(), 
                              [](unsigned char ch) { return !std::isspace(ch); }).base(), 
                  message.end());
    
    return message;
#else
    const char* error = dlerror();
    return error ? std::string(error) : "No error";
#endif
}

} // namespace ffi
} // namespace rbasic