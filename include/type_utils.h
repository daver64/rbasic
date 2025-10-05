#pragma once

#include "common.h"

namespace rbasic {

namespace TypeUtils {
    // Core type conversion functions
    int toInt(const ValueType& value);
    double toDouble(const ValueType& value);
    std::string toString(const ValueType& value);
    bool toBool(const ValueType& value);
    
    // Array index conversion with bounds checking
    int toArrayIndex(const ValueType& value);
    
    // Safe value extraction with type checking
    template<typename T>
    T getValue(const ValueType& value);
    
    // Type checking utilities
    bool isNumeric(const ValueType& value);
    bool isArray(const ValueType& value);
    bool isTypedArray(const ValueType& value);
    
    // Array utilities
    size_t getArraySize(const ValueType& value);
    void validateArrayBounds(const ValueType& arrayValue, int index);
    
    // Conversion with error context
    class ConversionError : public RuntimeError {
    public:
        ConversionError(const std::string& message) : RuntimeError("Type conversion error: " + message) {}
    };
    
} // namespace TypeUtils

} // namespace rbasic