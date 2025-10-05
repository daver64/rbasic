#pragma once

#include "../runtime/basic_runtime.h"
#include <string>
#include <unordered_map>
#include <memory>

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
class Library;
class FunctionSignature;

// Main FFI manager class
class FFIManager {
public:
    static FFIManager& instance();
    
    // Library management
    std::shared_ptr<Library> load_library(const std::string& name);
    bool unload_library(const std::string& name);
    std::shared_ptr<Library> get_library(const std::string& name);
    
    // Cleanup
    void cleanup();
    
private:
    FFIManager() = default;
    ~FFIManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Library>> loaded_libraries_;
};

// Represents a loaded dynamic library
class Library {
public:
    Library(const std::string& name, LibraryHandle handle);
    ~Library();
    
    // Get function address
    void* get_function_address(const std::string& function_name);
    
    // Library info
    const std::string& name() const { return name_; }
    bool is_valid() const { return handle_ != nullptr; }
    
private:
    std::string name_;
    LibraryHandle handle_;
};

// FFI error handling
class FFIError : public std::runtime_error {
public:
    explicit FFIError(const std::string& message) : std::runtime_error(message) {}
};

// Utility functions
std::string get_platform_library_name(const std::string& base_name);
std::string get_last_system_error();

} // namespace ffi
} // namespace rbasic