#include "unified_value.h"
#include <sstream>
#include <cmath>
#include <algorithm>

namespace rbasic {

// UnifiedArray implementation
UnifiedValue& UnifiedArray::at(const std::vector<int>& indices) {
    int index = calculate_index(indices);
    return elements_[index];
}

const UnifiedValue& UnifiedArray::at(const std::vector<int>& indices) const {
    int index = calculate_index(indices);
    auto it = elements_.find(index);
    if (it == elements_.end()) {
        static UnifiedValue default_value = make_int(0);
        return default_value;
    }
    return it->second;
}

UnifiedValue& UnifiedArray::operator[](int index) {
    return elements_[index];
}

const UnifiedValue& UnifiedArray::operator[](int index) const {
    auto it = elements_.find(index);
    if (it == elements_.end()) {
        static UnifiedValue default_value = make_int(0);
        return default_value;
    }
    return it->second;
}

void UnifiedArray::set_element(const std::vector<int>& indices, const UnifiedValue& value) {
    if (!is_valid_type(value)) {
        throw UnifiedValueError("Invalid type for array element");
    }
    int index = calculate_index(indices);
    elements_[index] = value;
}

void UnifiedArray::set_element(int index, const UnifiedValue& value) {
    if (!is_valid_type(value)) {
        throw UnifiedValueError("Invalid type for array element");
    }
    elements_[index] = value;
}

void UnifiedArray::resize(const std::vector<int>& new_dimensions) {
    dimensions_ = new_dimensions;
    // Keep existing elements that are still valid
}

void UnifiedArray::resize(int new_size) {
    dimensions_ = {new_size};
    // Keep existing elements that are still valid
}

int UnifiedArray::calculate_index(const std::vector<int>& indices) const {
    if (dimensions_.empty()) {
        return indices.empty() ? 0 : indices[0];
    }
    
    int index = 0;
    int multiplier = 1;
    
    for (int i = static_cast<int>(dimensions_.size()) - 1; i >= 0; i--) {
        if (i < static_cast<int>(indices.size())) {
            index += indices[i] * multiplier;
        }
        multiplier *= dimensions_[i];
    }
    
    return index;
}

bool UnifiedArray::is_valid_type(const UnifiedValue& value) const {
    switch (element_type_) {
        case ElementType::INT:
            return holds_type<int>(value);
        case ElementType::DOUBLE:
            return holds_type<double>(value);
        case ElementType::STRING:
            return holds_type<std::string>(value);
        case ElementType::BOOL:
            return holds_type<bool>(value);
        case ElementType::MIXED:
            return true;
    }
    return true;
}

// UnifiedStruct implementation
UnifiedValue& UnifiedStruct::get_field(const std::string& name) {
    return fields_[name];
}

const UnifiedValue& UnifiedStruct::get_field(const std::string& name) const {
    auto it = fields_.find(name);
    if (it == fields_.end()) {
        static UnifiedValue default_value = make_int(0);
        return default_value;
    }
    return it->second;
}

void UnifiedStruct::set_field(const std::string& name, const UnifiedValue& value) {
    fields_[name] = value;
}

bool UnifiedStruct::has_field(const std::string& name) const {
    return fields_.find(name) != fields_.end();
}

// TypeConverter implementation
int TypeConverter::to_int(const UnifiedValue& value) {
    if (holds_type<int>(value)) {
        return get_value<int>(value);
    } else if (holds_type<double>(value)) {
        return static_cast<int>(get_value<double>(value));
    } else if (holds_type<bool>(value)) {
        return get_value<bool>(value) ? 1 : 0;
    } else if (holds_type<std::string>(value)) {
        try {
            return std::stoi(get_value<std::string>(value));
        } catch (...) {
            return 0;
        }
    }
    throw UnifiedValueError("Cannot convert value to int");
}

double TypeConverter::to_double(const UnifiedValue& value) {
    if (holds_type<double>(value)) {
        return get_value<double>(value);
    } else if (holds_type<int>(value)) {
        return static_cast<double>(get_value<int>(value));
    } else if (holds_type<bool>(value)) {
        return get_value<bool>(value) ? 1.0 : 0.0;
    } else if (holds_type<std::string>(value)) {
        try {
            return std::stod(get_value<std::string>(value));
        } catch (...) {
            return 0.0;
        }
    }
    throw UnifiedValueError("Cannot convert value to double");
}

std::string TypeConverter::to_string(const UnifiedValue& value) {
    if (holds_type<std::string>(value)) {
        return get_value<std::string>(value);
    } else if (holds_type<int>(value)) {
        return std::to_string(get_value<int>(value));
    } else if (holds_type<double>(value)) {
        return std::to_string(get_value<double>(value));
    } else if (holds_type<bool>(value)) {
        return get_value<bool>(value) ? "true" : "false";
    } else if (holds_type<Vec2>(value)) {
        const auto& vec = get_value<Vec2>(value);
        return "vec2(" + std::to_string(vec.x()) + ", " + std::to_string(vec.y()) + ")";
    } else if (holds_type<Vec3>(value)) {
        const auto& vec = get_value<Vec3>(value);
        return "vec3(" + std::to_string(vec.x()) + ", " + std::to_string(vec.y()) + ", " + std::to_string(vec.z()) + ")";
    } else if (holds_type<Vec4>(value)) {
        const auto& vec = get_value<Vec4>(value);
        return "vec4(" + std::to_string(vec.x()) + ", " + std::to_string(vec.y()) + ", " + std::to_string(vec.z()) + ", " + std::to_string(vec.w()) + ")";
    }
    return "[Complex Value]";
}

bool TypeConverter::to_bool(const UnifiedValue& value) {
    if (holds_type<bool>(value)) {
        return get_value<bool>(value);
    } else if (holds_type<int>(value)) {
        return get_value<int>(value) != 0;
    } else if (holds_type<double>(value)) {
        return get_value<double>(value) != 0.0;
    } else if (holds_type<std::string>(value)) {
        return !get_value<std::string>(value).empty();
    }
    return false;
}

void* TypeConverter::to_pointer(const UnifiedValue& value) {
    if (holds_type<void*>(value)) {
        return get_value<void*>(value);
    } else if (holds_type<std::shared_ptr<UnifiedPointer>>(value)) {
        return get_value<std::shared_ptr<UnifiedPointer>>(value)->get();
    }
    throw UnifiedValueError("Cannot convert value to pointer");
}

Vec2 TypeConverter::to_vec2(const UnifiedValue& value) {
    if (holds_type<Vec2>(value)) {
        return get_value<Vec2>(value);
    }
    throw UnifiedValueError("Cannot convert value to Vec2");
}

Vec3 TypeConverter::to_vec3(const UnifiedValue& value) {
    if (holds_type<Vec3>(value)) {
        return get_value<Vec3>(value);
    }
    throw UnifiedValueError("Cannot convert value to Vec3");
}

Vec4 TypeConverter::to_vec4(const UnifiedValue& value) {
    if (holds_type<Vec4>(value)) {
        return get_value<Vec4>(value);
    }
    throw UnifiedValueError("Cannot convert value to Vec4");
}

bool TypeConverter::is_numeric(const UnifiedValue& value) {
    return holds_type<int>(value) || holds_type<double>(value);
}

bool TypeConverter::is_vector(const UnifiedValue& value) {
    return holds_type<Vec2>(value) || holds_type<Vec3>(value) || holds_type<Vec4>(value);
}

bool TypeConverter::is_matrix(const UnifiedValue& value) {
    return holds_type<Mat3>(value) || holds_type<Mat4>(value);
}

bool TypeConverter::is_array(const UnifiedValue& value) {
    return holds_type<std::shared_ptr<UnifiedArray>>(value);
}

bool TypeConverter::is_struct(const UnifiedValue& value) {
    return holds_type<std::shared_ptr<UnifiedStruct>>(value);
}

bool TypeConverter::is_pointer(const UnifiedValue& value) {
    return holds_type<void*>(value) || holds_type<std::shared_ptr<UnifiedPointer>>(value);
}

// ValueOperations implementation
UnifiedValue ValueOperations::add(const UnifiedValue& left, const UnifiedValue& right) {
    // Vector addition
    if (TypeConverter::is_vector(left) && TypeConverter::is_vector(right)) {
        return add_vectors(left, right);
    }
    
    // String concatenation
    if (holds_type<std::string>(left) || holds_type<std::string>(right)) {
        return make_string(TypeConverter::to_string(left) + TypeConverter::to_string(right));
    }
    
    // Numeric addition
    if (TypeConverter::is_numeric(left) && TypeConverter::is_numeric(right)) {
        if (holds_type<double>(left) || holds_type<double>(right)) {
            return make_double(TypeConverter::to_double(left) + TypeConverter::to_double(right));
        } else {
            return make_int(TypeConverter::to_int(left) + TypeConverter::to_int(right));
        }
    }
    
    throw UnifiedValueError("Invalid operands for addition");
}

UnifiedValue ValueOperations::subtract(const UnifiedValue& left, const UnifiedValue& right) {
    // Vector subtraction
    if (TypeConverter::is_vector(left) && TypeConverter::is_vector(right)) {
        return subtract_vectors(left, right);
    }
    
    // Numeric subtraction
    if (TypeConverter::is_numeric(left) && TypeConverter::is_numeric(right)) {
        if (holds_type<double>(left) || holds_type<double>(right)) {
            return make_double(TypeConverter::to_double(left) - TypeConverter::to_double(right));
        } else {
            return make_int(TypeConverter::to_int(left) - TypeConverter::to_int(right));
        }
    }
    
    throw UnifiedValueError("Invalid operands for subtraction");
}

UnifiedValue ValueOperations::multiply(const UnifiedValue& left, const UnifiedValue& right) {
    // Vector-scalar multiplication
    if (TypeConverter::is_vector(left) && TypeConverter::is_numeric(right)) {
        return multiply_vector_scalar(left, right);
    }
    if (TypeConverter::is_numeric(left) && TypeConverter::is_vector(right)) {
        return multiply_vector_scalar(right, left);
    }
    
    // Vector-vector multiplication (component-wise)
    if (TypeConverter::is_vector(left) && TypeConverter::is_vector(right)) {
        return multiply_vectors(left, right);
    }
    
    // Numeric multiplication
    if (TypeConverter::is_numeric(left) && TypeConverter::is_numeric(right)) {
        if (holds_type<double>(left) || holds_type<double>(right)) {
            return make_double(TypeConverter::to_double(left) * TypeConverter::to_double(right));
        } else {
            return make_int(TypeConverter::to_int(left) * TypeConverter::to_int(right));
        }
    }
    
    throw UnifiedValueError("Invalid operands for multiplication");
}

UnifiedValue ValueOperations::divide(const UnifiedValue& left, const UnifiedValue& right) {
    if (TypeConverter::is_numeric(left) && TypeConverter::is_numeric(right)) {
        double right_val = TypeConverter::to_double(right);
        if (right_val == 0.0) {
            throw UnifiedValueError("Division by zero");
        }
        return make_double(TypeConverter::to_double(left) / right_val);
    }
    
    throw UnifiedValueError("Invalid operands for division");
}

UnifiedValue ValueOperations::vector_length(const UnifiedValue& vector) {
    if (holds_type<Vec2>(vector)) {
        const auto& vec = get_value<Vec2>(vector);
        return make_double(glm::length(vec.data));
    } else if (holds_type<Vec3>(vector)) {
        const auto& vec = get_value<Vec3>(vector);
        return make_double(glm::length(vec.data));
    } else if (holds_type<Vec4>(vector)) {
        const auto& vec = get_value<Vec4>(vector);
        return make_double(glm::length(vec.data));
    }
    
    throw UnifiedValueError("Invalid operand for length()");
}

UnifiedValue ValueOperations::vector_normalize(const UnifiedValue& vector) {
    if (holds_type<Vec2>(vector)) {
        const auto& vec = get_value<Vec2>(vector);
        return make_vec2(glm::normalize(vec.data).x, glm::normalize(vec.data).y);
    } else if (holds_type<Vec3>(vector)) {
        const auto& vec = get_value<Vec3>(vector);
        glm::vec3 norm = glm::normalize(vec.data);
        return make_vec3(norm.x, norm.y, norm.z);
    } else if (holds_type<Vec4>(vector)) {
        const auto& vec = get_value<Vec4>(vector);
        glm::vec4 norm = glm::normalize(vec.data);
        return make_vec4(norm.x, norm.y, norm.z, norm.w);
    }
    
    throw UnifiedValueError("Invalid operand for normalize()");
}

UnifiedValue ValueOperations::vector_dot(const UnifiedValue& left, const UnifiedValue& right) {
    if (holds_type<Vec2>(left) && holds_type<Vec2>(right)) {
        const auto& left_vec = get_value<Vec2>(left);
        const auto& right_vec = get_value<Vec2>(right);
        return make_double(glm::dot(left_vec.data, right_vec.data));
    } else if (holds_type<Vec3>(left) && holds_type<Vec3>(right)) {
        const auto& left_vec = get_value<Vec3>(left);
        const auto& right_vec = get_value<Vec3>(right);
        return make_double(glm::dot(left_vec.data, right_vec.data));
    } else if (holds_type<Vec4>(left) && holds_type<Vec4>(right)) {
        const auto& left_vec = get_value<Vec4>(left);
        const auto& right_vec = get_value<Vec4>(right);
        return make_double(glm::dot(left_vec.data, right_vec.data));
    }
    
    throw UnifiedValueError("Invalid operands for dot()");
}

UnifiedValue ValueOperations::vector_cross(const UnifiedValue& left, const UnifiedValue& right) {
    if (holds_type<Vec3>(left) && holds_type<Vec3>(right)) {
        const auto& left_vec = get_value<Vec3>(left);
        const auto& right_vec = get_value<Vec3>(right);
        glm::vec3 result = glm::cross(left_vec.data, right_vec.data);
        return make_vec3(result.x, result.y, result.z);
    }
    
    throw UnifiedValueError("Cross product only supported for Vec3");
}

// Private helper implementations
UnifiedValue ValueOperations::add_vectors(const UnifiedValue& left, const UnifiedValue& right) {
    if (holds_type<Vec2>(left) && holds_type<Vec2>(right)) {
        const auto& left_vec = get_value<Vec2>(left);
        const auto& right_vec = get_value<Vec2>(right);
        glm::vec2 result = left_vec.data + right_vec.data;
        return make_vec2(result.x, result.y);
    } else if (holds_type<Vec3>(left) && holds_type<Vec3>(right)) {
        const auto& left_vec = get_value<Vec3>(left);
        const auto& right_vec = get_value<Vec3>(right);
        glm::vec3 result = left_vec.data + right_vec.data;
        return make_vec3(result.x, result.y, result.z);
    } else if (holds_type<Vec4>(left) && holds_type<Vec4>(right)) {
        const auto& left_vec = get_value<Vec4>(left);
        const auto& right_vec = get_value<Vec4>(right);
        glm::vec4 result = left_vec.data + right_vec.data;
        return make_vec4(result.x, result.y, result.z, result.w);
    }
    
    throw UnifiedValueError("Vector type mismatch in addition");
}

UnifiedValue ValueOperations::subtract_vectors(const UnifiedValue& left, const UnifiedValue& right) {
    if (holds_type<Vec2>(left) && holds_type<Vec2>(right)) {
        const auto& left_vec = get_value<Vec2>(left);
        const auto& right_vec = get_value<Vec2>(right);
        glm::vec2 result = left_vec.data - right_vec.data;
        return make_vec2(result.x, result.y);
    } else if (holds_type<Vec3>(left) && holds_type<Vec3>(right)) {
        const auto& left_vec = get_value<Vec3>(left);
        const auto& right_vec = get_value<Vec3>(right);
        glm::vec3 result = left_vec.data - right_vec.data;
        return make_vec3(result.x, result.y, result.z);
    } else if (holds_type<Vec4>(left) && holds_type<Vec4>(right)) {
        const auto& left_vec = get_value<Vec4>(left);
        const auto& right_vec = get_value<Vec4>(right);
        glm::vec4 result = left_vec.data - right_vec.data;
        return make_vec4(result.x, result.y, result.z, result.w);
    }
    
    throw UnifiedValueError("Vector type mismatch in subtraction");
}

UnifiedValue ValueOperations::multiply_vector_scalar(const UnifiedValue& vector, const UnifiedValue& scalar) {
    float scalar_val = static_cast<float>(TypeConverter::to_double(scalar));
    
    if (holds_type<Vec2>(vector)) {
        const auto& vec = get_value<Vec2>(vector);
        glm::vec2 result = vec.data * scalar_val;
        return make_vec2(result.x, result.y);
    } else if (holds_type<Vec3>(vector)) {
        const auto& vec = get_value<Vec3>(vector);
        glm::vec3 result = vec.data * scalar_val;
        return make_vec3(result.x, result.y, result.z);
    } else if (holds_type<Vec4>(vector)) {
        const auto& vec = get_value<Vec4>(vector);
        glm::vec4 result = vec.data * scalar_val;
        return make_vec4(result.x, result.y, result.z, result.w);
    }
    
    throw UnifiedValueError("Invalid vector type for scalar multiplication");
}

UnifiedValue ValueOperations::multiply_vectors(const UnifiedValue& left, const UnifiedValue& right) {
    if (holds_type<Vec2>(left) && holds_type<Vec2>(right)) {
        const auto& left_vec = get_value<Vec2>(left);
        const auto& right_vec = get_value<Vec2>(right);
        glm::vec2 result = left_vec.data * right_vec.data;
        return make_vec2(result.x, result.y);
    } else if (holds_type<Vec3>(left) && holds_type<Vec3>(right)) {
        const auto& left_vec = get_value<Vec3>(left);
        const auto& right_vec = get_value<Vec3>(right);
        glm::vec3 result = left_vec.data * right_vec.data;
        return make_vec3(result.x, result.y, result.z);
    } else if (holds_type<Vec4>(left) && holds_type<Vec4>(right)) {
        const auto& left_vec = get_value<Vec4>(left);
        const auto& right_vec = get_value<Vec4>(right);
        glm::vec4 result = left_vec.data * right_vec.data;
        return make_vec4(result.x, result.y, result.z, result.w);
    }
    
    throw UnifiedValueError("Vector type mismatch in multiplication");
}

// Value creation helpers
UnifiedValue make_int(int value) {
    return UnifiedValue(value);
}

UnifiedValue make_double(double value) {
    return UnifiedValue(value);
}

UnifiedValue make_string(const std::string& value) {
    return UnifiedValue(value);
}

UnifiedValue make_bool(bool value) {
    return UnifiedValue(value);
}

UnifiedValue make_pointer(void* ptr, const std::string& type_name) {
    return UnifiedValue(std::make_shared<UnifiedPointer>(ptr, type_name));
}

UnifiedValue make_vec2(float x, float y) {
    return UnifiedValue(Vec2(x, y));
}

UnifiedValue make_vec3(float x, float y, float z) {
    return UnifiedValue(Vec3(x, y, z));
}

UnifiedValue make_vec4(float x, float y, float z, float w) {
    return UnifiedValue(Vec4(x, y, z, w));
}

std::shared_ptr<UnifiedArray> make_array(const std::vector<int>& dimensions, UnifiedArray::ElementType type) {
    return std::make_shared<UnifiedArray>(dimensions, type);
}

std::shared_ptr<UnifiedStruct> make_struct(const std::string& type_name) {
    return std::make_shared<UnifiedStruct>(type_name);
}

} // namespace rbasic