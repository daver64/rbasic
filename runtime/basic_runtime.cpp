#include "basic_runtime.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace basic_runtime {

void init_runtime() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void print(const BasicValue& value) {
    std::cout << to_string(value);
}

void print_line() {
    std::cout << std::endl;
}

BasicValue input() {
    std::string line;
    std::getline(std::cin, line);
    
    // Try to parse as number
    try {
        if (line.find('.') != std::string::npos) {
            return std::stod(line);
        } else {
            return std::stoi(line);
        }
    } catch (const std::exception&) {
        return line; // Return as string if not a number
    }
}

int len(const BasicValue& str) {
    return static_cast<int>(to_string(str).length());
}

BasicValue mid(const BasicValue& str, int start, int length) {
    std::string s = to_string(str);
    start = std::max(0, start - 1); // BASIC is 1-indexed
    
    if (length == -1) {
        return s.substr(start);
    } else {
        return s.substr(start, length);
    }
}

BasicValue left(const BasicValue& str, int length) {
    std::string s = to_string(str);
    return s.substr(0, length);
}

BasicValue right(const BasicValue& str, int length) {
    std::string s = to_string(str);
    int start = std::max(0, static_cast<int>(s.length()) - length);
    return s.substr(start);
}

BasicValue abs_val(const BasicValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::abs(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::abs(std::get<double>(value));
    }
    return 0;
}

BasicValue sqrt_val(const BasicValue& value) {
    return std::sqrt(to_double(value));
}

BasicValue sqr_val(const BasicValue& value) {
    return std::sqrt(to_double(value));
}

BasicValue sin_val(const BasicValue& value) {
    return std::sin(to_double(value));
}

BasicValue cos_val(const BasicValue& value) {
    return std::cos(to_double(value));
}

BasicValue tan_val(const BasicValue& value) {
    return std::tan(to_double(value));
}

BasicValue asin_val(const BasicValue& value) {
    return std::asin(to_double(value));
}

BasicValue acos_val(const BasicValue& value) {
    return std::acos(to_double(value));
}

BasicValue atan_val(const BasicValue& value) {
    return std::atan(to_double(value));
}

BasicValue atan2_val(const BasicValue& y, const BasicValue& x) {
    return std::atan2(to_double(y), to_double(x));
}

BasicValue log_val(const BasicValue& value) {
    double val = to_double(value);
    if (val <= 0) {
        throw std::runtime_error("LOG requires a positive argument");
    }
    return std::log(val);
}

BasicValue log10_val(const BasicValue& value) {
    double val = to_double(value);
    if (val <= 0) {
        throw std::runtime_error("LOG10 requires a positive argument");
    }
    return std::log10(val);
}

BasicValue exp_val(const BasicValue& value) {
    return std::exp(to_double(value));
}

BasicValue pow_val(const BasicValue& base, const BasicValue& exp) {
    return std::pow(to_double(base), to_double(exp));
}

BasicValue floor_val(const BasicValue& value) {
    return std::floor(to_double(value));
}

BasicValue ceil_val(const BasicValue& value) {
    return std::ceil(to_double(value));
}

BasicValue round_val(const BasicValue& value) {
    return std::round(to_double(value));
}

BasicValue pi_val() {
    return 3.141592653589793;
}

BasicValue mod_val(const BasicValue& left, const BasicValue& right) {
    int leftInt = to_int(left);
    int rightInt = to_int(right);
    if (rightInt == 0) {
        throw std::runtime_error("Modulo by zero");
    }
    return leftInt % rightInt;
}

BasicValue int_val(const BasicValue& value) {
    return to_int(value);
}

BasicValue rnd() {
    return static_cast<double>(std::rand()) / RAND_MAX;
}

BasicArray create_array(const std::vector<int>& dimensions) {
    return BasicArray(dimensions);
}

BasicValue get_array_element(const BasicArray& array, const std::vector<int>& indices) {
    return array.at(indices);
}

void set_array_element(BasicArray& array, const std::vector<int>& indices, const BasicValue& value) {
    array.at(indices) = value;
}

BasicStruct create_struct(const std::string& typeName) {
    return BasicStruct(typeName);
}

BasicValue get_struct_field(const BasicStruct& struct_, const std::string& fieldName) {
    auto it = struct_.fields.find(fieldName);
    if (it != struct_.fields.end()) {
        return it->second;
    }
    return 0; // Default value if field not found
}

void set_struct_field(BasicStruct& struct_, const std::string& fieldName, const BasicValue& value) {
    struct_.fields[fieldName] = value;
}

int to_int(const BasicValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return static_cast<int>(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1 : 0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stoi(std::get<std::string>(value));
        } catch (const std::exception&) {
            return 0;
        }
    }
    return 0;
}

double to_double(const BasicValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value));
        } catch (const std::exception&) {
            return 0.0;
        }
    }
    return 0.0;
}

std::string to_string(const BasicValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<BasicStruct>(value)) {
        return "[struct " + std::get<BasicStruct>(value).typeName + "]";
    } else if (std::holds_alternative<BasicArray>(value)) {
        return "[array]";
    }
    return "";
}

bool to_bool(const BasicValue& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    }
    return false;
}

BasicValue add(const BasicValue& left, const BasicValue& right) {
    if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
        return to_string(left) + to_string(right);
    } else if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) + to_double(right);
    } else {
        return to_int(left) + to_int(right);
    }
}

BasicValue subtract(const BasicValue& left, const BasicValue& right) {
    if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) - to_double(right);
    } else {
        return to_int(left) - to_int(right);
    }
}

BasicValue multiply(const BasicValue& left, const BasicValue& right) {
    if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) * to_double(right);
    } else {
        return to_int(left) * to_int(right);
    }
}

BasicValue divide(const BasicValue& left, const BasicValue& right) {
    double rightVal = to_double(right);
    if (rightVal == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return to_double(left) / rightVal;
}

bool equal(const BasicValue& left, const BasicValue& right) {
    // Handle same types
    if (left.index() == right.index()) {
        if (std::holds_alternative<int>(left)) {
            return std::get<int>(left) == std::get<int>(right);
        } else if (std::holds_alternative<double>(left)) {
            return std::get<double>(left) == std::get<double>(right);
        } else if (std::holds_alternative<std::string>(left)) {
            return std::get<std::string>(left) == std::get<std::string>(right);
        } else if (std::holds_alternative<bool>(left)) {
            return std::get<bool>(left) == std::get<bool>(right);
        } else {
            // For struct and array types, compare as strings for now
            return to_string(left) == to_string(right);
        }
    }
    
    // Different types, try to compare as numbers or strings
    if ((std::holds_alternative<int>(left) || std::holds_alternative<double>(left)) &&
        (std::holds_alternative<int>(right) || std::holds_alternative<double>(right))) {
        return to_double(left) == to_double(right);
    }
    return to_string(left) == to_string(right);
}

bool not_equal(const BasicValue& left, const BasicValue& right) {
    return !equal(left, right);
}

bool less_than(const BasicValue& left, const BasicValue& right) {
    if ((std::holds_alternative<int>(left) || std::holds_alternative<double>(left)) &&
        (std::holds_alternative<int>(right) || std::holds_alternative<double>(right))) {
        return to_double(left) < to_double(right);
    }
    return to_string(left) < to_string(right);
}

bool less_equal(const BasicValue& left, const BasicValue& right) {
    return less_than(left, right) || equal(left, right);
}

bool greater_than(const BasicValue& left, const BasicValue& right) {
    return !less_equal(left, right);
}

bool greater_equal(const BasicValue& left, const BasicValue& right) {
    return !less_than(left, right);
}

} // namespace basic_runtime