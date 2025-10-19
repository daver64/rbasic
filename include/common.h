#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <cstdint>

// GLM includes for vector and matrix types
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace rbasic {

// Forward declarations for complex types
struct StructValue;

struct ArrayValue {
    std::map<int, std::variant<int, double, std::string, bool, StructValue>> elements;
    std::vector<int> dimensions;
    
    ArrayValue() = default;
    ArrayValue(const std::vector<int>& dims) : dimensions(dims) {}
    
    // Multidimensional index calculation
    int calculateIndex(const std::vector<int>& indices) const {
        if (dimensions.empty()) {
            // 1D array case
            return indices.empty() ? 0 : indices[0];
        }
        
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            if (i < static_cast<int>(indices.size())) {
                index += indices[i] * multiplier;
            }
            multiplier *= dimensions[i];
        }
        
        return index;
    }
};

// Typed arrays for interpreter
struct ByteArrayValue {
    std::vector<uint8_t> elements;
    std::vector<int> dimensions;
    
    ByteArrayValue() = default;
    ByteArrayValue(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize, 0);
    }
    
    uint8_t& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const uint8_t& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

struct IntArrayValue {
    std::vector<int> elements;
    std::vector<int> dimensions;
    
    IntArrayValue() = default;
    IntArrayValue(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize, 0);
    }
    
    int& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const int& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

struct DoubleArrayValue {
    std::vector<double> elements;
    std::vector<int> dimensions;
    
    DoubleArrayValue() = default;
    DoubleArrayValue(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize, 0.0);
    }
    
    double& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const double& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

struct PointerValue {
    void* ptr;
    std::string typeName;  // Optional type information for FFI
    
    PointerValue() : ptr(nullptr) {}
    PointerValue(void* p, const std::string& type = "") : ptr(p), typeName(type) {}
};

struct StructValue {
    std::string typeName;
    std::map<std::string, std::variant<int, double, std::string, bool>> fields;
    
    StructValue() = default;
    StructValue(const std::string& type) : typeName(type) {}
};

// GLM value types for built-in vector and matrix support
struct Vec2Value {
    glm::vec2 data;
    Vec2Value() : data(0.0f) {}
    Vec2Value(float x, float y) : data(x, y) {}
    Vec2Value(const glm::vec2& v) : data(v) {}
};

struct Vec3Value {
    glm::vec3 data;
    Vec3Value() : data(0.0f) {}
    Vec3Value(float x, float y, float z) : data(x, y, z) {}
    Vec3Value(const glm::vec3& v) : data(v) {}
};

struct Vec4Value {
    glm::vec4 data;
    Vec4Value() : data(0.0f) {}
    Vec4Value(float x, float y, float z, float w) : data(x, y, z, w) {}
    Vec4Value(const glm::vec4& v) : data(v) {}
};

struct Mat3Value {
    glm::mat3 data;
    Mat3Value() : data(1.0f) {}  // Identity matrix
    Mat3Value(const glm::mat3& m) : data(m) {}
};

struct Mat4Value {
    glm::mat4 data;
    Mat4Value() : data(1.0f) {}  // Identity matrix
    Mat4Value(const glm::mat4& m) : data(m) {}
};

struct QuatValue {
    glm::quat data;
    QuatValue() : data(1.0f, 0.0f, 0.0f, 0.0f) {}  // Identity quaternion
    QuatValue(float w, float x, float y, float z) : data(w, x, y, z) {}
    QuatValue(const glm::quat& q) : data(q) {}
};

// Common types
using ValueType = std::variant<int, double, std::string, bool, void*, ArrayValue, StructValue, PointerValue, ByteArrayValue, IntArrayValue, DoubleArrayValue, Vec2Value, Vec3Value, Vec4Value, Mat3Value, Mat4Value, QuatValue>;

// Type system
enum class BasicType {
    INTEGER,
    DOUBLE,
    STRING,
    BOOLEAN,
    ARRAY,
    STRUCT,
    VEC2,
    VEC3,
    VEC4,
    MAT3,
    MAT4,
    QUAT,
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

// Source position tracking
struct SourcePosition {
    int line;
    int column;
    std::string filename;
    
    SourcePosition(int l = -1, int c = -1, std::string f = "") 
        : line(l), column(c), filename(std::move(f)) {}
    
    std::string toString() const {
        std::string result;
        if (!filename.empty()) {
            result += filename;
        }
        if (line >= 0) {
            if (!result.empty()) result += ":";
            result += std::to_string(line);
            if (column >= 0) {
                result += ":" + std::to_string(column);
            }
        }
        return result;
    }
    
    bool isValid() const {
        return line >= 0;
    }
};

// Error handling with source position
class RBasicError : public std::runtime_error {
protected:
    SourcePosition position_;
    
public:
    RBasicError(const std::string& message, const SourcePosition& pos = SourcePosition()) 
        : std::runtime_error(formatMessage(message, pos)), position_(pos) {}
    
    const SourcePosition& getPosition() const { return position_; }
    
private:
    static std::string formatMessage(const std::string& message, const SourcePosition& pos) {
        if (pos.isValid()) {
            return message + " at " + pos.toString();
        }
        return message;
    }
};

class SyntaxError : public RBasicError {
public:
    SyntaxError(const std::string& message, const SourcePosition& pos = SourcePosition()) 
        : RBasicError("Syntax error: " + message, pos) {}
    
    // Backwards compatibility
    SyntaxError(const std::string& message, int line) 
        : RBasicError("Syntax error: " + message, SourcePosition(line)) {}
};

class RuntimeError : public RBasicError {
public:
    RuntimeError(const std::string& message, const SourcePosition& pos = SourcePosition()) 
        : RBasicError("Runtime error: " + message, pos) {}
};

// Utility functions
std::string valueToString(const ValueType& value);
bool isTruthy(const ValueType& value);
ValueType addValues(const ValueType& left, const ValueType& right);
ValueType subtractValues(const ValueType& left, const ValueType& right);
ValueType multiplyValues(const ValueType& left, const ValueType& right);
ValueType divideValues(const ValueType& left, const ValueType& right);
ValueType compareValues(const ValueType& left, const ValueType& right, const std::string& op);

// Performance-optimized number parsing
inline bool hasDecimalPoint(const std::string& str) {
    for (char c : str) {
        if (c == '.') return true;
    }
    return false;
}

// Import resolution for compilation
struct ImportResolutionResult {
    std::string resolvedSource;
    std::vector<std::string> importedFiles;
    bool success;
    std::string errorMessage;
    
    ImportResolutionResult(bool s = false) : success(s) {}
};

ImportResolutionResult resolveImports(const std::string& source, const std::string& baseFile = "");
std::string resolveImportPath(const std::string& filename, const std::string& currentFile = "");

} // namespace rbasic