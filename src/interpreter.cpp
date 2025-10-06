#include "interpreter.h"
#include "runtime.h"
#include "io_handler.h"
#include "type_utils.h"
#include "terminal.h"
#include "../runtime/basic_runtime.h"
#include "../include/ffi.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

// BasicValue is already available from basic_runtime.h
#ifdef _WIN32
#include <windows.h>
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
    } else if (node.operator_ == "mod") {
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
    // Built-in math functions (single argument)
    if (node.arguments.size() == 1 && 
        (node.name == "sqr" || node.name == "sqrt" || node.name == "abs" || 
         node.name == "sin" || node.name == "cos" || node.name == "tan" ||
         node.name == "asin" || node.name == "acos" || node.name == "atan" ||
         node.name == "log" || node.name == "log10" || node.name == "exp" ||
         node.name == "floor" || node.name == "ceil" || node.name == "round" ||
         node.name == "int")) {
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
        
        if (node.name == "sqr" || node.name == "sqrt") {
            lastValue = std::sqrt(numArg);
        } else if (node.name == "abs") {
            lastValue = std::abs(numArg);
        } else if (node.name == "sin") {
            lastValue = std::sin(numArg);
        } else if (node.name == "cos") {
            lastValue = std::cos(numArg);
        } else if (node.name == "tan") {
            lastValue = std::tan(numArg);
        } else if (node.name == "asin") {
            lastValue = std::asin(numArg);
        } else if (node.name == "acos") {
            lastValue = std::acos(numArg);
        } else if (node.name == "atan") {
            lastValue = std::atan(numArg);
        } else if (node.name == "log") {
            if (numArg <= 0) {
                throw RuntimeError("LOG requires a positive argument");
            }
            lastValue = std::log(numArg);
        } else if (node.name == "log10") {
            if (numArg <= 0) {
                throw RuntimeError("LOG10 requires a positive argument");
            }
            lastValue = std::log10(numArg);
        } else if (node.name == "exp") {
            lastValue = std::exp(numArg);
        } else if (node.name == "floor") {
            lastValue = std::floor(numArg);
        } else if (node.name == "ceil") {
            lastValue = std::ceil(numArg);
        } else if (node.name == "round") {
            lastValue = std::round(numArg);
        } else if (node.name == "int") {
            lastValue = static_cast<int>(numArg);
        }
        return true;
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
            } catch (const std::filesystem::filesystem_error& e) {
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
    
    // Buffer allocation functions
    if (node.name == "alloc_int_buffer" && node.arguments.size() == 0) {
        // Runtime returns BasicValue, we need to convert to ValueType
        void* ptr = std::get<void*>(basic_runtime::alloc_int_buffer());
        lastValue = ptr;
        return true;
    }
    
    if (node.name == "alloc_pointer_buffer" && node.arguments.size() == 0) {
        void* ptr = std::get<void*>(basic_runtime::alloc_pointer_buffer());
        lastValue = ptr;
        return true;
    }
    
    if (node.name == "alloc_buffer" && node.arguments.size() == 1) {
        ValueType sizeVal = evaluate(*node.arguments[0]);
        int size = std::get<int>(sizeVal);
        void* ptr = std::get<void*>(basic_runtime::alloc_buffer(size));
        lastValue = ptr;
        return true;
    }
    
    if (node.name == "deref_int" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            BasicValue bv = std::get<void*>(ptrVal);
            BasicValue result = basic_runtime::deref_int(bv);
            lastValue = std::get<double>(result);
        }
        return true;
    }
    
    if (node.name == "deref_pointer" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            BasicValue bv = std::get<void*>(ptrVal);
            BasicValue result = basic_runtime::deref_pointer(bv);
            lastValue = std::get<void*>(result);
        }
        return true;
    }
    
    if (node.name == "deref_string" && node.arguments.size() == 1) {
        ValueType ptrVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<void*>(ptrVal)) {
            BasicValue bv = std::get<void*>(ptrVal);
            BasicValue result = basic_runtime::deref_string(bv);
            lastValue = std::get<std::string>(result);
        }
        return true;
    }
    
    // SDL struct helper functions
    if (node.name == "create_sdl_rect" && node.arguments.size() == 4) {
        ValueType xVal = evaluate(*node.arguments[0]);
        ValueType yVal = evaluate(*node.arguments[1]);
        ValueType wVal = evaluate(*node.arguments[2]);
        ValueType hVal = evaluate(*node.arguments[3]);
        
        int x = std::get<int>(xVal);
        int y = std::get<int>(yVal);
        int w = std::get<int>(wVal);
        int h = std::get<int>(hVal);
        
        BasicValue result = basic_runtime::create_sdl_rect(x, y, w, h);
        lastValue = std::get<void*>(result);
        return true;
    }
    
    if (node.name == "create_sdl_event" && node.arguments.size() == 0) {
        BasicValue result = basic_runtime::create_sdl_event();
        lastValue = std::get<void*>(result);
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
    
    return false; // Function not handled by this dispatcher
}

// FFI Functions Handler - Simplified for Phase 1
bool Interpreter::handleFFIFunctions(CallExpr& node) {
    if (node.name == "load_library" && node.arguments.size() == 1) {
        ValueType libNameVal = evaluate(*node.arguments[0]);
        if (std::holds_alternative<std::string>(libNameVal)) {
            std::string libName = std::get<std::string>(libNameVal);
            // For Phase 1, just verify the library exists and return a simple success indicator
            try {
                auto& ffi_manager = rbasic::ffi::FFIManager::instance();
                auto library = ffi_manager.load_library(libName);
                if (library && library->is_valid()) {
                    lastValue = std::string("library_handle:" + libName);
                } else {
                    lastValue = std::string("error:Failed to load " + libName);
                }
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
                    auto& ffi_manager = rbasic::ffi::FFIManager::instance();
                    bool success = ffi_manager.unload_library(libName);
                    lastValue = success ? 1.0 : 0.0; // Return double for consistency
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
            // Load library if not already loaded
            auto& ffi_manager = rbasic::ffi::FFIManager::instance();
            auto library = ffi_manager.get_library(ffiFunc.library);
            if (!library) {
                library = ffi_manager.load_library(ffiFunc.library);
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
        bool returnsVoid = (ffiFunc.returnType == "void");
        
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
            
            // Pattern: (pointer, int)
            if ((param1Type == "pointer" || param1Type.find('*') != std::string::npos) && 
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
            // Four parameters - MessageBox patterns
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
            
            // Generic pattern: assume all int parameters for now
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
    // Create new scope for the for loop to isolate loop variables
    pushScope();
    
    // Execute initialization: var i = 1 or i = 1
    ValueType initValue = evaluate(*node.initialization);
    setVariable(node.variable, initValue);  // Use setVariable to handle both new and existing variables
    
    // Loop while condition is true
    while (isTruthy(evaluate(*node.condition))) {
        // Execute body
        for (auto& stmt : node.body) {
            stmt->accept(*this);
            if (hasReturned) {
                popScope();
                return;
            }
        }
        
        // Execute increment: i = i + 1
        // The increment expression should update the variable
        evaluate(*node.increment);
    }
    
    // Pop the for loop scope
    popScope();
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

} // namespace rbasic