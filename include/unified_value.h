#pragma once

#include <variant>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <type_traits>

// GLM includes for vector and matrix types
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace rbasic {

// Forward declarations
class UnifiedArray;
class UnifiedStruct;
class UnifiedPointer;

// GLM value wrappers
struct Vec2 {
    glm::vec2 data;
    Vec2() : data(0.0f) {}
    Vec2(float x, float y) : data(x, y) {}
    Vec2(const glm::vec2& v) : data(v) {}
    
    float& x() { return data.x; }
    float& y() { return data.y; }
    const float& x() const { return data.x; }
    const float& y() const { return data.y; }
};

struct Vec3 {
    glm::vec3 data;
    Vec3() : data(0.0f) {}
    Vec3(float x, float y, float z) : data(x, y, z) {}
    Vec3(const glm::vec3& v) : data(v) {}
    
    float& x() { return data.x; }
    float& y() { return data.y; }
    float& z() { return data.z; }
    const float& x() const { return data.x; }
    const float& y() const { return data.y; }
    const float& z() const { return data.z; }
};

struct Vec4 {
    glm::vec4 data;
    Vec4() : data(0.0f) {}
    Vec4(float x, float y, float z, float w) : data(x, y, z, w) {}
    Vec4(const glm::vec4& v) : data(v) {}
    
    float& x() { return data.x; }
    float& y() { return data.y; }
    float& z() { return data.z; }
    float& w() { return data.w; }
    const float& x() const { return data.x; }
    const float& y() const { return data.y; }
    const float& z() const { return data.z; }
    const float& w() const { return data.w; }
};

struct Mat3 {
    glm::mat3 data;
    Mat3() : data(1.0f) {}  // Identity matrix
    Mat3(const glm::mat3& m) : data(m) {}
};

struct Mat4 {
    glm::mat4 data;
    Mat4() : data(1.0f) {}  // Identity matrix
    Mat4(const glm::mat4& m) : data(m) {}
};

struct Quat {
    glm::quat data;
    Quat() : data(1.0f, 0.0f, 0.0f, 0.0f) {}  // Identity quaternion
    Quat(float w, float x, float y, float z) : data(w, x, y, z) {}
    Quat(const glm::quat& q) : data(q) {}
};

// Unified value type that works for both interpreter and runtime
using UnifiedValue = std::variant<
    int,
    double, 
    std::string,
    bool,
    void*,
    std::shared_ptr<UnifiedArray>,
    std::shared_ptr<UnifiedStruct>,
    std::shared_ptr<UnifiedPointer>,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4,
    Quat
>;

// Enhanced array type with type safety
class UnifiedArray {
public:
    enum class ElementType {
        INT, DOUBLE, STRING, BOOL, MIXED
    };
    
    UnifiedArray(ElementType type = ElementType::MIXED) : element_type_(type) {}
    UnifiedArray(const std::vector<int>& dimensions, ElementType type = ElementType::MIXED) 
        : dimensions_(dimensions), element_type_(type) {}
    
    // Type-safe element access
    UnifiedValue& at(const std::vector<int>& indices);
    const UnifiedValue& at(const std::vector<int>& indices) const;
    
    // Single dimension access (common case)
    UnifiedValue& operator[](int index);
    const UnifiedValue& operator[](int index) const;
    
    // Array properties
    size_t size() const { return elements_.size(); }
    const std::vector<int>& dimensions() const { return dimensions_; }
    ElementType element_type() const { return element_type_; }
    
    // Type-safe setters that enforce element type
    void set_element(const std::vector<int>& indices, const UnifiedValue& value);
    void set_element(int index, const UnifiedValue& value);
    
    // Resize operations
    void resize(const std::vector<int>& new_dimensions);
    void resize(int new_size);
    
private:
    std::map<int, UnifiedValue> elements_;
    std::vector<int> dimensions_;
    ElementType element_type_;
    
    int calculate_index(const std::vector<int>& indices) const;
    bool is_valid_type(const UnifiedValue& value) const;
};

// Enhanced struct type
class UnifiedStruct {
public:
    UnifiedStruct(const std::string& type_name) : type_name_(type_name) {}
    
    // Field access
    UnifiedValue& get_field(const std::string& name);
    const UnifiedValue& get_field(const std::string& name) const;
    void set_field(const std::string& name, const UnifiedValue& value);
    
    // Struct properties
    const std::string& type_name() const { return type_name_; }
    const std::map<std::string, UnifiedValue>& fields() const { return fields_; }
    bool has_field(const std::string& name) const;
    
private:
    std::string type_name_;
    std::map<std::string, UnifiedValue> fields_;
};

// Enhanced pointer type with type safety
class UnifiedPointer {
public:
    UnifiedPointer(void* ptr = nullptr, const std::string& type_name = "") 
        : ptr_(ptr), type_name_(type_name) {}
    
    void* get() const { return ptr_; }
    void set(void* ptr) { ptr_ = ptr; }
    
    const std::string& type_name() const { return type_name_; }
    void set_type_name(const std::string& name) { type_name_ = name; }
    
    bool is_null() const { return ptr_ == nullptr; }
    
    // Type-safe casting
    template<typename T>
    T* as() const {
        return static_cast<T*>(ptr_);
    }
    
private:
    void* ptr_;
    std::string type_name_;
};

// Type checking utilities
template<typename T>
bool holds_type(const UnifiedValue& value) {
    return std::holds_alternative<T>(value);
}

template<typename T>
T& get_value(UnifiedValue& value) {
    if (!std::holds_alternative<T>(value)) {
        throw std::runtime_error("Type mismatch in UnifiedValue access");
    }
    return std::get<T>(value);
}

template<typename T>
const T& get_value(const UnifiedValue& value) {
    if (!std::holds_alternative<T>(value)) {
        throw std::runtime_error("Type mismatch in UnifiedValue access");
    }
    return std::get<T>(value);
}

// Safe type conversion utilities
class TypeConverter {
public:
    static int to_int(const UnifiedValue& value);
    static double to_double(const UnifiedValue& value);
    static std::string to_string(const UnifiedValue& value);
    static bool to_bool(const UnifiedValue& value);
    static void* to_pointer(const UnifiedValue& value);
    
    // GLM conversions
    static Vec2 to_vec2(const UnifiedValue& value);
    static Vec3 to_vec3(const UnifiedValue& value);
    static Vec4 to_vec4(const UnifiedValue& value);
    
    // Type checking
    static bool is_numeric(const UnifiedValue& value);
    static bool is_vector(const UnifiedValue& value);
    static bool is_matrix(const UnifiedValue& value);
    static bool is_array(const UnifiedValue& value);
    static bool is_struct(const UnifiedValue& value);
    static bool is_pointer(const UnifiedValue& value);
};

// Arithmetic operations with type safety
class ValueOperations {
public:
    static UnifiedValue add(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue subtract(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue multiply(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue divide(const UnifiedValue& left, const UnifiedValue& right);
    
    static UnifiedValue compare(const UnifiedValue& left, const UnifiedValue& right, const std::string& op);
    static UnifiedValue logical_and(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue logical_or(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue logical_not(const UnifiedValue& value);
    
    // GLM-specific operations
    static UnifiedValue vector_length(const UnifiedValue& vector);
    static UnifiedValue vector_normalize(const UnifiedValue& vector);
    static UnifiedValue vector_dot(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue vector_cross(const UnifiedValue& left, const UnifiedValue& right);
    
private:
    static UnifiedValue add_vectors(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue subtract_vectors(const UnifiedValue& left, const UnifiedValue& right);
    static UnifiedValue multiply_vector_scalar(const UnifiedValue& vector, const UnifiedValue& scalar);
    static UnifiedValue multiply_vectors(const UnifiedValue& left, const UnifiedValue& right);
};

// Value creation helpers
UnifiedValue make_int(int value);
UnifiedValue make_double(double value);
UnifiedValue make_string(const std::string& value);
UnifiedValue make_bool(bool value);
UnifiedValue make_pointer(void* ptr, const std::string& type_name = "");
UnifiedValue make_vec2(float x, float y);
UnifiedValue make_vec3(float x, float y, float z);
UnifiedValue make_vec4(float x, float y, float z, float w);
std::shared_ptr<UnifiedArray> make_array(const std::vector<int>& dimensions = {}, UnifiedArray::ElementType type = UnifiedArray::ElementType::MIXED);
std::shared_ptr<UnifiedStruct> make_struct(const std::string& type_name);

// Error handling
class UnifiedValueError : public std::runtime_error {
public:
    explicit UnifiedValueError(const std::string& message) : std::runtime_error(message) {}
};

} // namespace rbasic