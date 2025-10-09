#pragma once
#include "../runtime/basic_runtime.h"

namespace basic_runtime {
    
    // Unified FFI type conversion utilities
    class FFITypeConverter {
    public:
        static void* getAsPointer(const BasicValue& val);
        static std::string getAsString(const BasicValue& val);
        static const char* getAsCString(const BasicValue& val);
        static int getAsInt(const BasicValue& val);
        static double getAsDouble(const BasicValue& val);
    };
    
}