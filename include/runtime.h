#pragma once

#include "common.h"

namespace rbasic {

class Runtime {
public:
    // Built-in functions that both interpreter and compiled code can use
    static ValueType basicPrint(const std::vector<ValueType>& args);
    static ValueType basicInput(const std::vector<ValueType>& args);
    static ValueType basicLen(const std::vector<ValueType>& args);
    static ValueType basicMid(const std::vector<ValueType>& args);
    static ValueType basicLeft(const std::vector<ValueType>& args);
    static ValueType basicRight(const std::vector<ValueType>& args);
    static ValueType basicAbs(const std::vector<ValueType>& args);
    static ValueType basicSqrt(const std::vector<ValueType>& args);
    static ValueType basicInt(const std::vector<ValueType>& args);
    static ValueType basicRnd(const std::vector<ValueType>& args);
    
    // Type conversion functions
    static int toInt(const ValueType& value);
    static double toDouble(const ValueType& value);
    static std::string toString(const ValueType& value);
    static bool toBool(const ValueType& value);
};

} // namespace rbasic