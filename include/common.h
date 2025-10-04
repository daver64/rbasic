#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>

namespace rbasic {

// Common types
using ValueType = std::variant<int, double, std::string, bool>;

// Type system
enum class BasicType {
    INTEGER,
    DOUBLE,
    STRING,
    BOOLEAN,
    ARRAY,
    STRUCT,
    UNKNOWN
};

struct TypeInfo {
    BasicType type;
    std::string structName;  // For struct types
    std::vector<int> dimensions; // For array types
    
    TypeInfo(BasicType t = BasicType::UNKNOWN, std::string sn = "", std::vector<int> dims = {})
        : type(t), structName(std::move(sn)), dimensions(std::move(dims)) {}
};

// Forward declarations
class ASTNode;
class Value;
class FunctionDecl;
class StructDecl;

// Error handling
class RBasicError : public std::runtime_error {
public:
    RBasicError(const std::string& message) : std::runtime_error(message) {}
};

class SyntaxError : public RBasicError {
public:
    SyntaxError(const std::string& message, int line = -1) 
        : RBasicError("Syntax error: " + message + (line >= 0 ? " at line " + std::to_string(line) : "")) {}
};

class RuntimeError : public RBasicError {
public:
    RuntimeError(const std::string& message) : RBasicError("Runtime error: " + message) {}
};

// Utility functions
std::string valueToString(const ValueType& value);
bool isTruthy(const ValueType& value);
ValueType addValues(const ValueType& left, const ValueType& right);
ValueType subtractValues(const ValueType& left, const ValueType& right);
ValueType multiplyValues(const ValueType& left, const ValueType& right);
ValueType divideValues(const ValueType& left, const ValueType& right);
ValueType compareValues(const ValueType& left, const ValueType& right, const std::string& op);

} // namespace rbasic