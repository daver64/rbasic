#include "type_utils.h"
#include <sstream>
#include <cmath>

namespace rbasic {

namespace TypeUtils {

int toInt(const ValueType& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return static_cast<int>(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1 : 0;
    } else if (std::holds_alternative<std::string>(value)) {
        const auto& str = std::get<std::string>(value);
        try {
            return std::stoi(str);
        } catch (const std::exception&) {
            throw ConversionError("Cannot convert string '" + str + "' to integer");
        }
    }
    throw ConversionError("Cannot convert value to integer");
}

double toDouble(const ValueType& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        const auto& str = std::get<std::string>(value);
        try {
            return std::stod(str);
        } catch (const std::exception&) {
            throw ConversionError("Cannot convert string '" + str + "' to double");
        }
    }
    throw ConversionError("Cannot convert value to double");
}

std::string toString(const ValueType& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<ArrayValue>(value)) {
        return "[Array]";
    } else if (std::holds_alternative<StructValue>(value)) {
        return "[Struct]";
    } else if (std::holds_alternative<ByteArrayValue>(value)) {
        return "[ByteArray]";
    } else if (std::holds_alternative<IntArrayValue>(value)) {
        return "[IntArray]";
    } else if (std::holds_alternative<DoubleArrayValue>(value)) {
        return "[DoubleArray]";
    }
    return "[Unknown]";
}

bool toBool(const ValueType& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        const auto& str = std::get<std::string>(value);
        return !str.empty() && str != "false" && str != "0";
    }
    return false; // Arrays and structs are considered true if they exist
}

int toArrayIndex(const ValueType& value) {
    int index = toInt(value);
    if (index < 0) {
        throw ConversionError("Array index cannot be negative: " + std::to_string(index));
    }
    return index;
}

template<>
int getValue<int>(const ValueType& value) {
    return toInt(value);
}

template<>
double getValue<double>(const ValueType& value) {
    return toDouble(value);
}

template<>
std::string getValue<std::string>(const ValueType& value) {
    return toString(value);
}

template<>
bool getValue<bool>(const ValueType& value) {
    return toBool(value);
}

template<>
uint8_t getValue<uint8_t>(const ValueType& value) {
    int intValue = toInt(value);
    if (intValue < 0 || intValue > 255) {
        throw ConversionError("Value " + std::to_string(intValue) + " is out of byte range (0-255)");
    }
    return static_cast<uint8_t>(intValue);
}

bool isNumeric(const ValueType& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<double>(value);
}

bool isArray(const ValueType& value) {
    return std::holds_alternative<ArrayValue>(value);
}

bool isTypedArray(const ValueType& value) {
    return std::holds_alternative<ByteArrayValue>(value) ||
           std::holds_alternative<IntArrayValue>(value) ||
           std::holds_alternative<DoubleArrayValue>(value);
}

size_t getArraySize(const ValueType& value) {
    if (std::holds_alternative<ArrayValue>(value)) {
        return std::get<ArrayValue>(value).elements.size();
    } else if (std::holds_alternative<ByteArrayValue>(value)) {
        return std::get<ByteArrayValue>(value).elements.size();
    } else if (std::holds_alternative<IntArrayValue>(value)) {
        return std::get<IntArrayValue>(value).elements.size();
    } else if (std::holds_alternative<DoubleArrayValue>(value)) {
        return std::get<DoubleArrayValue>(value).elements.size();
    }
    throw ConversionError("Value is not an array");
}

void validateArrayBounds(const ValueType& arrayValue, int index) {
    if (index < 0) {
        throw RuntimeError("Array index cannot be negative: " + std::to_string(index));
    }
    
    size_t arraySize = getArraySize(arrayValue);
    if (static_cast<size_t>(index) >= arraySize) {
        throw RuntimeError("Array index out of bounds: " + std::to_string(index) + 
                          " (array size: " + std::to_string(arraySize) + ")");
    }
}

} // namespace TypeUtils

} // namespace rbasic