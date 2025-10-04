#include "common.h"
#include <sstream>
#include <cmath>

namespace rbasic {

std::string valueToString(const ValueType& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    }
    return "";
}

bool isTruthy(const ValueType& value) {
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

ValueType addValues(const ValueType& left, const ValueType& right) {
    // String concatenation
    if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
        return valueToString(left) + valueToString(right);
    }
    
    // Numeric addition
    bool hasDouble = std::holds_alternative<double>(left) || std::holds_alternative<double>(right);
    
    if (hasDouble) {
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
        return leftVal + rightVal;
    } else {
        int leftVal = std::holds_alternative<int>(left) ? std::get<int>(left) : 0;
        int rightVal = std::holds_alternative<int>(right) ? std::get<int>(right) : 0;
        return leftVal + rightVal;
    }
}

ValueType subtractValues(const ValueType& left, const ValueType& right) {
    bool hasDouble = std::holds_alternative<double>(left) || std::holds_alternative<double>(right);
    
    if (hasDouble) {
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
        return leftVal - rightVal;
    } else {
        int leftVal = std::holds_alternative<int>(left) ? std::get<int>(left) : 0;
        int rightVal = std::holds_alternative<int>(right) ? std::get<int>(right) : 0;
        return leftVal - rightVal;
    }
}

ValueType multiplyValues(const ValueType& left, const ValueType& right) {
    bool hasDouble = std::holds_alternative<double>(left) || std::holds_alternative<double>(right);
    
    if (hasDouble) {
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
        return leftVal * rightVal;
    } else {
        int leftVal = std::holds_alternative<int>(left) ? std::get<int>(left) : 0;
        int rightVal = std::holds_alternative<int>(right) ? std::get<int>(right) : 0;
        return leftVal * rightVal;
    }
}

ValueType divideValues(const ValueType& left, const ValueType& right) {
    double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                    std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
    double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                     std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
    
    if (rightVal == 0.0) {
        throw RuntimeError("Division by zero");
    }
    
    return leftVal / rightVal;
}

ValueType compareValues(const ValueType& left, const ValueType& right, const std::string& op) {
    // For numeric comparison
    if ((std::holds_alternative<int>(left) || std::holds_alternative<double>(left)) &&
        (std::holds_alternative<int>(right) || std::holds_alternative<double>(right))) {
        
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        static_cast<double>(std::get<int>(left));
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         static_cast<double>(std::get<int>(right));
        
        if (op == "==") return leftVal == rightVal;
        if (op == "!=") return leftVal != rightVal;
        if (op == "<") return leftVal < rightVal;
        if (op == "<=") return leftVal <= rightVal;
        if (op == ">") return leftVal > rightVal;
        if (op == ">=") return leftVal >= rightVal;
    }
    
    // For string comparison
    std::string leftStr = valueToString(left);
    std::string rightStr = valueToString(right);
    
    if (op == "==") return leftStr == rightStr;
    if (op == "!=") return leftStr != rightStr;
    if (op == "<") return leftStr < rightStr;
    if (op == "<=") return leftStr <= rightStr;
    if (op == ">") return leftStr > rightStr;
    if (op == ">=") return leftStr >= rightStr;
    
    return false;
}

} // namespace rbasic