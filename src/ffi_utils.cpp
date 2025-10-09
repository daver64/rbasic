#include "ffi_utils.h"

namespace basic_runtime {

void* FFITypeConverter::getAsPointer(const BasicValue& val) {
    if (std::holds_alternative<void*>(val)) {
        return std::get<void*>(val);
    }
    return nullptr;
}

std::string FFITypeConverter::getAsString(const BasicValue& val) {
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val);
    }
    return "";
}

const char* FFITypeConverter::getAsCString(const BasicValue& val) {
    if (std::holds_alternative<std::string>(val)) {
        // Note: This returns a pointer to internal string data
        // Should only be used immediately in function calls
        static thread_local std::string temp;
        temp = std::get<std::string>(val);
        return temp.c_str();
    }
    return nullptr;
}

int FFITypeConverter::getAsInt(const BasicValue& val) {
    if (std::holds_alternative<int>(val)) {
        return std::get<int>(val);
    } else if (std::holds_alternative<double>(val)) {
        return static_cast<int>(std::get<double>(val));
    }
    return 0;
}

double FFITypeConverter::getAsDouble(const BasicValue& val) {
    if (std::holds_alternative<double>(val)) {
        return std::get<double>(val);
    } else if (std::holds_alternative<int>(val)) {
        return static_cast<double>(std::get<int>(val));
    }
    return 0.0;
}

}