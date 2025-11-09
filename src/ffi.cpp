#include "ffi.h"
#include "common.h"
#include <iostream>
#include <algorithm>
#include <vector>

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
    // On Linux, try multiple approaches
    // 1. Try the name as-is first
    handle = dlopen(platform_name.c_str(), RTLD_LAZY);
    
    // 2. If that fails, try common system paths and versions for SDL2 libraries
    if (!handle) {
        std::vector<std::string> try_paths;
        
        if (name == "SDL2" || name.find("SDL2") == 0) {
            // SDL2 base library
            try_paths = {
                "/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0",
                "/usr/lib/libSDL2-2.0.so.0",
                "/usr/local/lib/libSDL2.so",
                "libSDL2-2.0.so.0",
                "libSDL2.so"
            };
        } else if (name == "SDL2_gfx") {
            // SDL2_gfx library
            try_paths = {
                "/usr/lib/x86_64-linux-gnu/libSDL2_gfx.so",
                "/usr/lib/x86_64-linux-gnu/libSDL2_gfx-1.0.so.0",
                "/usr/lib/libSDL2_gfx.so",
                "libSDL2_gfx.so"
            };
        } else if (name == "SDL2_image") {
            // SDL2_image library
            try_paths = {
                "/usr/lib/x86_64-linux-gnu/libSDL2_image.so",
                "/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so.0",
                "/usr/lib/libSDL2_image.so",
                "libSDL2_image.so"
            };
        }
        
        for (const auto& path : try_paths) {
            handle = dlopen(path.c_str(), RTLD_LAZY);
            if (handle) break;
        }
    }
    
    // 3. Try with RTLD_NOW if RTLD_LAZY failed
    if (!handle) {
        handle = dlopen(platform_name.c_str(), RTLD_NOW);
    }
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
    library_search_paths_.clear();
}

void FFIManager::add_library_search_path(const std::string& path) {
    library_search_paths_.push_back(path);
}

void FFIManager::clear_library_search_paths() {
    library_search_paths_.clear();
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
    
    // Handle common library mappings
    std::string mapped_name = base_name;
    
#ifdef _WIN32
    // Windows: SDL2.dll, sqlite3.dll, etc.
    if (mapped_name == "SDL2") {
        return "SDL2.dll";
    } else if (mapped_name == "SDL2_image") {
        return "SDL2_image.dll";
    } else if (mapped_name == "SDL2_gfx") {
        return "SDL2_gfx.dll";
    } else if (mapped_name == "sqlite3") {
        return "sqlite3.dll";
    }
    return mapped_name + ".dll";
#else
    // Linux: libSDL2.so, libsqlite3.so, etc.
    if (mapped_name == "SDL2" || mapped_name == "SDL2.dll") {
        return "libSDL2.so";
    } else if (mapped_name == "SDL2_image" || mapped_name == "SDL2_image.dll") {
        return "libSDL2_image.so";
    } else if (mapped_name == "SDL2_gfx" || mapped_name == "SDL2_gfx.dll") {
        return "libSDL2_gfx.so";
    } else if (mapped_name == "sqlite3" || mapped_name == "sqlite3.dll") {
        return "libsqlite3.so";
    }
    
    // Default Linux naming
    if (mapped_name.substr(0, 3) != "lib") {
        mapped_name = "lib" + mapped_name;
    }
    
#ifdef __APPLE__
    return mapped_name + ".dylib";
#else
    return mapped_name + ".so";
#endif
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