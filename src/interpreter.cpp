#include "interpreter.h"
#include "runtime.h"
#include "io_handler.h"
#include "type_utils.h"
#include "terminal.h"
#include "lexer.h"
#include "parser.h"
#include "math_utils.h"
#include "../runtime/basic_runtime.h"
#include "../include/ffi.h"
#include "../include/safe_ffi.h"
#include "../include/unified_value.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

// BasicValue is already available from basic_runtime.h
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>
#include <filesystem>

#ifdef _WIN32
// Undefine Windows macros that conflict with std::min/std::max
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

namespace rbasic {

// Helper functions for converting between BasicValue and UnifiedValue
UnifiedValue basicValueToUnifiedValue(const BasicValue& bv) {
    if (std::holds_alternative<int>(bv)) {
        return make_int(std::get<int>(bv));
    } else if (std::holds_alternative<double>(bv)) {
        return make_double(std::get<double>(bv));
    } else if (std::holds_alternative<std::string>(bv)) {
        return make_string(std::get<std::string>(bv));
    } else if (std::holds_alternative<bool>(bv)) {
        return make_int(std::get<bool>(bv) ? 1 : 0);
    } else if (std::holds_alternative<void*>(bv)) {
        return make_pointer(std::get<void*>(bv), "void");
    }
    return make_int(0); // fallback
}

BasicValue unifiedValueToBasicValue(const UnifiedValue& uv) {
    if (holds_type<int>(uv)) {
        return get_value<int>(uv);
    } else if (holds_type<double>(uv)) {
        return get_value<double>(uv);
    } else if (holds_type<std::string>(uv)) {
        return get_value<std::string>(uv);
    } else if (holds_type<bool>(uv)) {
        return get_value<bool>(uv);
    } else if (holds_type<void*>(uv)) {
        return get_value<void*>(uv);
    }
    return 0; // fallback
}

Interpreter::Interpreter(std::unique_ptr<IOHandler> io) : hasReturned(false) {
    // Initialize boolean constants
    globals["true"] = true;
    globals["false"] = false;
    
    // Set up I/O handler (default to console if none provided)
    if (io) {
        ioHandler = std::move(io);
    } else {
        ioHandler = createIOHandler("console");
    }
}

void Interpreter::defineVariable(const std::string& name, const ValueType& value) {
    if (!scopes.empty()) {
        scopes.back()[name] = value;
    } else {
        globals[name] = value;
    }
}

ValueType Interpreter::getVariable(const std::string& name) {
    // Search through scope stack from top to bottom (reverse vector order)
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return (*it)[name];
        }
    }
    
    // Check global scope
    if (globals.find(name) != globals.end()) {
        return globals[name];
    }
    
    throw RuntimeError("Undefined variable '" + name + "'", getCurrentPosition());
}

bool Interpreter::variableExists(const std::string& name) {
    // Search through scope stack from top to bottom (reverse vector order)
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    
    // Check global scope
    if (globals.find(name) != globals.end()) {
        return true;
    }
    
    return false;
}

void Interpreter::setVariable(const std::string& name, const ValueType& value) {
    // Search through scope stack from top to bottom to find existing variable
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            (*it)[name] = value;
            return;
        }
    }
    
    // Check global scope
    if (globals.find(name) != globals.end()) {
        globals[name] = value;
    } else {
        defineVariable(name, value);
    }
}

void Interpreter::pushScope() {
    scopes.push_back(std::map<std::string, ValueType>());
}

void Interpreter::popScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

void Interpreter::interpret(Program& program) {
    try {
        program.accept(*this);
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

ValueType Interpreter::evaluate(Expression& expr) {
    // Track source position for error reporting
    setCurrentPosition(expr.getPosition());
    expr.accept(*this);
    return lastValue;
}

IOHandler* Interpreter::getIOHandler() const {
    return ioHandler.get();
}

// Visitor implementations
void Interpreter::visit(LiteralExpr& node) {
    lastValue = node.value;
}

void Interpreter::visit(VariableExpr& node) {
    // Handle array access
    if (!node.indices.empty()) {
        ValueType arrayVar = getVariable(node.name);
        
        // Evaluate all indices
        std::vector<int> indices;
        for (auto& indexExpr : node.indices) {
            indices.push_back(TypeUtils::toArrayIndex(evaluate(*indexExpr)));
        }
        
        if (std::holds_alternative<ArrayValue>(arrayVar)) {
            ArrayValue& array = std::get<ArrayValue>(arrayVar);
            int flatIndex = array.calculateIndex(indices);
            
            if (array.elements.find(flatIndex) != array.elements.end()) {
                // Convert from simple variant to full ValueType
                auto& element = array.elements[flatIndex];
                if (std::holds_alternative<int>(element)) {
                    lastValue = std::get<int>(element);
                } else if (std::holds_alternative<double>(element)) {
                    lastValue = std::get<double>(element);
                } else if (std::holds_alternative<std::string>(element)) {
                    lastValue = std::get<std::string>(element);
                } else if (std::holds_alternative<bool>(element)) {
                    lastValue = std::get<bool>(element);
                }
            } else {
                // Return default value based on context - for now, return 0
                lastValue = 0;
            }
        } else if (std::holds_alternative<ByteArrayValue>(arrayVar)) {
            ByteArrayValue& array = std::get<ByteArrayValue>(arrayVar);
            lastValue = static_cast<int>(array.at(indices));
        } else if (std::holds_alternative<IntArrayValue>(arrayVar)) {
            IntArrayValue& array = std::get<IntArrayValue>(arrayVar);
            lastValue = array.at(indices);
        } else if (std::holds_alternative<DoubleArrayValue>(arrayVar)) {
            DoubleArrayValue& array = std::get<DoubleArrayValue>(arrayVar);
            lastValue = array.at(indices);
        } else {
            throw RuntimeError("Variable '" + node.name + "' is not an array");
        }
        return;
    }
    
    // Handle struct member access
    if (!node.member.empty()) {
        auto structValue = getVariable(node.name);
        if (auto structVal = std::get_if<StructValue>(&structValue)) {
            auto it = structVal->fields.find(node.member);
            if (it != structVal->fields.end()) {
                // Convert from struct field variant to ValueType
                auto& fieldValue = it->second;
                if (std::holds_alternative<int>(fieldValue)) {
                    lastValue = std::get<int>(fieldValue);
                } else if (std::holds_alternative<double>(fieldValue)) {
                    lastValue = std::get<double>(fieldValue);
                } else if (std::holds_alternative<std::string>(fieldValue)) {
                    lastValue = std::get<std::string>(fieldValue);
                } else if (std::holds_alternative<bool>(fieldValue)) {
                    lastValue = std::get<bool>(fieldValue);
                }
            } else {
                throw RuntimeError("Struct member '" + node.member + "' not found");
            }
        } else {
            throw RuntimeError("'" + node.name + "' is not a struct");
        }
        return;
    }
    
    // Regular variable access
    // First check if it's a predefined constant
    if (node.name == "NULL" || node.name == "null") {
        auto constant = basic_runtime::get_constant("NULL");
        if (std::holds_alternative<void*>(constant)) {
            lastValue = std::get<void*>(constant);
        } else {
            lastValue = static_cast<void*>(nullptr);
        }
        return;
    }
    if (node.name == "TRUE" || node.name == "true") {
        auto constant = basic_runtime::get_constant("TRUE");
        if (std::holds_alternative<bool>(constant)) {
            lastValue = std::get<bool>(constant);
        } else {
            lastValue = true;
        }
        return;
    }
    if (node.name == "FALSE" || node.name == "false") {
        auto constant = basic_runtime::get_constant("FALSE");
        if (std::holds_alternative<bool>(constant)) {
            lastValue = std::get<bool>(constant);
        } else {
            lastValue = false;
        }
        return;
    }
    
    // Check for SDL2/SQLite/Windows constants
    if (node.name.find("SDL_") == 0 || node.name.find("SDLK_") == 0 || 
        node.name.find("SQLITE_") == 0 || node.name.find("MB_") == 0) {
        auto constant = basic_runtime::get_constant(node.name);
        if (std::holds_alternative<double>(constant)) {
            lastValue = std::get<double>(constant);
        } else if (std::holds_alternative<int>(constant)) {
            lastValue = std::get<int>(constant);
        } else {
            lastValue = 0.0; // Default for unknown constants
        }
        return;
    }
    
    lastValue = getVariable(node.name);
}

void Interpreter::visit(BinaryExpr& node) {
    ValueType left = evaluate(*node.left);
    ValueType right = evaluate(*node.right);
    
    if (node.operator_ == "+") {
        lastValue = addValues(left, right);
    } else if (node.operator_ == "-") {
        lastValue = subtractValues(left, right);
    } else if (node.operator_ == "*") {
        lastValue = multiplyValues(left, right);
    } else if (node.operator_ == "/") {
        lastValue = divideValues(left, right);
    } else if (node.operator_ == "mod" || node.operator_ == "%") {
        // Implement modulo operation
        int leftInt = TypeUtils::toInt(left);
        int rightInt = TypeUtils::toInt(right);
            
        if (rightInt == 0) {
            throw RuntimeError("MOD by zero");
        }
        lastValue = leftInt % rightInt;
    } else if (node.operator_ == "==" || node.operator_ == "=") {
        lastValue = compareValues(left, right, "==");
    } else if (node.operator_ == "<>" || node.operator_ == "!=") {
        lastValue = compareValues(left, right, "!=");
    } else if (node.operator_ == "<") {
        lastValue = compareValues(left, right, "<");
    } else if (node.operator_ == "<=") {
        lastValue = compareValues(left, right, "<=");
    } else if (node.operator_ == ">") {
        lastValue = compareValues(left, right, ">");
    } else if (node.operator_ == ">=") {
        lastValue = compareValues(left, right, ">=");
    } else if (node.operator_ == "and") {
        lastValue = TypeUtils::toBool(left) && TypeUtils::toBool(right);
    } else if (node.operator_ == "or") {
        lastValue = TypeUtils::toBool(left) || TypeUtils::toBool(right);
    } else {
        throw RuntimeError("Unknown binary operator: " + node.operator_);
    }
}

void Interpreter::visit(AssignExpr& node) {
    ValueType value = evaluate(*node.value);
    
    // Handle array assignment
    if (!node.indices.empty()) {
        ValueType arrayVar = getVariable(node.variable);
        
        // Evaluate all indices
        std::vector<int> indices;
        for (auto& indexExpr : node.indices) {
            indices.push_back(TypeUtils::toArrayIndex(evaluate(*indexExpr)));
        }
        
        if (std::holds_alternative<ArrayValue>(arrayVar)) {
            ArrayValue array = std::get<ArrayValue>(arrayVar);
            int flatIndex = array.calculateIndex(indices);
            
            // Convert ValueType to simple variant for storage
            if (std::holds_alternative<int>(value)) {
                array.elements[flatIndex] = std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                array.elements[flatIndex] = std::get<double>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                array.elements[flatIndex] = std::get<std::string>(value);
            } else if (std::holds_alternative<bool>(value)) {
                array.elements[flatIndex] = std::get<bool>(value);
            }
            
            setVariable(node.variable, array);
        } else if (std::holds_alternative<ByteArrayValue>(arrayVar)) {
            ByteArrayValue array = std::get<ByteArrayValue>(arrayVar);
            uint8_t byteValue = TypeUtils::getValue<uint8_t>(value);
            array.at(indices) = byteValue;
            setVariable(node.variable, array);
        } else if (std::holds_alternative<IntArrayValue>(arrayVar)) {
            IntArrayValue array = std::get<IntArrayValue>(arrayVar);
            int intValue = TypeUtils::toInt(value);
            array.at(indices) = intValue;
            setVariable(node.variable, array);
        } else if (std::holds_alternative<DoubleArrayValue>(arrayVar)) {
            DoubleArrayValue array = std::get<DoubleArrayValue>(arrayVar);
            double doubleValue = TypeUtils::toDouble(value);
            array.at(indices) = doubleValue;
            setVariable(node.variable, array);
        } else {
            throw RuntimeError("Variable '" + node.variable + "' is not an array");
        }
    } else {
        // Regular variable assignment
        setVariable(node.variable, value);
    }
    
    lastValue = value;
}

void Interpreter::visit(ComponentAssignExpr& node) {
    ValueType newValue = evaluate(*node.value);
    
    // Get the variable name first, before evaluating the object
    if (auto varExpr = dynamic_cast<VariableExpr*>(node.object.get())) {
        std::string varName = varExpr->name;
        ValueType objectValue = getVariable(varName);  // Get directly from variables map
        
        // Convert the new value to float for GLM components
        float floatValue = 0.0f;
        if (std::holds_alternative<double>(newValue)) {
            floatValue = static_cast<float>(std::get<double>(newValue));
        } else if (std::holds_alternative<int>(newValue)) {
            floatValue = static_cast<float>(std::get<int>(newValue));
        } else {
            throw RuntimeError("Cannot assign non-numeric value to vector component");
        }
        
        // Update the component based on vector type
        if (std::holds_alternative<Vec2Value>(objectValue)) {
            Vec2Value vec = std::get<Vec2Value>(objectValue);
            if (node.component == "x") {
                vec.data.x = floatValue;
            } else if (node.component == "y") {
                vec.data.y = floatValue;
            } else {
                throw RuntimeError("Invalid component '" + node.component + "' for vec2");
            }
            setVariable(varName, vec);
            lastValue = static_cast<double>(floatValue);
        } else if (std::holds_alternative<Vec3Value>(objectValue)) {
            Vec3Value vec = std::get<Vec3Value>(objectValue);
            if (node.component == "x") {
                vec.data.x = floatValue;
            } else if (node.component == "y") {
                vec.data.y = floatValue;
            } else if (node.component == "z") {
                vec.data.z = floatValue;
            } else {
                throw RuntimeError("Invalid component '" + node.component + "' for vec3");
            }
            setVariable(varName, vec);
            lastValue = static_cast<double>(floatValue);
        } else if (std::holds_alternative<Vec4Value>(objectValue)) {
            Vec4Value vec = std::get<Vec4Value>(objectValue);
            if (node.component == "x") {
                vec.data.x = floatValue;
            } else if (node.component == "y") {
                vec.data.y = floatValue;
            } else if (node.component == "z") {
                vec.data.z = floatValue;
            } else if (node.component == "w") {
                vec.data.w = floatValue;
            } else {
                throw RuntimeError("Invalid component '" + node.component + "' for vec4");
            }
            setVariable(varName, vec);
            lastValue = static_cast<double>(floatValue);
        } else {
            throw RuntimeError("Component assignment not supported for this type");
        }
    } else {
        throw RuntimeError("Can only assign to components of variables");
    }
}

void Interpreter::visit(UnaryExpr& node) {
    ValueType operand = evaluate(*node.operand);
    
    if (node.operator_ == "-") {
        if (std::holds_alternative<int>(operand)) {
            lastValue = -std::get<int>(operand);
        } else if (std::holds_alternative<double>(operand)) {
            lastValue = -std::get<double>(operand);
        } else {
            throw RuntimeError("Cannot negate non-numeric value");
        }
    } else if (node.operator_ == "not") {
        lastValue = !isTruthy(operand);
    } else {
        throw RuntimeError("Unknown unary operator: " + node.operator_);
    }
}

void Interpreter::visit(CallExpr& node) {
    // Set position for error reporting
    setCurrentPosition(node.getPosition());
    
    // Dispatch to specialized handlers
    if (handleIOFunctions(node) ||
        handleMathFunctions(node) ||
        handleStringFunctions(node) ||
        handleArrayFunctions(node) ||
        handleFileFunctions(node) ||
        handleTerminalFunctions(node) ||
        handleFFIFunctions(node) ||
        handleUserDefinedFunction(node)) {
        return;
    }
    
    // If we get here, the function was not found
    throw RuntimeError("Unknown function: " + node.name, getCurrentPosition());
}

// I/O Functions Handler
bool Interpreter::handleIOFunctions(CallExpr& node) {
    if (node.name == "print") {
        for (size_t i = 0; i < node.arguments.size(); i++) {
            ValueType value = evaluate(*node.arguments[i]);
            ioHandler->print(valueToString(value));
            if (i < node.arguments.size() - 1) {
                ioHandler->print(" ");
            }
        }
        ioHandler->newline();
        lastValue = 0; // print returns 0
        return true;
    }

    if (node.name == "debug_print") {
        for (size_t i = 0; i < node.arguments.size(); i++) {
            ValueType value = evaluate(*node.arguments[i]);
            std::cout << valueToString(value);
            if (i < node.arguments.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
        std::cout.flush();
        lastValue = 0; // debug_print returns 0
        return true;
    }
    
    if (node.name == "input" && node.arguments.size() == 0) {
        std::string input_text = ioHandler->input();
        
        // Try to parse as number first
        ValueType value;
        try {
            if (hasDecimalPoint(input_text)) {
                value = std::stod(input_text);
            } else {
                value = std::stoi(input_text);
            }
        } catch (const std::invalid_argument&) {
            // If not a number, store as string
            value = input_text;
        } catch (const std::out_of_range&) {
            // Number too large, store as string
            value = input_text;
        }
        
        lastValue = value;
        return true;
    }
    
    return false; // Function not handled by this dispatcher
}

bool Interpreter::handleMathFunctions(CallExpr& node) {
    // Built-in math functions (single argument) - optimized with dispatcher
    if (node.arguments.size() == 1) {
        auto& dispatcher = MathFunctionDispatcher::getInstance();
        if (dispatcher.hasFunction(node.name)) {
            ValueType arg = evaluate(*node.arguments[0]);
            double numArg = 0.0;
            
            // Convert argument to double
            if (std::holds_alternative<int>(arg)) {
                numArg = static_cast<double>(std::get<int>(arg));
            } else if (std::holds_alternative<double>(arg)) {
                numArg = std::get<double>(arg);
            } else {
                throw RuntimeError(node.name + " requires a numeric argument");
            }
            
            try {
                double result = dispatcher.callFunction(node.name, numArg);
                // Convert back to int for int() function
                if (node.name == "int") {
                    lastValue = static_cast<int>(result);
                } else {
                    lastValue = result;
                }
            } catch (const std::exception& e) {
                throw RuntimeError(e.what());
            }
            return true;
        }
    }
    
    // Two-argument math functions
    if (node.arguments.size() == 2) {
        if (node.name == "pow") {
            ValueType base = evaluate(*node.arguments[0]);
            ValueType exp = evaluate(*node.arguments[1]);
            
            double baseNum = std::holds_alternative<int>(base) ? 
                static_cast<double>(std::get<int>(base)) : std::get<double>(base);
            double expNum = std::holds_alternative<int>(exp) ? 
                static_cast<double>(std::get<int>(exp)) : std::get<double>(exp);
                
            lastValue = std::pow(baseNum, expNum);
            return true;
        } else if (node.name == "atan2") {
            ValueType y = evaluate(*node.arguments[0]);
            ValueType x = evaluate(*node.arguments[1]);
            
            double yNum = std::holds_alternative<int>(y) ? 
                static_cast<double>(std::get<int>(y)) : std::get<double>(y);
            double xNum = std::holds_alternative<int>(x) ? 
                static_cast<double>(std::get<int>(x)) : std::get<double>(x);
                
            lastValue = std::atan2(yNum, xNum);
            return true;
        } else if (node.name == "mod") {
            ValueType left = evaluate(*node.arguments[0]);
            ValueType right = evaluate(*node.arguments[1]);
            
            int leftInt = std::holds_alternative<int>(left) ? 
                std::get<int>(left) : static_cast<int>(std::get<double>(left));
            int rightInt = std::holds_alternative<int>(right) ? 
                std::get<int>(right) : static_cast<int>(std::get<double>(right));
                
            if (rightInt == 0) {
                throw RuntimeError("MOD by zero");
            }
            lastValue = leftInt % rightInt;
            return true;
        } else if (node.name == "min") {
            ValueType left = evaluate(*node.arguments[0]);
            ValueType right = evaluate(*node.arguments[1]);
            
            double leftNum = std::holds_alternative<int>(left) ? 
                static_cast<double>(std::get<int>(left)) : std::get<double>(left);
            double rightNum = std::holds_alternative<int>(right) ? 
                static_cast<double>(std::get<int>(right)) : std::get<double>(right);
                
            lastValue = std::min(leftNum, rightNum);
            return true;
        } else if (node.name == "max") {
            ValueType left = evaluate(*node.arguments[0]);
            ValueType right = evaluate(*node.arguments[1]);
            
            double leftNum = std::holds_alternative<int>(left) ? 
                static_cast<double>(std::get<int>(left)) : std::get<double>(left);
            double rightNum = std::holds_alternative<int>(right) ? 
                static_cast<double>(std::get<int>(right)) : std::get<double>(right);
                
            lastValue = std::max(leftNum, rightNum);
            return true;
        }
    }
    
    // Zero-argument math constants/functions
    if (node.arguments.size() == 0) {
        if (node.name == "pi") {
            lastValue = 3.141592653589793;
            return true;
        }
    }
    
    // Random functions
    if (node.name == "rnd" || node.name == "random") {
        if (node.arguments.size() == 0) {
            lastValue = static_cast<double>(std::rand()) / RAND_MAX;
            return true;
        } else if (node.arguments.size() == 1) {
            ValueType arg = evaluate(*node.arguments[0]);
            int maxVal = 1;
            if (std::holds_alternative<int>(arg)) {
                maxVal = std::get<int>(arg);
            } else if (std::holds_alternative<double>(arg)) {
                maxVal = static_cast<int>(std::get<double>(arg));
            }
            if (maxVal <= 0) maxVal = 1;
            lastValue = (std::rand() % maxVal) + 1;  // 1 to maxVal
            return true;
        }
    }
    
    if (node.name == "randomise" && node.arguments.size() == 0) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        lastValue = 0; // randomise doesn't return a value
        return true;
    }
    
    // GLM vector functions
    if (node.name == "length" && node.arguments.size() == 1) {
        ValueType arg = evaluate(*node.arguments[0]);
        if (std::holds_alternative<Vec2Value>(arg)) {
            Vec2Value vec = std::get<Vec2Value>(arg);
            lastValue = static_cast<double>(glm::length(vec.data));
            return true;
        } else if (std::holds_alternative<Vec3Value>(arg)) {
            Vec3Value vec = std::get<Vec3Value>(arg);
            lastValue = static_cast<double>(glm::length(vec.data));
            return true;
        } else if (std::holds_alternative<Vec4Value>(arg)) {
            Vec4Value vec = std::get<Vec4Value>(arg);
            lastValue = static_cast<double>(glm::length(vec.data));
            return true;
        } else {
            throw RuntimeError("length() requires a vector argument");
        }
    }
    
    if (node.name == "normalize" && node.arguments.size() == 1) {
        ValueType arg = evaluate(*node.arguments[0]);
        if (std::holds_alternative<Vec2Value>(arg)) {
            Vec2Value vec = std::get<Vec2Value>(arg);
            lastValue = Vec2Value(glm::normalize(vec.data));
            return true;
        } else if (std::holds_alternative<Vec3Value>(arg)) {
            Vec3Value vec = std::get<Vec3Value>(arg);
            lastValue = Vec3Value(glm::normalize(vec.data));
            return true;
        } else if (std::holds_alternative<Vec4Value>(arg)) {
            Vec4Value vec = std::get<Vec4Value>(arg);
            lastValue = Vec4Value(glm::normalize(vec.data));
            return true;
        } else {
            throw RuntimeError("normalize() requires a vector argument");
        }
    }
    
    if (node.name == "dot" && node.arguments.size() == 2) {
        ValueType left = evaluate(*node.arguments[0]);
        ValueType right = evaluate(*node.arguments[1]);
        
        if (std::holds_alternative<Vec2Value>(left) && std::holds_alternative<Vec2Value>(right)) {
            Vec2Value leftVec = std::get<Vec2Value>(left);
            Vec2Value rightVec = std::get<Vec2Value>(right);
            lastValue = static_cast<double>(glm::dot(leftVec.data, rightVec.data));
            return true;
        } else if (std::holds_alternative<Vec3Value>(left) && std::holds_alternative<Vec3Value>(right)) {
            Vec3Value leftVec = std::get<Vec3Value>(left);
            Vec3Value rightVec = std::get<Vec3Value>(right);
            lastValue = static_cast<double>(glm::dot(leftVec.data, rightVec.data));
            return true;
        } else if (std::holds_alternative<Vec4Value>(left) && std::holds_alternative<Vec4Value>(right)) {
            Vec4Value leftVec = std::get<Vec4Value>(left);
            Vec4Value rightVec = std::get<Vec4Value>(right);
            lastValue = static_cast<double>(glm::dot(leftVec.data, rightVec.data));
            return true;
        } else {
            throw RuntimeError("dot() requires two vectors of the same type");
        }
    }
    
    if (node.name == "cross" && node.arguments.size() == 2) {
        ValueType left = evaluate(*node.arguments[0]);
        ValueType right = evaluate(*node.arguments[1]);
        
        if (std::holds_alternative<Vec3Value>(left) && std::holds_alternative<Vec3Value>(right)) {
            Vec3Value leftVec = std::get<Vec3Value>(left);
            Vec3Value rightVec = std::get<Vec3Value>(right);
            lastValue = Vec3Value(glm::cross(leftVec.data, rightVec.data));
            return true;
        } else {
            throw RuntimeError("cross() requires two vec3 arguments");
        }
    }
    
    if (node.name == "distance" && node.arguments.size() == 2) {
        ValueType left = evaluate(*node.arguments[0]);
        ValueType right = evaluate(*node.arguments[1]);
        
        if (std::holds_alternative<Vec2Value>(left) && std::holds_alternative<Vec2Value>(right)) {
            Vec2Value leftVec = std::get<Vec2Value>(left);
            Vec2Value rightVec = std::get<Vec2Value>(right);
            lastValue = static_cast<double>(glm::distance(leftVec.data, rightVec.data));
            return true;
        } else if (std::holds_alternative<Vec3Value>(left) && std::holds_alternative<Vec3Value>(right)) {
            Vec3Value leftVec = std::get<Vec3Value>(left);
            Vec3Value rightVec = std::get<Vec3Value>(right);
            lastValue = static_cast<double>(glm::distance(leftVec.data, rightVec.data));
            return true;
        } else if (std::holds_alternative<Vec4Value>(left) && std::holds_alternative<Vec4Value>(right)) {
            Vec4Value leftVec = std::get<Vec4Value>(left);
            Vec4Value rightVec = std::get<Vec4Value>(right);
            lastValue = static_cast<double>(glm::distance(leftVec.data, rightVec.data));
            return true;
        } else {
            throw RuntimeError("distance() requires two vectors of the same type");
        }
    }
    
    return false; // Function not handled by this dispatcher
}

bool Interpreter::handleStringFunctions(CallExpr& node) {
    // String functions (mid, left, right, len, str, val)
    if (node.arguments.size() == 2 || (node.name == "mid" && (node.arguments.size() == 2 || node.arguments.size() == 3))) {
        if (node.name == "mid") {
            if (node.arguments.size() < 2 || node.arguments.size() > 3) {
                throw RuntimeError("MID requires 2 or 3 arguments");
            }
            
            std::string str = valueToString(evaluate(*node.arguments[0]));
            int start = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
                std::get<int>(evaluate(*node.arguments[1])) : 
                static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
            
            start = std::max(1, start) - 1; // Convert from 1-based to 0-based
            
            if (node.arguments.size() == 3) {
                int length = std::holds_alternative<int>(evaluate(*node.arguments[2])) ? 
                    std::get<int>(evaluate(*node.arguments[2])) : 
                    static_cast<int>(std::get<double>(evaluate(*node.arguments[2])));
                    
                if (start >= static_cast<int>(str.length())) {
                    lastValue = std::string("");
                } else {
                    lastValue = str.substr(start, length);
                }
            } else {
                if (start >= static_cast<int>(str.length())) {
                    lastValue = std::string("");
                } else {
                    lastValue = str.substr(start);
                }
            }
            return true;
        } else if (node.name == "left") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            int length = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
                std::get<int>(evaluate(*node.arguments[1])) : 
                static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
                
            lastValue = str.substr(0, std::max(0, length));
            return true;
        } else if (node.name == "right") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            int length = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
                std::get<int>(evaluate(*node.arguments[1])) : 
                static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
                
            int start = std::max(0, static_cast<int>(str.length()) - length);
            lastValue = str.substr(start);
            return true;
        }
    }
    
    // Single-argument string functions
    if (node.arguments.size() == 1) {
        if (node.name == "len") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            lastValue = static_cast<int>(str.length());
            return true;
        } else if (node.name == "str") {
            ValueType value = evaluate(*node.arguments[0]);
            lastValue = valueToString(value);
            return true;
        } else if (node.name == "val") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            try {
                if (hasDecimalPoint(str)) {
                    lastValue = std::stod(str);
                } else {
                    lastValue = std::stoi(str);
                }
            } catch (const std::exception&) {
                lastValue = 0; // Default to 0 if conversion fails
            }
            return true;
        }
    }
    
    return false; // Function not handled by this dispatcher
}

bool Interpreter::handleArrayFunctions(CallExpr& node) {
    // Array creation functions
    if (node.name == "byte_array" && node.arguments.size() >= 1) {
        std::vector<int> dims;
        for (auto& arg : node.arguments) {
            ValueType dimVal = evaluate(*arg);
            int dim = 0;
            if (std::holds_alternative<int>(dimVal)) {
                dim = std::get<int>(dimVal);
            } else if (std::holds_alternative<double>(dimVal)) {
                dim = static_cast<int>(std::get<double>(dimVal));
            } else {
                throw RuntimeError("Array dimensions must be numeric");
            }
            dims.push_back(dim);
        }
        lastValue = ByteArrayValue(dims);
        return true;
    }
    
    if (node.name == "int_array" && node.arguments.size() >= 1) {
        std::vector<int> dims;
        for (auto& arg : node.arguments) {
            ValueType dimVal = evaluate(*arg);
            int dim = 0;
            if (std::holds_alternative<int>(dimVal)) {
                dim = std::get<int>(dimVal);
            } else if (std::holds_alternative<double>(dimVal)) {
                dim = static_cast<int>(std::get<double>(dimVal));
            } else {
                throw RuntimeError("Array dimensions must be numeric");
            }
            dims.push_back(dim);
        }
        lastValue = IntArrayValue(dims);
        return true;
    }
    
    if (node.name == "double_array" && node.arguments.size() >= 1) {
        std::vector<int> dims;
        for (auto& arg : node.arguments) {
            ValueType dimVal = evaluate(*arg);
            int dim = 0;
            if (std::holds_alternative<int>(dimVal)) {
                dim = std::get<int>(dimVal);
            } else if (std::holds_alternative<double>(dimVal)) {
                dim = static_cast<int>(std::get<double>(dimVal));
            } else {
                throw RuntimeError("Array dimensions must be numeric");
            }
            dims.push_back(dim);
        }
        lastValue = DoubleArrayValue(dims);
        return true;
    }
    
    return false; // Function not handled by this dispatcher
}

bool Interpreter::handleFileFunctions(CallExpr& node) {
    // File I/O functions
    if (node.name == "file_exists" && node.arguments.size() == 1) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(filenameVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            lastValue = std::filesystem::exists(filename);
        } else {
            lastValue = false;
        }
        return true;
    }
    
    if (node.name == "file_size" && node.arguments.size() == 1) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(filenameVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            try {
                if (std::filesystem::exists(filename)) {
                    auto size = std::filesystem::file_size(filename);
                    lastValue = static_cast<int>(size);
                } else {
                    lastValue = -1;  // File doesn't exist
                }
            } catch (const std::filesystem::filesystem_error& /* e */) {
                lastValue = -1;  // Filesystem error
            } catch (...) {
                lastValue = -1;  // Other error
            }
        } else {
            lastValue = -1;
        }
        return true;
    }
    
    if (node.name == "delete_file" && node.arguments.size() == 1) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(filenameVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            try {
                lastValue = std::filesystem::remove(filename);
            } catch (...) {
                lastValue = false;
            }
        } else {
            lastValue = false;
        }
        return true;
    }
    
    if (node.name == "rename_file" && node.arguments.size() == 2) {
        ValueType oldnameVal = evaluate(*node.arguments[0]);
        ValueType newnameVal = evaluate(*node.arguments[1]);
        if (std::holds_alternative<std::string>(oldnameVal) && std::holds_alternative<std::string>(newnameVal)) {
            std::string oldname = std::get<std::string>(oldnameVal);
            std::string newname = std::get<std::string>(newnameVal);
            try {
                std::filesystem::rename(oldname, newname);
                lastValue = true;
            } catch (...) {
                lastValue = false;
            }
        } else {
            lastValue = false;
        }
        return true;
    }
    
    if (node.name == "read_text_file" && node.arguments.size() == 1) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(filenameVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            std::ifstream file(filename);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
                file.close();
                lastValue = content;
            } else {
                lastValue = std::string("");
            }
        } else {
            lastValue = std::string("");
        }
        return true;
    }
    
    if (node.name == "write_text_file" && node.arguments.size() == 2) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        ValueType contentVal = evaluate(*node.arguments[1]);
        if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<std::string>(contentVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            std::string content = std::get<std::string>(contentVal);
            std::ofstream file(filename);
            if (file.is_open()) {
                file << content;
                file.flush();  // Ensure content is written to disk
                file.close();
                lastValue = !file.fail();
            } else {
                lastValue = false;
            }
        } else {
            lastValue = false;
        }
        return true;
    }
    
    if (node.name == "append_text_file" && node.arguments.size() == 2) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        ValueType contentVal = evaluate(*node.arguments[1]);
        if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<std::string>(contentVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            std::string content = std::get<std::string>(contentVal);
            std::ofstream file(filename, std::ios::app);
            if (file.is_open()) {
                file << content;
                file.flush();  // Ensure content is written to disk
                file.close();
                lastValue = !file.fail();
            } else {
                lastValue = false;
            }
        } else {
            lastValue = false;
        }
        return true;
    }
    
    if (node.name == "load_binary_file" && node.arguments.size() == 1) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(filenameVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            std::ifstream file(filename, std::ios::binary);
            if (file.is_open()) {
                // Get file size
                file.seekg(0, std::ios::end);
                size_t fileSize = file.tellg();
                file.seekg(0, std::ios::beg);
                
                // Create buffer
                ByteArrayValue buffer({static_cast<int>(fileSize)});
                
                // Read data
                file.read(reinterpret_cast<char*>(buffer.elements.data()), fileSize);
                file.close();
                
                lastValue = buffer;
            } else {
                lastValue = ByteArrayValue();
            }
        } else {
            lastValue = ByteArrayValue();
        }
        return true;
    }
    
    if (node.name == "write_binary_file" && node.arguments.size() == 2) {
        ValueType filenameVal = evaluate(*node.arguments[0]);
        ValueType bufferVal = evaluate(*node.arguments[1]);
        if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<ByteArrayValue>(bufferVal)) {
            std::string filename = std::get<std::string>(filenameVal);
            ByteArrayValue& buffer = std::get<ByteArrayValue>(bufferVal);
            std::ofstream file(filename, std::ios::binary);
            if (file.is_open()) {
                file.write(reinterpret_cast<const char*>(buffer.elements.data()), buffer.elements.size());
                file.flush();  // Ensure content is written to disk
                file.close();
                lastValue = !file.fail();
            } else {
                lastValue = false;
            }
        } else {
            lastValue = false;
        }
        return true;
    }
    
    return false; // Function not handled by this dispatcher
}

// Terminal Functions Handler
bool Interpreter::handleTerminalFunctions(CallExpr& node) {
    // Initialize terminal if needed
    static bool terminalInitialized = false;
    if (!terminalInitialized) {
        Terminal::initialize();
        terminalInitialized = true;
    }
    
    std::vector<ValueType> args;
    for (auto& arg : node.arguments) {
        args.push_back(evaluate(*arg));
    }
    
    if (node.name == "terminal_init") {
        lastValue = Terminal::initialize();
        return true;
    }
    
    if (node.name == "terminal_cleanup") {
        Terminal::cleanup();
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_supports_color") {
        lastValue = Terminal::supportsColor();
        return true;
    }
    
    if (node.name == "terminal_clear") {
        Terminal::clear();
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_set_cursor") {
        if (args.size() >= 2) {
            Terminal::setCursor(TypeUtils::toInt(args[0]), TypeUtils::toInt(args[1]));
        }
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_get_cursor_row") {
        int row, col;
        Terminal::getCursor(row, col);
        lastValue = row;
        return true;
    }
    
    if (node.name == "terminal_get_cursor_col") {
        int row, col;
        Terminal::getCursor(row, col);
        lastValue = col;
        return true;
    }
    
    if (node.name == "terminal_save_cursor") {
        Terminal::saveCursor();
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_restore_cursor") {
        Terminal::restoreCursor();
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_set_color") {
        if (args.size() >= 2) {
            Terminal::setColor(static_cast<Color>(TypeUtils::toInt(args[0])), 
                              static_cast<Color>(TypeUtils::toInt(args[1])));
        } else if (args.size() >= 1) {
            Terminal::setColor(static_cast<Color>(TypeUtils::toInt(args[0])));
        }
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_reset_color") {
        Terminal::resetColor();
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_print") {
        if (args.size() >= 3) {
            Terminal::print(TypeUtils::toString(args[0]), 
                           static_cast<Color>(TypeUtils::toInt(args[1])), 
                           static_cast<Color>(TypeUtils::toInt(args[2])));
        } else if (args.size() >= 2) {
            Terminal::print(TypeUtils::toString(args[0]), 
                           static_cast<Color>(TypeUtils::toInt(args[1])));
        } else if (args.size() >= 1) {
            Terminal::print(TypeUtils::toString(args[0]));
        }
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_println") {
        if (args.size() >= 3) {
            Terminal::println(TypeUtils::toString(args[0]), 
                             static_cast<Color>(TypeUtils::toInt(args[1])), 
                             static_cast<Color>(TypeUtils::toInt(args[2])));
        } else if (args.size() >= 2) {
            Terminal::println(TypeUtils::toString(args[0]), 
                             static_cast<Color>(TypeUtils::toInt(args[1])));
        } else if (args.size() >= 1) {
            Terminal::println(TypeUtils::toString(args[0]));
        } else {
            Terminal::println();
        }
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_get_rows") {
        int rows, cols;
        Terminal::getSize(rows, cols);
        lastValue = rows;
        return true;
    }
    
    if (node.name == "terminal_get_cols") {
        int rows, cols;
        Terminal::getSize(rows, cols);
        lastValue = cols;
        return true;
    }
    
    if (node.name == "terminal_kbhit") {
        lastValue = Terminal::kbhit();
        return true;
    }
    
    if (node.name == "terminal_getch") {
        lastValue = Terminal::getch();
        return true;
    }
    
    if (node.name == "terminal_getline") {
        if (args.size() >= 2) {
            lastValue = Terminal::getline(TypeUtils::toString(args[0]), 
                                      static_cast<Color>(TypeUtils::toInt(args[1])));
        } else if (args.size() >= 1) {
            lastValue = Terminal::getline(TypeUtils::toString(args[0]));
        } else {
            lastValue = Terminal::getline();
        }
        return true;
    }
    
    if (node.name == "terminal_show_cursor") {
        if (args.size() >= 1) {
            Terminal::showCursor(TypeUtils::toBool(args[0]));
        }
        lastValue = 0;
        return true;
    }
    
    if (node.name == "terminal_set_echo") {
        if (args.size() >= 1) {
            Terminal::setEcho(TypeUtils::toBool(args[0]));
        }
        lastValue = 0;
        return true;
    }
    
    // Buffer allocation functions - Updated for Safe FFI
    if (node.name == "alloc_int_buffer" && node.arguments.size() == 0) {
        try {
            auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
            auto int_buffer = safe_ffi_manager.allocate_int_buffer();
            if (int_buffer) {
                lastValue = static_cast<void*>(int_buffer.get());
            } else {
                lastValue = static_cast<void*>(nullptr);
            }
        } catch (const rbasic::ffi::SafeFFIError& e) {
            throw RuntimeError("Safe int buffer allocation failed: " + std::string(e.what()));
        }
        return true;
    }
    
    if (node.name == "alloc_pointer_buffer" && node.arguments.size() == 0) {
        try {
            auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
            auto ptr_buffer = safe_ffi_manager.allocate_pointer_buffer();
            if (ptr_buffer) {
                lastValue = static_cast<void*>(ptr_buffer.get());
            } else {
                lastValue = static_cast<void*>(nullptr);
            }
        } catch (const rbasic::ffi::SafeFFIError& e) {
            throw RuntimeError("Safe pointer buffer allocation failed: " + std::string(e.what()));
        }
        return true;
    }
    
    if (node.name == "alloc_buffer" && node.arguments.size() == 1) {
        ValueType sizeVal = evaluate(*node.arguments[0]);
        int size = std::get<int>(sizeVal);
        
        // Use Safe FFI Manager for buffer allocation
        try {
            auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
            auto buffer = safe_ffi_manager.allocate_buffer(size);
            if (buffer) {
                // Return pointer to the safe buffer data
                lastValue = static_cast<void*>(buffer->data());
            } else {
                lastValue = static_cast<void*>(nullptr);
            }
        } catch (const rbasic::ffi::SafeFFIError& e) {
            throw RuntimeError("Safe buffer allocation failed: " + std::string(e.what()));
        }
        return true;
    }
    
    if (node.name == "deref_int" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            void* ptr = std::get<void*>(ptrVal);
            
            // Use Safe FFI Manager for safe dereference
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                UnifiedValue ptr_uv = make_pointer(ptr, "void");
                UnifiedValue result = safe_ffi_manager.deref_int_safe(ptr_uv);
                
                if (holds_type<int>(result)) {
                    lastValue = static_cast<double>(get_value<int>(result));
                } else {
                    lastValue = 0.0;
                }
            } catch (const rbasic::ffi::SafeFFIError& e) {
                throw RuntimeError("Safe int dereference failed: " + std::string(e.what()));
            }
        }
        return true;
    }
    
    if (node.name == "deref_pointer" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            void* ptr = std::get<void*>(ptrVal);
            
            // Use Safe FFI Manager for safe pointer dereference
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                UnifiedValue ptr_uv = make_pointer(ptr, "void");
                UnifiedValue result = safe_ffi_manager.deref_pointer_safe(ptr_uv);
                
                if (holds_type<void*>(result)) {
                    lastValue = get_value<void*>(result);
                } else {
                    lastValue = static_cast<void*>(nullptr);
                }
            } catch (const rbasic::ffi::SafeFFIError& e) {
                throw RuntimeError("Safe pointer dereference failed: " + std::string(e.what()));
            }
        }
        return true;
    }
    
    if (node.name == "deref_string" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            void* ptr = std::get<void*>(ptrVal);
            
            // Use Safe FFI Manager for safe string dereference  
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                UnifiedValue ptr_uv = make_pointer(ptr, "void");
                UnifiedValue result = safe_ffi_manager.deref_string_safe(ptr_uv);
                
                if (holds_type<std::string>(result)) {
                    lastValue = get_value<std::string>(result);
                } else {
                    lastValue = std::string("");
                }
            } catch (const rbasic::ffi::SafeFFIError& e) {
                throw RuntimeError("Safe string dereference failed: " + std::string(e.what()));
            }
        }
        return true;
    }
    
    // SDL struct helper functions - Updated for Safe FFI
    if (node.name == "create_sdl_rect" && node.arguments.size() == 4) {
        ValueType xVal = evaluate(*node.arguments[0]);
        ValueType yVal = evaluate(*node.arguments[1]);
        ValueType wVal = evaluate(*node.arguments[2]);
        ValueType hVal = evaluate(*node.arguments[3]);
        
        int x = std::get<int>(xVal);
        int y = std::get<int>(yVal);
        int w = std::get<int>(wVal);
        int h = std::get<int>(hVal);
        
        // Use Safe FFI's create_safe_sdl_rect function
        try {
            UnifiedValue result = rbasic::ffi::create_safe_sdl_rect(x, y, w, h);
            if (holds_type<void*>(result)) {
                lastValue = get_value<void*>(result);
            } else {
                lastValue = static_cast<void*>(nullptr);
            }
        } catch (const rbasic::ffi::SafeFFIError& e) {
            throw RuntimeError("Safe SDL rect creation failed: " + std::string(e.what()));
        }
        return true;
    }
    
    if (node.name == "create_sdl_event" && node.arguments.size() == 0) {
        // Use Safe FFI's create_safe_sdl_event function
        try {
            UnifiedValue result = rbasic::ffi::create_safe_sdl_event();
            if (holds_type<void*>(result)) {
                lastValue = get_value<void*>(result);
            } else if (holds_type<std::shared_ptr<UnifiedPointer>>(result)) {
                auto ptr = get_value<std::shared_ptr<UnifiedPointer>>(result);
                lastValue = ptr->get();
            } else {
                lastValue = static_cast<void*>(nullptr);
            }
        } catch (const rbasic::ffi::SafeFFIError& e) {
            throw RuntimeError("Safe SDL event creation failed: " + std::string(e.what()));
        }
        return true;
    }
    
    if (node.name == "get_event_type" && node.arguments.size() == 1) {
        ValueType eventVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(eventVal)) {
            BasicValue bv = std::get<void*>(eventVal);
            BasicValue result = basic_runtime::get_event_type(bv);
            lastValue = std::get<double>(result);
        }
        return true;
    }
    
    if (node.name == "deref_int_offset" && node.arguments.size() == 2) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        ValueType offsetVal = evaluate(*node.arguments[1]);
        if (std::holds_alternative<void*>(ptrVal) && std::holds_alternative<double>(offsetVal)) {
            void* ptr = std::get<void*>(ptrVal);
            size_t offset = static_cast<size_t>(std::get<double>(offsetVal));
            
            // Use Safe FFI Manager for bounds-checked offset dereference
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                UnifiedValue ptr_uv = make_pointer(ptr, "void");
                UnifiedValue result = safe_ffi_manager.deref_int_safe(ptr_uv, offset);
                
                if (holds_type<int>(result)) {
                    lastValue = static_cast<double>(get_value<int>(result));
                } else {
                    lastValue = 0.0;
                }
            } catch (const rbasic::ffi::SafeFFIError& e) {
                throw RuntimeError("Safe int offset dereference failed: " + std::string(e.what()));
            }
        }
        return true;
    }
    
    if (node.name == "get_key_code" && node.arguments.size() == 1) {
        ValueType eventVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(eventVal)) {
            BasicValue bv = std::get<void*>(eventVal);
            BasicValue result = basic_runtime::get_key_code(bv);
            lastValue = std::get<double>(result);
        }
        return true;
    }
    
    if (node.name == "get_rect_field" && node.arguments.size() == 2) {
        ValueType rectVal = evaluate(*node.arguments[0]);
        ValueType fieldVal = evaluate(*node.arguments[1]);
        std::string field = std::get<std::string>(fieldVal);
        
        if (std::holds_alternative<void*>(rectVal)) {
            BasicValue bv = std::get<void*>(rectVal);
            BasicValue result = basic_runtime::get_rect_field(bv, field);
            lastValue = std::get<double>(result);
        }
        return true;
    }
    
    // SDL resource management functions
    if (node.name == "free_sdl_resource" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            BasicValue bv = std::get<void*>(ptrVal);
            BasicValue result = basic_runtime::free_sdl_resource(bv);
            lastValue = std::get<bool>(result);
        } else {
            lastValue = false;
        }
        return true;
    }
    
    if (node.name == "sdl_cleanup_all" && node.arguments.size() == 0) {
        basic_runtime::sdl_cleanup_all();
        lastValue = true;
        return true;
    }
    
    // Constant/NULL handling functions
    if (node.name == "get_constant" && node.arguments.size() == 1) {
        ValueType nameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(nameVal)) {
            std::string name = std::get<std::string>(nameVal);
            BasicValue result = basic_runtime::get_constant(name);
            
            // Convert BasicValue back to ValueType for interpreter
            if (std::holds_alternative<void*>(result)) {
                lastValue = std::get<void*>(result);
            } else if (std::holds_alternative<double>(result)) {
                lastValue = std::get<double>(result);
            } else if (std::holds_alternative<bool>(result)) {
                lastValue = std::get<bool>(result);
            } else if (std::holds_alternative<std::string>(result)) {
                lastValue = std::get<std::string>(result);
            } else if (std::holds_alternative<int>(result)) {
                lastValue = std::get<int>(result);
            }
        }
        return true;
    }
    
    if (node.name == "is_null" && node.arguments.size() == 1) {
        ValueType val = evaluate(*node.arguments[0]);
        
        if (std::holds_alternative<void*>(val)) {
            void* ptr = std::get<void*>(val);
            
            // Use Safe FFI Manager for null checking
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                UnifiedValue ptr_uv = make_pointer(ptr, "void");
                bool result = safe_ffi_manager.is_null_safe(ptr_uv);
                lastValue = result;
            } catch (const rbasic::ffi::SafeFFIError& e) {
                throw RuntimeError("Safe null check failed: " + std::string(e.what()));
            }
        } else {
            lastValue = false; // Non-pointer values are not null
        }
        return true;
    }
    
    if (node.name == "not_null" && node.arguments.size() == 1) {
        ValueType val = evaluate(*node.arguments[0]);
        
        if (std::holds_alternative<void*>(val)) {
            void* ptr = std::get<void*>(val);
            
            // Use Safe FFI Manager for not-null checking
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                UnifiedValue ptr_uv = make_pointer(ptr, "void");
                bool result = safe_ffi_manager.not_null_safe(ptr_uv);
                lastValue = result;
            } catch (const rbasic::ffi::SafeFFIError& e) {
                throw RuntimeError("Safe not-null check failed: " + std::string(e.what()));
            }
        } else {
            lastValue = true; // Non-pointer values are considered not-null
        }
        return true;
    }
    
    return false; // Function not handled by this dispatcher
}

// FFI Functions Handler - Updated for Phase 2 Safe FFI
bool Interpreter::handleFFIFunctions(CallExpr& node) {
    if (node.name == "load_library" && node.arguments.size() == 1) {
        ValueType libNameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(libNameVal)) {
            std::string libName = std::get<std::string>(libNameVal);
            // Use Safe FFI Manager
            try {
                auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                auto library = safe_ffi_manager.load_library(libName);
                if (library && library->is_valid()) {
                    lastValue = std::string("library_handle:" + libName);
                } else {
                    lastValue = std::string("error:Failed to load " + libName);
                }
            } catch (const rbasic::ffi::SafeFFIError& e) {
                lastValue = std::string("error:SafeFFI exception: " + std::string(e.what()));
            } catch (...) {
                lastValue = std::string("error:Exception loading " + libName);
            }
        } else {
            throw RuntimeError("load_library() requires a string library name");
        }
        return true;
    }
    
    if (node.name == "unload_library" && node.arguments.size() == 1) {
        ValueType handleVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(handleVal)) {
            std::string handle = std::get<std::string>(handleVal);
            if (handle.length() >= 15 && handle.substr(0, 15) == "library_handle:") {
                std::string libName = handle.substr(15); // Remove "library_handle:" prefix
                try {
                    auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
                    bool success = safe_ffi_manager.unload_library(libName);
                    lastValue = success ? 1.0 : 0.0; // Return double for consistency
                } catch (const rbasic::ffi::SafeFFIError& /* e */) {
                    lastValue = 0.0;
                } catch (...) {
                    lastValue = 0.0;
                }
            } else {
                lastValue = 0.0;
            }
        } else {
            lastValue = 0.0;
        }
        return true;
    }
    
    
    // Check if it's a declared FFI function
    auto ffiIt = ffiFunctions.find(node.name);
    if (ffiIt != ffiFunctions.end()) {
        const auto& ffiFunc = *ffiIt->second;
        
        try {
            // Load library if not already loaded using Safe FFI
            auto& safe_ffi_manager = rbasic::ffi::SafeFFIManager::instance();
            auto library = safe_ffi_manager.get_library(ffiFunc.library);
            if (!library) {
                library = safe_ffi_manager.load_library(ffiFunc.library);
            }
            
            if (!library || !library->is_valid()) {
                throw RuntimeError("Failed to load library: " + ffiFunc.library);
            }

            // Get function pointer
            void* funcPtr = library->get_function_address(ffiFunc.name);
            if (!funcPtr) {
                throw RuntimeError("Function not found in library: " + ffiFunc.name);
            }
            
            // Use generic FFI calling system
            if (callGenericFFIFunction(ffiFunc, node, funcPtr)) {
                return true;
            }
            
            // Fallback error
            throw RuntimeError("Failed to call FFI function: " + ffiFunc.name);
        } catch (const rbasic::ffi::SafeFFIError& e) {
            throw RuntimeError("Safe FFI call failed: " + std::string(e.what()));
        } catch (const std::exception& e) {
            throw RuntimeError("FFI call failed: " + std::string(e.what()));
        }
        
        return true;
    }
    
    return false; // Function not handled by this dispatcher
}

bool Interpreter::callGenericFFIFunction(const FFIFunctionDecl& ffiFunc, CallExpr& node, void* funcPtr) {
    // Generic FFI function calling for SDL2, SQLite, and all library patterns
    
    try {
        // Check argument count
        if (node.arguments.size() != ffiFunc.parameters.size()) {
            throw RuntimeError("Function " + ffiFunc.name + " expects " + 
                             std::to_string(ffiFunc.parameters.size()) + " arguments, got " + 
                             std::to_string(node.arguments.size()));
        }
        
        // Determine return type
        bool returnsPointer = (ffiFunc.returnType == "pointer" || ffiFunc.returnType.find('*') != std::string::npos);
        bool returnsInteger = (ffiFunc.returnType == "int" || ffiFunc.returnType == "integer");
        bool returnsString = (ffiFunc.returnType == "string" || ffiFunc.returnType == "char*");
        // bool returnsVoid = (ffiFunc.returnType == "void");  // Currently unused
        
        // Handle function calls based on parameter count and signature
        if (ffiFunc.parameters.size() == 0) {
            // No parameters - common for system functions
            if (returnsInteger) {
                typedef int (*Func0)();
                auto func = reinterpret_cast<Func0>(funcPtr);
                lastValue = static_cast<double>(func());
            } else if (returnsPointer) {
                typedef void* (*Func0)();
                auto func = reinterpret_cast<Func0>(funcPtr);
                lastValue = func();
            } else if (returnsString) {
                typedef const char* (*Func0)();
                auto func = reinterpret_cast<Func0>(funcPtr);
                const char* result = func();
                lastValue = std::string(result ? result : "");
            } else {
                typedef void (*Func0)();
                auto func = reinterpret_cast<Func0>(funcPtr);
                func();
                lastValue = 0.0;
            }
        } else if (ffiFunc.parameters.size() == 1) {
            // Single parameter
            ValueType argVal = evaluate(*node.arguments[0]);
            const auto& paramType = ffiFunc.parameters[0].second;
            
            if (paramType == "int" || paramType == "integer") {
                int param = getIntValue(argVal);
                if (returnsInteger) {
                    typedef int (*Func1)(int);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    lastValue = static_cast<double>(func(param));
                } else if (returnsPointer) {
                    typedef void* (*Func1)(int);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    lastValue = func(param);
                } else if (returnsString) {
                    typedef const char* (*Func1)(int);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    const char* result = func(param);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func1)(int);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    func(param);
                    lastValue = 0.0;
                }
            } else if (paramType == "string") {
                std::string str = getStringValue(argVal);
                if (returnsInteger) {
                    typedef int (*Func1)(const char*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    lastValue = static_cast<double>(func(str.c_str()));
                } else if (returnsPointer) {
                    typedef void* (*Func1)(const char*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    lastValue = func(str.c_str());
                } else if (returnsString) {
                    typedef const char* (*Func1)(const char*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    const char* result = func(str.c_str());
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func1)(const char*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    func(str.c_str());
                    lastValue = 0.0;
                }
            } else if (paramType == "pointer" || paramType.find('*') != std::string::npos) {
                void* param = getPointerValue(argVal);
                if (returnsInteger) {
                    typedef int (*Func1)(void*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    lastValue = static_cast<double>(func(param));
                } else if (returnsPointer) {
                    typedef void* (*Func1)(void*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    lastValue = func(param);
                } else if (returnsString) {
                    typedef const char* (*Func1)(void*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    const char* result = func(param);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func1)(void*);
                    auto func = reinterpret_cast<Func1>(funcPtr);
                    func(param);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 2) {
            // Two parameters - common SDL/SQLite patterns
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            const auto& param1Type = ffiFunc.parameters[0].second;
            const auto& param2Type = ffiFunc.parameters[1].second;
            
            // Pattern: (string, pointer) - SQLite functions
            if (param1Type == "string" && 
                (param2Type == "pointer" || param2Type.find('*') != std::string::npos)) {
                std::string param1 = getStringValue(arg1Val);
                void* param2 = getPointerValue(arg2Val);
                
                if (returnsInteger) {
                    typedef int (*Func2)(const char*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = static_cast<double>(func(param1.c_str(), param2));
                } else if (returnsPointer) {
                    typedef void* (*Func2)(const char*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = func(param1.c_str(), param2);
                } else {
                    typedef void (*Func2)(const char*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    func(param1.c_str(), param2);
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, int)
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) && 
                (param2Type == "int" || param2Type == "integer")) {
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                
                if (returnsInteger) {
                    typedef int (*Func2)(void*, int);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2));
                } else if (returnsPointer) {
                    typedef void* (*Func2)(void*, int);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = func(param1, param2);
                } else {
                    typedef void (*Func2)(void*, int);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    func(param1, param2);
                    lastValue = 0.0;
                }
            }
            // Pattern: (int, int)
            else if ((param1Type == "int" || param1Type == "integer") && 
                     (param2Type == "int" || param2Type == "integer")) {
                int param1 = getIntValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                
                if (returnsInteger) {
                    typedef int (*Func2)(int, int);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2));
                } else if (returnsPointer) {
                    typedef void* (*Func2)(int, int);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = func(param1, param2);
                } else {
                    typedef void (*Func2)(int, int);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    func(param1, param2);
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, pointer) -> int (SDL2 RenderDrawRect, RenderFillRect)
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     (param2Type == "pointer" || param2Type.find('*') != std::string::npos)) {
                void* param1 = getPointerValue(arg1Val);
                void* param2 = getPointerValue(arg2Val);
                
                if (returnsInteger) {
                    typedef int (*Func2)(void*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2));
                } else if (returnsPointer) {
                    typedef void* (*Func2)(void*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = func(param1, param2);
                } else {
                    typedef void (*Func2)(void*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    func(param1, param2);
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, string) -> void - SDL_SetWindowTitle
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     param2Type == "string") {
                void* param1 = getPointerValue(arg1Val);
                std::string param2 = getStringValue(arg2Val);
                
                if (returnsInteger) {
                    typedef int (*Func2)(void*, const char*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2.c_str()));
                } else if (returnsPointer) {
                    typedef void* (*Func2)(void*, const char*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = func(param1, param2.c_str());
                } else {
                    typedef void (*Func2)(void*, const char*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    func(param1, param2.c_str());
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, pointer) - SDL_GetMouseState, output parameters
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     (param2Type == "pointer" || param2Type.find('*') != std::string::npos)) {
                void* param1 = getPointerValue(arg1Val);
                void* param2 = getPointerValue(arg2Val);
                
                if (returnsInteger) {
                    typedef int (*Func2)(void*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2));
                } else if (returnsPointer) {
                    typedef void* (*Func2)(void*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    lastValue = func(param1, param2);
                } else {
                    typedef void (*Func2)(void*, void*);
                    auto func = reinterpret_cast<Func2>(funcPtr);
                    func(param1, param2);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 3) {
            // Three parameters - SDL renderer creation patterns
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            const auto& param1Type = ffiFunc.parameters[0].second;
            const auto& param2Type = ffiFunc.parameters[1].second;
            const auto& param3Type = ffiFunc.parameters[2].second;
            
            // Pattern: (pointer, int, int) - SDL_CreateRenderer
            if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) && 
                (param2Type == "int" || param2Type == "integer") &&
                (param3Type == "int" || param3Type == "integer")) {
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                
                if (returnsPointer) {
                    typedef void* (*Func3)(void*, int, int);
                    auto func = reinterpret_cast<Func3>(funcPtr);
                    lastValue = func(param1, param2, param3);
                } else if (returnsInteger) {
                    typedef int (*Func3)(void*, int, int);
                    auto func = reinterpret_cast<Func3>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3));
                } else {
                    typedef void (*Func3)(void*, int, int);
                    auto func = reinterpret_cast<Func3>(funcPtr);
                    func(param1, param2, param3);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 4) {
            // Four parameters - critical SDL2 and Windows API patterns
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            const auto& param1Type = ffiFunc.parameters[0].second;
            const auto& param2Type = ffiFunc.parameters[1].second;
            const auto& param3Type = ffiFunc.parameters[2].second;
            const auto& param4Type = ffiFunc.parameters[3].second;
            
            // Pattern: (int, string, string, int) - MessageBoxA
            if ((param1Type == "int" || param1Type == "integer") && param2Type == "string" &&
                param3Type == "string" && (param4Type == "int" || param4Type == "integer")) {
                int param1 = getIntValue(arg1Val);
                std::string str2 = getStringValue(arg2Val);
                std::string str3 = getStringValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                
                if (returnsInteger) {
                    typedef int (*Func4)(int, const char*, const char*, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = static_cast<double>(func(param1, str2.c_str(), str3.c_str(), param4));
                } else if (returnsPointer) {
                    typedef void* (*Func4)(int, const char*, const char*, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = func(param1, str2.c_str(), str3.c_str(), param4);
                } else {
                    typedef void (*Func4)(int, const char*, const char*, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    func(param1, str2.c_str(), str3.c_str(), param4);
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, pointer, pointer, pointer) -> int - SDL_RenderCopy
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     (param2Type == "pointer" || param2Type.find('*') != std::string::npos) &&
                     (param3Type == "pointer" || param3Type.find('*') != std::string::npos) &&
                     (param4Type == "pointer" || param4Type.find('*') != std::string::npos)) {
                
                void* param1 = getPointerValue(arg1Val);
                void* param2 = getPointerValue(arg2Val);
                void* param3 = getPointerValue(arg3Val);
                void* param4 = getPointerValue(arg4Val);
                
                if (returnsInteger) {
                    typedef int (*Func4)(void*, void*, void*, void*);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4));
                } else if (returnsPointer) {
                    typedef void* (*Func4)(void*, void*, void*, void*);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = func(param1, param2, param3, param4);
                } else {
                    typedef void (*Func4)(void*, void*, void*, void*);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    func(param1, param2, param3, param4);
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, int, int, int) -> pointer - SDL_CreateTexture  
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     (param2Type == "int" || param2Type == "integer") &&
                     (param3Type == "int" || param3Type == "integer") &&
                     (param4Type == "int" || param4Type == "integer")) {
                
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                
                if (returnsInteger) {
                    typedef int (*Func4)(void*, int, int, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4));
                } else if (returnsPointer) {
                    typedef void* (*Func4)(void*, int, int, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = func(param1, param2, param3, param4);
                } else {
                    typedef void (*Func4)(void*, int, int, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    func(param1, param2, param3, param4);
                    lastValue = 0.0;
                }
            }
            // Pattern: (pointer, pointer, pointer, int) -> int - SDL_UpdateTexture
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     (param2Type == "pointer" || param2Type.find('*') != std::string::npos) &&
                     (param3Type == "pointer" || param3Type.find('*') != std::string::npos) &&
                     (param4Type == "int" || param4Type == "integer")) {
                
                void* param1 = getPointerValue(arg1Val);
                void* param2 = getPointerValue(arg2Val);
                void* param3 = getPointerValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                
                if (returnsInteger) {
                    typedef int (*Func4)(void*, void*, void*, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4));
                } else if (returnsPointer) {
                    typedef void* (*Func4)(void*, void*, void*, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    lastValue = func(param1, param2, param3, param4);
                } else {
                    typedef void (*Func4)(void*, void*, void*, int);
                    auto func = reinterpret_cast<Func4>(funcPtr);
                    func(param1, param2, param3, param4);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 5) {
            // Five parameters - SQLite patterns like sqlite3_prepare_v2
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            
            const auto& param1Type = ffiFunc.parameters[0].second;
            const auto& param2Type = ffiFunc.parameters[1].second;
            const auto& param3Type = ffiFunc.parameters[2].second;
            const auto& param4Type = ffiFunc.parameters[3].second;
            const auto& param5Type = ffiFunc.parameters[4].second;
            
            // Pattern: (pointer, string, int, pointer, pointer) - sqlite3_prepare_v2
            if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                param2Type == "string" &&
                (param3Type == "int" || param3Type == "integer") &&
                (param4Type == "pointer" || param4Type.find('*') != std::string::npos) &&
                (param5Type == "pointer" || param5Type.find('*') != std::string::npos)) {
                
                void* param1 = getPointerValue(arg1Val);
                std::string param2 = getStringValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                void* param4 = getPointerValue(arg4Val);
                void* param5 = getPointerValue(arg5Val);
                
                if (returnsInteger) {
                    typedef int (*Func5)(void*, const char*, int, void*, void*);
                    auto func = reinterpret_cast<Func5>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2.c_str(), param3, param4, param5));
                } else if (returnsPointer) {
                    typedef void* (*Func5)(void*, const char*, int, void*, void*);
                    auto func = reinterpret_cast<Func5>(funcPtr);
                    lastValue = func(param1, param2.c_str(), param3, param4, param5);
                } else {
                    typedef void (*Func5)(void*, const char*, int, void*, void*);
                    auto func = reinterpret_cast<Func5>(funcPtr);
                    func(param1, param2.c_str(), param3, param4, param5);
                    lastValue = 0.0;
                }
            }
            
            // Pattern: (pointer, int, int, int, int) - SDL2 SetRenderDrawColor, RenderDrawLine
            else if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) &&
                     (param2Type == "int" || param2Type == "integer") &&
                     (param3Type == "int" || param3Type == "integer") &&
                     (param4Type == "int" || param4Type == "integer") &&
                     (param5Type == "int" || param5Type == "integer")) {
                
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                
                if (returnsInteger) {
                    typedef int (*Func5)(void*, int, int, int, int);
                    auto func = reinterpret_cast<Func5>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5));
                } else if (returnsPointer) {
                    typedef void* (*Func5)(void*, int, int, int, int);
                    auto func = reinterpret_cast<Func5>(funcPtr);
                    lastValue = func(param1, param2, param3, param4, param5);
                } else {
                    typedef void (*Func5)(void*, int, int, int, int);
                    auto func = reinterpret_cast<Func5>(funcPtr);
                    func(param1, param2, param3, param4, param5);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 6) {
            // Six parameters - SDL_CreateWindow pattern
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            ValueType arg6Val = evaluate(*node.arguments[5]);
            
            const auto& param1Type = ffiFunc.parameters[0].second;
            
            // Pattern: (string, int, int, int, int, int) - SDL_CreateWindow
            if (param1Type == "string") {
                std::string str1 = getStringValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                
                if (returnsPointer) {
                    typedef void* (*Func6)(const char*, int, int, int, int, int);
                    auto func = reinterpret_cast<Func6>(funcPtr);
                    lastValue = func(str1.c_str(), param2, param3, param4, param5, param6);
                } else if (returnsInteger) {
                    typedef int (*Func6)(const char*, int, int, int, int, int);
                    auto func = reinterpret_cast<Func6>(funcPtr);
                    lastValue = static_cast<double>(func(str1.c_str(), param2, param3, param4, param5, param6));
                } else if (returnsString) {
                    typedef const char* (*Func6)(const char*, int, int, int, int, int);
                    auto func = reinterpret_cast<Func6>(funcPtr);
                    const char* result = func(str1.c_str(), param2, param3, param4, param5, param6);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func6)(const char*, int, int, int, int, int);
                    auto func = reinterpret_cast<Func6>(funcPtr);
                    func(str1.c_str(), param2, param3, param4, param5, param6);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 7) {
            // Seven parameters - advanced library functions
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            ValueType arg6Val = evaluate(*node.arguments[5]);
            ValueType arg7Val = evaluate(*node.arguments[6]);
            
            // Generic pattern: assume all int parameters for now
            int param1 = getIntValue(arg1Val);
            int param2 = getIntValue(arg2Val);
            int param3 = getIntValue(arg3Val);
            int param4 = getIntValue(arg4Val);
            int param5 = getIntValue(arg5Val);
            int param6 = getIntValue(arg6Val);
            int param7 = getIntValue(arg7Val);
            
            if (returnsInteger) {
                typedef int (*Func7)(int, int, int, int, int, int, int);
                auto func = reinterpret_cast<Func7>(funcPtr);
                lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7));
            } else if (returnsPointer) {
                typedef void* (*Func7)(int, int, int, int, int, int, int);
                auto func = reinterpret_cast<Func7>(funcPtr);
                lastValue = func(param1, param2, param3, param4, param5, param6, param7);
            } else if (returnsString) {
                typedef const char* (*Func7)(int, int, int, int, int, int, int);
                auto func = reinterpret_cast<Func7>(funcPtr);
                const char* result = func(param1, param2, param3, param4, param5, param6, param7);
                lastValue = std::string(result ? result : "");
            } else {
                typedef void (*Func7)(int, int, int, int, int, int, int);
                auto func = reinterpret_cast<Func7>(funcPtr);
                func(param1, param2, param3, param4, param5, param6, param7);
                lastValue = 0.0;
            }
        } else if (ffiFunc.parameters.size() == 8) {
            // Eight parameters - maximum complexity library functions
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            ValueType arg6Val = evaluate(*node.arguments[5]);
            ValueType arg7Val = evaluate(*node.arguments[6]);
            ValueType arg8Val = evaluate(*node.arguments[7]);
            
            const auto& param1Type = ffiFunc.parameters[0].second;
            const auto& param7Type = ffiFunc.parameters[6].second;
            
            // Pattern: (pointer, int, int, int, int, int, int, int) - SDL2_gfx circle functions
            if (param1Type == "pointer" && param7Type == "integer") {
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                
                if (returnsInteger) {
                    typedef int (*Func8)(void*, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8));
                } else {
                    typedef void (*Func8)(void*, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8);
                    lastValue = 0.0;
                }
            // Pattern: (pointer, pointer, pointer, pointer, double, pointer, double, int) - SDL_RenderCopyEx
            } else if (param1Type == "pointer" && param7Type == "double") {
                void* param1 = getPointerValue(arg1Val);
                void* param2 = getPointerValue(arg2Val);
                void* param3 = getPointerValue(arg3Val);
                void* param4 = getPointerValue(arg4Val);
                double param5 = getDoubleValue(arg5Val);
                void* param6 = getPointerValue(arg6Val);
                double param7 = getDoubleValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                
                if (returnsInteger) {
                    typedef int (*Func8)(void*, void*, void*, void*, double, void*, double, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8));
                } else {
                    typedef void (*Func8)(void*, void*, void*, void*, double, void*, double, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8);
                    lastValue = 0.0;
                }
            } else {
                // Generic pattern: assume all int parameters for compatibility
                int param1 = getIntValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                
                if (returnsInteger) {
                    typedef int (*Func8)(int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8));
                } else if (returnsPointer) {
                    typedef void* (*Func8)(int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    lastValue = func(param1, param2, param3, param4, param5, param6, param7, param8);
                } else if (returnsString) {
                    typedef const char* (*Func8)(int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    const char* result = func(param1, param2, param3, param4, param5, param6, param7, param8);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func8)(int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func8>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 9) {
            // Nine parameters
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            ValueType arg6Val = evaluate(*node.arguments[5]);
            ValueType arg7Val = evaluate(*node.arguments[6]);
            ValueType arg8Val = evaluate(*node.arguments[7]);
            ValueType arg9Val = evaluate(*node.arguments[8]);
            
            const auto& param1Type = ffiFunc.parameters[0].second;
            
            // Pattern: (pointer, int, int, int, int, int, int, int, int) - SDL2_gfx ellipse functions  
            if (param1Type == "pointer") {
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                int param9 = getIntValue(arg9Val);
                
                if (returnsInteger) {
                    typedef int (*Func9)(void*, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func9>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8, param9));
                } else {
                    typedef void (*Func9)(void*, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func9>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8, param9);
                    lastValue = 0.0;
                }
            } else {
                // Generic pattern: assume all int parameters for compatibility
                int param1 = getIntValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                int param9 = getIntValue(arg9Val);
                
                if (returnsInteger) {
                    typedef int (*Func9)(int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func9>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8, param9));
                } else if (returnsPointer) {
                    typedef void* (*Func9)(int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func9>(funcPtr);
                    lastValue = func(param1, param2, param3, param4, param5, param6, param7, param8, param9);
                } else if (returnsString) {
                    typedef const char* (*Func9)(int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func9>(funcPtr);
                    const char* result = func(param1, param2, param3, param4, param5, param6, param7, param8, param9);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func9)(int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func9>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8, param9);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 10) {
            // Ten parameters - for functions like filledTrigonRGBA
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            ValueType arg6Val = evaluate(*node.arguments[5]);
            ValueType arg7Val = evaluate(*node.arguments[6]);
            ValueType arg8Val = evaluate(*node.arguments[7]);
            ValueType arg9Val = evaluate(*node.arguments[8]);
            ValueType arg10Val = evaluate(*node.arguments[9]);
            
            const auto& param1Type = ffiFunc.parameters[0].second;
            
            // Pattern: (pointer, int, int, int, int, int, int, int, int, int) - filledTrigonRGBA
            if (param1Type == "pointer") {
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                int param9 = getIntValue(arg9Val);
                int param10 = getIntValue(arg10Val);
                
                if (returnsInteger) {
                    typedef int (*Func10)(void*, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func10>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10));
                } else {
                    typedef void (*Func10)(void*, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func10>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
                    lastValue = 0.0;
                }
            } else {
                // Generic pattern: assume all int parameters
                int param1 = getIntValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                int param9 = getIntValue(arg9Val);
                int param10 = getIntValue(arg10Val);
                
                if (returnsInteger) {
                    typedef int (*Func10)(int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func10>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10));
                } else if (returnsPointer) {
                    typedef void* (*Func10)(int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func10>(funcPtr);
                    lastValue = func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
                } else if (returnsString) {
                    typedef const char* (*Func10)(int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func10>(funcPtr);
                    const char* result = func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func10)(int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func10>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
                    lastValue = 0.0;
                }
            }
        } else if (ffiFunc.parameters.size() == 11) {
            // Eleven parameters - for functions like filledTrigonRGBA
            ValueType arg1Val = evaluate(*node.arguments[0]);
            ValueType arg2Val = evaluate(*node.arguments[1]);
            ValueType arg3Val = evaluate(*node.arguments[2]);
            ValueType arg4Val = evaluate(*node.arguments[3]);
            ValueType arg5Val = evaluate(*node.arguments[4]);
            ValueType arg6Val = evaluate(*node.arguments[5]);
            ValueType arg7Val = evaluate(*node.arguments[6]);
            ValueType arg8Val = evaluate(*node.arguments[7]);
            ValueType arg9Val = evaluate(*node.arguments[8]);
            ValueType arg10Val = evaluate(*node.arguments[9]);
            ValueType arg11Val = evaluate(*node.arguments[10]);
            
            const auto& param1Type = ffiFunc.parameters[0].second;
            
            // Pattern: (pointer, int, int, int, int, int, int, int, int, int, int) - filledTrigonRGBA
            if (param1Type == "pointer") {
                void* param1 = getPointerValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                int param9 = getIntValue(arg9Val);
                int param10 = getIntValue(arg10Val);
                int param11 = getIntValue(arg11Val);
                
                if (returnsInteger) {
                    typedef int (*Func11)(void*, int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func11>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11));
                } else {
                    typedef void (*Func11)(void*, int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func11>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
                    lastValue = 0.0;
                }
            } else {
                // Generic pattern: assume all int parameters
                int param1 = getIntValue(arg1Val);
                int param2 = getIntValue(arg2Val);
                int param3 = getIntValue(arg3Val);
                int param4 = getIntValue(arg4Val);
                int param5 = getIntValue(arg5Val);
                int param6 = getIntValue(arg6Val);
                int param7 = getIntValue(arg7Val);
                int param8 = getIntValue(arg8Val);
                int param9 = getIntValue(arg9Val);
                int param10 = getIntValue(arg10Val);
                int param11 = getIntValue(arg11Val);
                
                if (returnsInteger) {
                    typedef int (*Func11)(int, int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func11>(funcPtr);
                    lastValue = static_cast<double>(func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11));
                } else if (returnsPointer) {
                    typedef void* (*Func11)(int, int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func11>(funcPtr);
                    lastValue = func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
                } else if (returnsString) {
                    typedef const char* (*Func11)(int, int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func11>(funcPtr);
                    const char* result = func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
                    lastValue = std::string(result ? result : "");
                } else {
                    typedef void (*Func11)(int, int, int, int, int, int, int, int, int, int, int);
                    auto func = reinterpret_cast<Func11>(funcPtr);
                    func(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);
                    lastValue = 0.0;
                }
            }
        } else {
            // Unsupported parameter count
            throw RuntimeError("FFI functions with " + std::to_string(ffiFunc.parameters.size()) + 
                             " parameters not yet implemented for " + ffiFunc.name);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw RuntimeError("FFI call failed for " + ffiFunc.name + ": " + std::string(e.what()));
    }
}

// Helper functions for type conversion
int Interpreter::getIntValue(const ValueType& value) {
    if (std::holds_alternative<double>(value)) {
        return static_cast<int>(std::get<double>(value));
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    }
    return 0;
}

std::string Interpreter::getStringValue(const ValueType& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return "";
}

void* Interpreter::getPointerValue(const ValueType& value) {
    if (std::holds_alternative<void*>(value)) {
        return std::get<void*>(value);
    }
    return nullptr;
}

double Interpreter::getDoubleValue(const ValueType& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    }
    return 0.0;
}

bool Interpreter::handleUserDefinedFunction(CallExpr& node) {
    // User-defined function call
    auto funcIt = functions.find(node.name);
    if (funcIt != functions.end()) {
        const auto& func = *funcIt->second;  // Dereference the unique_ptr
        
        // Check argument count
        if (node.arguments.size() != func.parameters.size()) {
            throw RuntimeError("Function " + node.name + " expects " + 
                             std::to_string(func.parameters.size()) + " arguments, got " + 
                             std::to_string(node.arguments.size()));
        }
        
        // Evaluate arguments in the CURRENT scope before creating new scope
        std::vector<ValueType> argValues;
        for (size_t i = 0; i < node.arguments.size(); i++) {
            ValueType argValue = evaluate(*node.arguments[i]);
            argValues.push_back(argValue);
        }
        
        // Create new scope for function
        pushScope();
        
        // Bind parameters using pre-evaluated arguments
        for (size_t i = 0; i < func.parameters.size(); i++) {
            defineVariable(func.parameters[i], argValues[i]);
        }
        
        // Execute function body
        hasReturned = false;
        for (auto& stmt : func.body) {
            stmt->accept(*this);
            if (hasReturned) break;
        }
        
        popScope();
        hasReturned = false;
        return true;
    }
    
    return false; // Function not handled by this dispatcher
}

void Interpreter::visit(StructLiteralExpr& node) {
    // Check if struct type exists
    auto structIt = structs.find(node.structName);
    if (structIt == structs.end()) {
        throw RuntimeError("Unknown struct type: " + node.structName);
    }
    
    // Create struct instance with field values
    StructValue structValue(node.structName);
    const auto& structDecl = *structIt->second;
    
    if (node.values.size() != structDecl.fields.size()) {
        throw RuntimeError("Struct '" + node.structName + "' expects " + 
                          std::to_string(structDecl.fields.size()) + " values, got " + 
                          std::to_string(node.values.size()));
    }
    
    // Initialize fields with provided values
    for (size_t i = 0; i < node.values.size(); i++) {
        ValueType value = evaluate(*node.values[i]);
        const std::string& fieldName = structDecl.fields[i];
        
        // Convert ValueType to the variant type used in StructValue
        if (std::holds_alternative<int>(value)) {
            structValue.fields[fieldName] = std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            structValue.fields[fieldName] = std::get<double>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            structValue.fields[fieldName] = std::get<std::string>(value);
        } else if (std::holds_alternative<bool>(value)) {
            structValue.fields[fieldName] = std::get<bool>(value);
        } else {
            throw RuntimeError("Unsupported value type for struct field");
        }
    }
    
    lastValue = structValue;
}

void Interpreter::visit(GLMConstructorExpr& node) {
    switch (node.glmType) {
        case TokenType::VEC2: {
            if (node.arguments.size() != 2) {
                throw RuntimeError("vec2 constructor expects 2 arguments, got " + std::to_string(node.arguments.size()));
            }
            ValueType x_val = evaluate(*node.arguments[0]);
            ValueType y_val = evaluate(*node.arguments[1]);
            
            float x = std::holds_alternative<double>(x_val) ? static_cast<float>(std::get<double>(x_val)) :
                     std::holds_alternative<int>(x_val) ? static_cast<float>(std::get<int>(x_val)) : 0.0f;
            float y = std::holds_alternative<double>(y_val) ? static_cast<float>(std::get<double>(y_val)) :
                     std::holds_alternative<int>(y_val) ? static_cast<float>(std::get<int>(y_val)) : 0.0f;
            
            lastValue = Vec2Value(x, y);
            break;
        }
        case TokenType::VEC3: {
            if (node.arguments.size() != 3) {
                throw RuntimeError("vec3 constructor expects 3 arguments, got " + std::to_string(node.arguments.size()));
            }
            ValueType x_val = evaluate(*node.arguments[0]);
            ValueType y_val = evaluate(*node.arguments[1]);
            ValueType z_val = evaluate(*node.arguments[2]);
            
            float x = std::holds_alternative<double>(x_val) ? static_cast<float>(std::get<double>(x_val)) :
                     std::holds_alternative<int>(x_val) ? static_cast<float>(std::get<int>(x_val)) : 0.0f;
            float y = std::holds_alternative<double>(y_val) ? static_cast<float>(std::get<double>(y_val)) :
                     std::holds_alternative<int>(y_val) ? static_cast<float>(std::get<int>(y_val)) : 0.0f;
            float z = std::holds_alternative<double>(z_val) ? static_cast<float>(std::get<double>(z_val)) :
                     std::holds_alternative<int>(z_val) ? static_cast<float>(std::get<int>(z_val)) : 0.0f;
            
            lastValue = Vec3Value(x, y, z);
            break;
        }
        case TokenType::VEC4: {
            if (node.arguments.size() != 4) {
                throw RuntimeError("vec4 constructor expects 4 arguments, got " + std::to_string(node.arguments.size()));
            }
            ValueType x_val = evaluate(*node.arguments[0]);
            ValueType y_val = evaluate(*node.arguments[1]);
            ValueType z_val = evaluate(*node.arguments[2]);
            ValueType w_val = evaluate(*node.arguments[3]);
            
            float x = std::holds_alternative<double>(x_val) ? static_cast<float>(std::get<double>(x_val)) :
                     std::holds_alternative<int>(x_val) ? static_cast<float>(std::get<int>(x_val)) : 0.0f;
            float y = std::holds_alternative<double>(y_val) ? static_cast<float>(std::get<double>(y_val)) :
                     std::holds_alternative<int>(y_val) ? static_cast<float>(std::get<int>(y_val)) : 0.0f;
            float z = std::holds_alternative<double>(z_val) ? static_cast<float>(std::get<double>(z_val)) :
                     std::holds_alternative<int>(z_val) ? static_cast<float>(std::get<int>(z_val)) : 0.0f;
            float w = std::holds_alternative<double>(w_val) ? static_cast<float>(std::get<double>(w_val)) :
                     std::holds_alternative<int>(w_val) ? static_cast<float>(std::get<int>(w_val)) : 0.0f;
            
            lastValue = Vec4Value(x, y, z, w);
            break;
        }
        case TokenType::MAT3: {
            if (node.arguments.size() == 0) {
                lastValue = Mat3Value(); // Identity matrix
            } else if (node.arguments.size() == 9) {
                std::vector<float> elements;
                for (int i = 0; i < 9; i++) {
                    ValueType val = evaluate(*node.arguments[i]);
                    float f = std::holds_alternative<double>(val) ? static_cast<float>(std::get<double>(val)) :
                             std::holds_alternative<int>(val) ? static_cast<float>(std::get<int>(val)) : 0.0f;
                    elements.push_back(f);
                }
                glm::mat3 mat(elements[0], elements[1], elements[2], 
                             elements[3], elements[4], elements[5],
                             elements[6], elements[7], elements[8]);
                lastValue = Mat3Value(mat);
            } else {
                throw RuntimeError("mat3 constructor expects 0 or 9 arguments, got " + std::to_string(node.arguments.size()));
            }
            break;
        }
        case TokenType::MAT4: {
            if (node.arguments.size() == 0) {
                lastValue = Mat4Value(); // Identity matrix
            } else if (node.arguments.size() == 16) {
                std::vector<float> elements;
                for (int i = 0; i < 16; i++) {
                    ValueType val = evaluate(*node.arguments[i]);
                    float f = std::holds_alternative<double>(val) ? static_cast<float>(std::get<double>(val)) :
                             std::holds_alternative<int>(val) ? static_cast<float>(std::get<int>(val)) : 0.0f;
                    elements.push_back(f);
                }
                glm::mat4 mat(elements[0], elements[1], elements[2], elements[3],
                             elements[4], elements[5], elements[6], elements[7],
                             elements[8], elements[9], elements[10], elements[11],
                             elements[12], elements[13], elements[14], elements[15]);
                lastValue = Mat4Value(mat);
            } else {
                throw RuntimeError("mat4 constructor expects 0 or 16 arguments, got " + std::to_string(node.arguments.size()));
            }
            break;
        }
        case TokenType::QUAT: {
            if (node.arguments.size() == 0) {
                lastValue = QuatValue(); // Identity quaternion
            } else if (node.arguments.size() == 4) {
                ValueType w_val = evaluate(*node.arguments[0]);
                ValueType x_val = evaluate(*node.arguments[1]);
                ValueType y_val = evaluate(*node.arguments[2]);
                ValueType z_val = evaluate(*node.arguments[3]);
                
                float w = std::holds_alternative<double>(w_val) ? static_cast<float>(std::get<double>(w_val)) :
                         std::holds_alternative<int>(w_val) ? static_cast<float>(std::get<int>(w_val)) : 1.0f;
                float x = std::holds_alternative<double>(x_val) ? static_cast<float>(std::get<double>(x_val)) :
                         std::holds_alternative<int>(x_val) ? static_cast<float>(std::get<int>(x_val)) : 0.0f;
                float y = std::holds_alternative<double>(y_val) ? static_cast<float>(std::get<double>(y_val)) :
                         std::holds_alternative<int>(y_val) ? static_cast<float>(std::get<int>(y_val)) : 0.0f;
                float z = std::holds_alternative<double>(z_val) ? static_cast<float>(std::get<double>(z_val)) :
                         std::holds_alternative<int>(z_val) ? static_cast<float>(std::get<int>(z_val)) : 0.0f;
                
                lastValue = QuatValue(w, x, y, z);
            } else {
                throw RuntimeError("quat constructor expects 0 or 4 arguments, got " + std::to_string(node.arguments.size()));
            }
            break;
        }
        default:
            throw RuntimeError("Unknown GLM type in constructor");
    }
}

void Interpreter::visit(GLMComponentAccessExpr& node) {
    ValueType objectValue = evaluate(*node.object);
    
    if (std::holds_alternative<Vec2Value>(objectValue)) {
        const Vec2Value& vec = std::get<Vec2Value>(objectValue);
        if (node.component == "x") {
            lastValue = static_cast<double>(vec.data.x);
        } else if (node.component == "y") {
            lastValue = static_cast<double>(vec.data.y);
        } else {
            throw RuntimeError("Invalid component '" + node.component + "' for vec2");
        }
    } else if (std::holds_alternative<Vec3Value>(objectValue)) {
        const Vec3Value& vec = std::get<Vec3Value>(objectValue);
        if (node.component == "x") {
            lastValue = static_cast<double>(vec.data.x);
        } else if (node.component == "y") {
            lastValue = static_cast<double>(vec.data.y);
        } else if (node.component == "z") {
            lastValue = static_cast<double>(vec.data.z);
        } else {
            throw RuntimeError("Invalid component '" + node.component + "' for vec3");
        }
    } else if (std::holds_alternative<Vec4Value>(objectValue)) {
        const Vec4Value& vec = std::get<Vec4Value>(objectValue);
        if (node.component == "x") {
            lastValue = static_cast<double>(vec.data.x);
        } else if (node.component == "y") {
            lastValue = static_cast<double>(vec.data.y);
        } else if (node.component == "z") {
            lastValue = static_cast<double>(vec.data.z);
        } else if (node.component == "w") {
            lastValue = static_cast<double>(vec.data.w);
        } else {
            throw RuntimeError("Invalid component '" + node.component + "' for vec4");
        }
    } else {
        throw RuntimeError("Component access not supported for this type");
    }
}

void Interpreter::visit(ExpressionStmt& node) {
    evaluate(*node.expression);
}

void Interpreter::visit(VarStmt& node) {
    ValueType value = evaluate(*node.value);
    
    // Handle struct member assignment
    if (!node.member.empty()) {
        ValueType structVar = getVariable(node.variable);
        if (std::holds_alternative<StructValue>(structVar)) {
            StructValue structVal = std::get<StructValue>(structVar);
            
            // Convert ValueType to simple variant for storage
            if (std::holds_alternative<int>(value)) {
                structVal.fields[node.member] = std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                structVal.fields[node.member] = std::get<double>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                structVal.fields[node.member] = std::get<std::string>(value);
            } else if (std::holds_alternative<bool>(value)) {
                structVal.fields[node.member] = std::get<bool>(value);
            }
            
            defineVariable(node.variable, structVal);  // Update the variable
        } else {
            throw RuntimeError("Variable '" + node.variable + "' is not a struct");
        }
    }
    // Handle array assignment
    else if (!node.indices.empty()) {
        ValueType arrayVar = getVariable(node.variable);
        
        // Evaluate all indices
        std::vector<int> indices;
        for (auto& indexExpr : node.indices) {
            indices.push_back(TypeUtils::toArrayIndex(evaluate(*indexExpr)));
        }
        
        if (std::holds_alternative<ArrayValue>(arrayVar)) {
            ArrayValue array = std::get<ArrayValue>(arrayVar);
            int flatIndex = array.calculateIndex(indices);
            
            // Convert ValueType to simple variant for storage
            if (std::holds_alternative<int>(value)) {
                array.elements[flatIndex] = std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                array.elements[flatIndex] = std::get<double>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                array.elements[flatIndex] = std::get<std::string>(value);
            } else if (std::holds_alternative<bool>(value)) {
                array.elements[flatIndex] = std::get<bool>(value);
            }
            
            setVariable(node.variable, array);  // Use setVariable instead of defineVariable
        } else if (std::holds_alternative<ByteArrayValue>(arrayVar)) {
            ByteArrayValue array = std::get<ByteArrayValue>(arrayVar);
            uint8_t byteValue = TypeUtils::getValue<uint8_t>(value);
            array.at(indices) = byteValue;
            setVariable(node.variable, array);
        } else if (std::holds_alternative<IntArrayValue>(arrayVar)) {
            IntArrayValue array = std::get<IntArrayValue>(arrayVar);
            int intValue = TypeUtils::toInt(value);
            array.at(indices) = intValue;
            setVariable(node.variable, array);
        } else if (std::holds_alternative<DoubleArrayValue>(arrayVar)) {
            DoubleArrayValue array = std::get<DoubleArrayValue>(arrayVar);
            double doubleValue = TypeUtils::toDouble(value);
            array.at(indices) = doubleValue;
            setVariable(node.variable, array);
        } else {
            throw RuntimeError("Variable '" + node.variable + "' is not an array");
        }
    } else {
        // Regular variable assignment
        defineVariable(node.variable, value);
    }
}

void Interpreter::visit(PrintStmt& node) {
    for (size_t i = 0; i < node.expressions.size(); i++) {
        ValueType value = evaluate(*node.expressions[i]);
        ioHandler->print(valueToString(value));
        if (i < node.expressions.size() - 1) {
            ioHandler->print(" ");
        }
    }
    ioHandler->newline();
}

void Interpreter::visit(InputStmt& node) {
    std::string input_text = ioHandler->input();
    
    // Try to parse as number first
    ValueType value;
    try {
        if (hasDecimalPoint(input_text)) {
            // Contains decimal point, parse as double
            value = std::stod(input_text);
        } else {
            // Try to parse as integer
            value = std::stoi(input_text);
        }
    } catch (const std::exception&) {
        // If parsing fails, store as string
        value = input_text;
    }
    
    setVariable(node.variable, value);
}

void Interpreter::visit(IfStmt& node) {
    ValueType condition = evaluate(*node.condition);
    
    if (isTruthy(condition)) {
        // Create new scope for the then branch
        pushScope();
        
        for (auto& stmt : node.thenBranch) {
            stmt->accept(*this);
            if (hasReturned) {
                popScope();
                return;
            }
        }
        
        // Pop the then branch scope
        popScope();
    } else {
        // Create new scope for the else branch
        pushScope();
        
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
            if (hasReturned) {
                popScope();
                return;
            }
        }
        
        // Pop the else branch scope
        popScope();
    }
}

void Interpreter::visit(ModernForStmt& node) {
    // Use variable backup/restore instead of isolated scope
    // This allows access to parent scope while protecting loop variable
    
    ValueType backupValue;
    bool hadVariable = false;
    
    // Check if loop variable already exists and back it up
    if (variableExists(node.variable)) {
        backupValue = getVariable(node.variable);
        hadVariable = true;
    }
    
    // Initialize loop variable in current scope (not isolated)
    ValueType initValue = evaluate(*node.initialization);
    setVariable(node.variable, initValue);
    
    // Execute the loop
    while (isTruthy(evaluate(*node.condition))) {
        // Execute body - can access all parent scope variables
        for (auto& stmt : node.body) {
            stmt->accept(*this);
            if (hasReturned) {
                // Restore on early return
                if (hadVariable) {
                    setVariable(node.variable, backupValue);
                }
                return;
            }
        }
        
        // Execute increment
        evaluate(*node.increment);
    }
    
    // Restore or remove loop variable
    if (hadVariable) {
        setVariable(node.variable, backupValue);
    } else {
        // Remove the loop variable if it didn't exist before
        if (!scopes.empty()) {
            scopes.back().erase(node.variable);
        }
    }
}

void Interpreter::visit(WhileStmt& node) {
    while (isTruthy(evaluate(*node.condition))) {
        // Execute while block without creating new scope for now
        for (auto& stmt : node.body) {
            stmt->accept(*this);
            if (hasReturned) {
                return;
            }
        }
    }
}

void Interpreter::visit(ReturnStmt& node) {
    if (node.value) {
        lastValue = evaluate(*node.value);
    } else {
        lastValue = 0;
    }
    hasReturned = true;
}

void Interpreter::visit(FunctionDecl& node) {
    functions[node.name] = std::make_unique<FunctionDecl>(
        node.name, node.parameters, node.paramTypes, node.returnType, std::vector<std::unique_ptr<Statement>>());
    
    // Move the body statements
    for (auto& stmt : node.body) {
        functions[node.name]->body.push_back(std::move(stmt));
    }
}

void Interpreter::visit(StructDecl& node) {
    structs[node.name] = std::make_unique<StructDecl>(
        node.name, node.fields, node.fieldTypes);
}

void Interpreter::visit(DimStmt& node) {
    if (node.dimensions.empty()) {
        // Simple variable declaration
        if (node.type == "integer") {
            defineVariable(node.variable, 0);
        } else if (node.type == "double") {
            defineVariable(node.variable, 0.0);
        } else if (node.type == "string") {
            defineVariable(node.variable, std::string(""));
        } else if (node.type == "boolean") {
            defineVariable(node.variable, false);
        } else {
            // Check if it's a defined struct type
            auto structIt = structs.find(node.type);
            if (structIt != structs.end()) {
                StructValue structInstance(node.type);
                
                // Initialize struct fields with default values
                for (size_t i = 0; i < structIt->second->fields.size(); i++) {
                    const std::string& fieldName = structIt->second->fields[i];
                    const std::string& fieldType = structIt->second->fieldTypes[i];
                    
                    if (fieldType == "integer") {
                        structInstance.fields[fieldName] = 0;
                    } else if (fieldType == "double") {
                        structInstance.fields[fieldName] = 0.0;
                    } else if (fieldType == "string") {
                        structInstance.fields[fieldName] = std::string("");
                    } else if (fieldType == "boolean") {
                        structInstance.fields[fieldName] = false;
                    } else {
                        structInstance.fields[fieldName] = 0; // Default for unknown types
                    }
                }
                
                defineVariable(node.variable, structInstance);
            } else {
                // Unknown type - initialize to 0
                defineVariable(node.variable, 0);
            }
        }
    } else {
        // Array declaration - evaluate dimensions and create array
        std::vector<int> dimensions;
        for (auto& dimExpr : node.dimensions) {
            dimensions.push_back(TypeUtils::toInt(evaluate(*dimExpr)));
        }
        
        ArrayValue array(dimensions);
        defineVariable(node.variable, array);
    }
}

void Interpreter::visit(ImportStmt& node) {
    // Resolve import path
    std::string filepath = resolveImportPath(node.filename);
    
    // Check for circular imports
    if (importStack.find(filepath) != importStack.end()) {
        throw std::runtime_error("Circular import detected: " + filepath);
    }
    
    // Check if already imported
    if (importedFiles.find(filepath) != importedFiles.end()) {
        return; // Already imported, skip
    }
    
    // Read and parse the imported file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open import file: " + filepath);
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // Add to import stack for circular detection
    importStack.insert(filepath);
    
    try {
        // Parse the imported file
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        
        // Execute the imported file in current context
        program->accept(*this);
        
        // Mark as imported
        importedFiles.insert(filepath);
        
    } catch (const std::exception& e) {
        importStack.erase(filepath);
        throw std::runtime_error("Error importing '" + filepath + "': " + e.what());
    }
    
    importStack.erase(filepath);
}

void Interpreter::visit(FFIFunctionDecl& node) {
    // Store FFI function declaration for later use
    auto ffiDecl = std::make_unique<FFIFunctionDecl>(node.name, node.library, node.returnType, node.parameters);
    ffiFunctions[node.name] = std::move(ffiDecl);
    // Note: The actual library loading will happen when the function is first called
}

void Interpreter::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (hasReturned) break;
    }
}

std::string Interpreter::resolveImportPath(const std::string& filename) {
    // If absolute path, use as-is
    if (std::filesystem::path(filename).is_absolute()) {
        return filename;
    }
    
    // Try relative to the directory of the current file being processed
    if (!currentFile.empty()) {
        std::filesystem::path currentFileDir = std::filesystem::path(currentFile).parent_path();
        std::filesystem::path relativePath = currentFileDir / filename;
        if (std::filesystem::exists(relativePath)) {
            return relativePath.string();
        }
    }
    
    // Try relative to current working directory
    std::filesystem::path currentPath = std::filesystem::current_path() / filename;
    if (std::filesystem::exists(currentPath)) {
        return currentPath.string();
    }
    
    // Try relative to executable directory
    std::filesystem::path execPath = std::filesystem::path(getCurrentExecutablePath()).parent_path() / filename;
    if (std::filesystem::exists(execPath)) {
        return execPath.string();
    }
    
    // Try common library paths
    std::vector<std::string> searchPaths = {
        "./examples/",   // Add examples directory for testing
        "./lib/",
        "./libs/", 
        "./stdlib/",
        "../lib/",
        "../libs/",
        "../stdlib/"
    };
    
    for (const auto& searchPath : searchPaths) {
        std::filesystem::path libPath = std::filesystem::path(searchPath) / filename;
        if (std::filesystem::exists(libPath)) {
            return libPath.string();
        }
    }
    
    // If not found anywhere, return the original filename (will cause error)
    return filename;
}

std::string Interpreter::getCurrentExecutablePath() {
#ifdef _WIN32
    // Use dynamic allocation for safety
    std::vector<char> buffer(MAX_PATH);
    DWORD result = GetModuleFileNameA(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
    
    // Handle case where buffer is too small
    while (result == buffer.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.resize(buffer.size() * 2);
        result = GetModuleFileNameA(NULL, buffer.data(), static_cast<DWORD>(buffer.size()));
    }
    
    if (result == 0) {
        return ""; // Failed to get path
    }
    
    return std::string(buffer.data(), result);
#else
    // Use dynamic allocation with proper bounds checking
    constexpr size_t initial_size = 1024;
    std::vector<char> buffer(initial_size);
    
    ssize_t len = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    
    // If buffer was too small, try with larger buffer
    if (len == static_cast<ssize_t>(buffer.size() - 1)) {
        buffer.resize(4096); // Try larger buffer
        len = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    }
    
    if (len == -1) {
        return ""; // Failed to read link
    }
    
    return std::string(buffer.data(), len);
#endif
}

} // namespace rbasic