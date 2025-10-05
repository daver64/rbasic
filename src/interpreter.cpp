#include "interpreter.h"
#include "runtime.h"
#include "io_handler.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

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
    
    throw RuntimeError("Undefined variable '" + name + "'");
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
    if (node.index) {
        ValueType arrayVar = getVariable(node.name);
        if (std::holds_alternative<ArrayValue>(arrayVar)) {
            ArrayValue& array = std::get<ArrayValue>(arrayVar);
            ValueType indexValue = evaluate(*node.index);
            int index = std::holds_alternative<int>(indexValue) ? 
                std::get<int>(indexValue) : static_cast<int>(std::get<double>(indexValue));
            
            if (array.elements.find(index) != array.elements.end()) {
                // Convert from simple variant to full ValueType
                auto& element = array.elements[index];
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
        int leftInt = std::holds_alternative<int>(left) ? 
            std::get<int>(left) : static_cast<int>(std::get<double>(left));
        int rightInt = std::holds_alternative<int>(right) ? 
            std::get<int>(right) : static_cast<int>(std::get<double>(right));
            
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
        lastValue = isTruthy(left) && isTruthy(right);
    } else if (node.operator_ == "or") {
        lastValue = isTruthy(left) || isTruthy(right);
    } else {
        throw RuntimeError("Unknown binary operator: " + node.operator_);
    }
}

void Interpreter::visit(AssignExpr& node) {
    ValueType value = evaluate(*node.value);
    
    // Handle array assignment
    if (node.index) {
        ValueType arrayVar = getVariable(node.variable);
        if (std::holds_alternative<ArrayValue>(arrayVar)) {
            ArrayValue array = std::get<ArrayValue>(arrayVar);
            ValueType indexValue = evaluate(*node.index);
            int index = std::holds_alternative<int>(indexValue) ? 
                std::get<int>(indexValue) : static_cast<int>(std::get<double>(indexValue));
            
            // Convert ValueType to simple variant for storage
            if (std::holds_alternative<int>(value)) {
                array.elements[index] = std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                array.elements[index] = std::get<double>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                array.elements[index] = std::get<std::string>(value);
            } else if (std::holds_alternative<bool>(value)) {
                array.elements[index] = std::get<bool>(value);
            }
            
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
    // Built-in I/O functions - check these first!
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
        return;
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
        return;
    }
    
    if (node.name == "input" && node.arguments.size() == 0) {
        std::string input_text = ioHandler->input();
        
        // Try to parse as number first
        ValueType value;
        try {
            if (input_text.find('.') != std::string::npos) {
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
        return;
    }

    // Graphics functions
    if (node.name == "graphics_mode" && node.arguments.size() == 2) {
        int width = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        int height = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
            std::get<int>(evaluate(*node.arguments[1])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
        ioHandler->graphics_mode(width, height);
        lastValue = 0;
        return;
    }
    
    if (node.name == "text_mode" && node.arguments.size() == 0) {
        ioHandler->text_mode();
        lastValue = 0;
        return;
    }
    
    if (node.name == "clear_screen" && node.arguments.size() == 0) {
        ioHandler->clear_screen();
        lastValue = 0;
        return;
    }
    
    if (node.name == "set_colour" && node.arguments.size() == 3) {
        int r = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        int g = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
            std::get<int>(evaluate(*node.arguments[1])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
        int b = std::holds_alternative<int>(evaluate(*node.arguments[2])) ? 
            std::get<int>(evaluate(*node.arguments[2])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[2])));
        ioHandler->set_colour(r, g, b);
        lastValue = 0;
        return;
    }
    
    if (node.name == "draw_pixel" && node.arguments.size() == 2) {
        int x = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        int y = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
            std::get<int>(evaluate(*node.arguments[1])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
        ioHandler->draw_pixel(x, y);
        lastValue = 0;
        return;
    }
    
    if (node.name == "draw_line" && node.arguments.size() == 4) {
        int x1 = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        int y1 = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
            std::get<int>(evaluate(*node.arguments[1])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
        int x2 = std::holds_alternative<int>(evaluate(*node.arguments[2])) ? 
            std::get<int>(evaluate(*node.arguments[2])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[2])));
        int y2 = std::holds_alternative<int>(evaluate(*node.arguments[3])) ? 
            std::get<int>(evaluate(*node.arguments[3])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[3])));
        ioHandler->draw_line(x1, y1, x2, y2);
        lastValue = 0;
        return;
    }
    
    if (node.name == "draw_rect" && (node.arguments.size() == 4 || node.arguments.size() == 5)) {
        int x = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        int y = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
            std::get<int>(evaluate(*node.arguments[1])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
        int width = std::holds_alternative<int>(evaluate(*node.arguments[2])) ? 
            std::get<int>(evaluate(*node.arguments[2])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[2])));
        int height = std::holds_alternative<int>(evaluate(*node.arguments[3])) ? 
            std::get<int>(evaluate(*node.arguments[3])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[3])));
        bool filled = false;
        if (node.arguments.size() == 5) {
            ValueType filledValue = evaluate(*node.arguments[4]);
            filled = std::holds_alternative<bool>(filledValue) ? std::get<bool>(filledValue) : 
                     (std::holds_alternative<int>(filledValue) ? std::get<int>(filledValue) != 0 : 
                      std::get<double>(filledValue) != 0.0);
        }
        ioHandler->draw_rect(x, y, width, height, filled);
        lastValue = 0;
        return;
    }
    
    if (node.name == "draw_text" && node.arguments.size() == 3) {
        int x = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        int y = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
            std::get<int>(evaluate(*node.arguments[1])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
        std::string text = valueToString(evaluate(*node.arguments[2]));
        ioHandler->draw_text(x, y, text);
        lastValue = 0;
        return;
    }
    
    if (node.name == "refresh_screen" && node.arguments.size() == 0) {
        ioHandler->refresh_screen();
        lastValue = 0;
        return;
    }
    
    if (node.name == "key_pressed" && node.arguments.size() == 1) {
        ValueType keyValue = evaluate(*node.arguments[0]);
        std::string key;
        
        // Handle numeric key codes (ASCII values)
        if (std::holds_alternative<int>(keyValue)) {
            int keyCode = std::get<int>(keyValue);
            if (keyCode >= 32 && keyCode <= 126) { // Printable ASCII range
                key = std::string(1, static_cast<char>(keyCode));
            } else {
                key = std::to_string(keyCode); // Non-printable, keep as string
            }
        } else {
            key = valueToString(keyValue);
        }
        
        bool pressed = ioHandler->key_pressed(key);
        lastValue = pressed;
        return;
    }
    
    if (node.name == "quit_requested" && node.arguments.size() == 0) {
        bool quit = ioHandler->quit_requested();
        lastValue = quit;
        return;
    }
    
    if (node.name == "sleep_ms" && node.arguments.size() == 1) {
        int ms = std::holds_alternative<int>(evaluate(*node.arguments[0])) ? 
            std::get<int>(evaluate(*node.arguments[0])) : 
            static_cast<int>(std::get<double>(evaluate(*node.arguments[0])));
        ioHandler->sleep_ms(ms);
        lastValue = 0;
        return;
    }
    
    if (node.name == "get_ticks" && node.arguments.size() == 0) {
        int ticks = ioHandler->get_ticks();
        lastValue = ticks;
        return;
    }

    // Built-in math functions
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
            return;
        } else if (node.name == "abs") {
            lastValue = std::abs(numArg);
            return;
        } else if (node.name == "sin") {
            lastValue = std::sin(numArg);
            return;
        } else if (node.name == "cos") {
            lastValue = std::cos(numArg);
            return;
        } else if (node.name == "tan") {
            lastValue = std::tan(numArg);
            return;
        } else if (node.name == "asin") {
            lastValue = std::asin(numArg);
            return;
        } else if (node.name == "acos") {
            lastValue = std::acos(numArg);
            return;
        } else if (node.name == "atan") {
            lastValue = std::atan(numArg);
            return;
        } else if (node.name == "log") {
            if (numArg <= 0) {
                throw RuntimeError("LOG requires a positive argument");
            }
            lastValue = std::log(numArg);
            return;
        } else if (node.name == "log10") {
            if (numArg <= 0) {
                throw RuntimeError("LOG10 requires a positive argument");
            }
            lastValue = std::log10(numArg);
            return;
        } else if (node.name == "exp") {
            lastValue = std::exp(numArg);
            return;
        } else if (node.name == "floor") {
            lastValue = std::floor(numArg);
            return;
        } else if (node.name == "ceil") {
            lastValue = std::ceil(numArg);
            return;
        } else if (node.name == "round") {
            lastValue = std::round(numArg);
            return;
        } else if (node.name == "int") {
            lastValue = static_cast<int>(numArg);
            return;
        }
    }
    
    // Two-argument functions and mid (which can have 2 or 3 arguments)
    if (node.arguments.size() == 2 || (node.name == "mid" && (node.arguments.size() == 2 || node.arguments.size() == 3))) {
        if (node.name == "pow") {
            ValueType base = evaluate(*node.arguments[0]);
            ValueType exp = evaluate(*node.arguments[1]);
            
            double baseNum = std::holds_alternative<int>(base) ? 
                static_cast<double>(std::get<int>(base)) : std::get<double>(base);
            double expNum = std::holds_alternative<int>(exp) ? 
                static_cast<double>(std::get<int>(exp)) : std::get<double>(exp);
                
            lastValue = std::pow(baseNum, expNum);
            return;
        } else if (node.name == "atan2") {
            ValueType y = evaluate(*node.arguments[0]);
            ValueType x = evaluate(*node.arguments[1]);
            
            double yNum = std::holds_alternative<int>(y) ? 
                static_cast<double>(std::get<int>(y)) : std::get<double>(y);
            double xNum = std::holds_alternative<int>(x) ? 
                static_cast<double>(std::get<int>(x)) : std::get<double>(x);
                
            lastValue = std::atan2(yNum, xNum);
            return;
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
            return;
        } else if (node.name == "min") {
            ValueType left = evaluate(*node.arguments[0]);
            ValueType right = evaluate(*node.arguments[1]);
            
            double leftNum = std::holds_alternative<int>(left) ? 
                static_cast<double>(std::get<int>(left)) : std::get<double>(left);
            double rightNum = std::holds_alternative<int>(right) ? 
                static_cast<double>(std::get<int>(right)) : std::get<double>(right);
                
            lastValue = std::min(leftNum, rightNum);
            return;
        } else if (node.name == "max") {
            ValueType left = evaluate(*node.arguments[0]);
            ValueType right = evaluate(*node.arguments[1]);
            
            double leftNum = std::holds_alternative<int>(left) ? 
                static_cast<double>(std::get<int>(left)) : std::get<double>(left);
            double rightNum = std::holds_alternative<int>(right) ? 
                static_cast<double>(std::get<int>(right)) : std::get<double>(right);
                
            lastValue = std::max(leftNum, rightNum);
            return;
        } else if (node.name == "mid") {
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
            return;
        } else if (node.name == "left") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            int length = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
                std::get<int>(evaluate(*node.arguments[1])) : 
                static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
                
            lastValue = str.substr(0, std::max(0, length));
            return;
        } else if (node.name == "right") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            int length = std::holds_alternative<int>(evaluate(*node.arguments[1])) ? 
                std::get<int>(evaluate(*node.arguments[1])) : 
                static_cast<int>(std::get<double>(evaluate(*node.arguments[1])));
                
            int start = std::max(0, static_cast<int>(str.length()) - length);
            lastValue = str.substr(start);
            return;
        }
    }
    
    // Single-argument functions
    if (node.arguments.size() == 1) {
        if (node.name == "len") {
            std::string str = valueToString(evaluate(*node.arguments[0]));
            lastValue = static_cast<int>(str.length());
            return;
        } else if (node.name == "str") {
            // Convert number to string
            ValueType value = evaluate(*node.arguments[0]);
            lastValue = valueToString(value);
            return;
        } else if (node.name == "val") {
            // Convert string to number
            std::string str = valueToString(evaluate(*node.arguments[0]));
            try {
                if (str.find('.') != std::string::npos) {
                    lastValue = std::stod(str);
                } else {
                    lastValue = std::stoi(str);
                }
            } catch (const std::exception&) {
                lastValue = 0; // Default to 0 if conversion fails
            }
            return;
        } else if (node.name == "rnd" || node.name == "random") {
            ValueType arg = evaluate(*node.arguments[0]);
            int maxVal = 1;
            if (std::holds_alternative<int>(arg)) {
                maxVal = std::get<int>(arg);
            } else if (std::holds_alternative<double>(arg)) {
                maxVal = static_cast<int>(std::get<double>(arg));
            }
            if (maxVal <= 0) maxVal = 1;
            lastValue = (std::rand() % maxVal) + 1;  // 1 to maxVal
            return;
        }
    }
    
    // Zero-argument functions
    if (node.arguments.size() == 0) {
        if (node.name == "rnd" || node.name == "random") {
            lastValue = static_cast<double>(std::rand()) / RAND_MAX;
            return;
        } else if (node.name == "randomise") {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            lastValue = 0; // randomise doesn't return a value
            return;
        } else if (node.name == "pi") {
            lastValue = 3.141592653589793;
            return;
        }
    }
    
    // Built-in functions
    if (node.name == "sqr" && node.arguments.size() == 1) {
        ValueType arg = evaluate(*node.arguments[0]);
        if (std::holds_alternative<int>(arg)) {
            lastValue = std::sqrt(static_cast<double>(std::get<int>(arg)));
        } else if (std::holds_alternative<double>(arg)) {
            lastValue = std::sqrt(std::get<double>(arg));
        } else {
            throw RuntimeError("SQR requires a numeric argument");
        }
        return;
    }
    
#ifdef RBASIC_SQLITE_SUPPORT
    // Database functions
    if (node.name == "db_open" && node.arguments.size() == 1) {
        std::string path = valueToString(evaluate(*node.arguments[0]));
        lastValue = Runtime::basicDbOpen({path});
        return;
    }
    
    if (node.name == "db_close" && node.arguments.size() == 0) {
        lastValue = Runtime::basicDbClose({});
        return;
    }
    
    if (node.name == "db_exec" && node.arguments.size() == 1) {
        std::string sql = valueToString(evaluate(*node.arguments[0]));
        lastValue = Runtime::basicDbExec({sql});
        return;
    }
    
    if (node.name == "db_query" && node.arguments.size() == 1) {
        std::string sql = valueToString(evaluate(*node.arguments[0]));
        lastValue = Runtime::basicDbQuery({sql});
        return;
    }
    
    if (node.name == "db_error" && node.arguments.size() == 0) {
        lastValue = Runtime::basicDbError({});
        return;
    }
    
    if (node.name == "db_escape" && node.arguments.size() == 1) {
        std::string str = valueToString(evaluate(*node.arguments[0]));
        lastValue = Runtime::basicDbEscape({str});
        return;
    }
#endif
    
    // User-defined functions
    if (functions.find(node.name) != functions.end()) {
        auto& func = functions[node.name];
        
        if (node.arguments.size() != func->parameters.size()) {
            throw RuntimeError("Function " + node.name + " expects " + 
                             std::to_string(func->parameters.size()) + " arguments, got " + 
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
        for (size_t i = 0; i < func->parameters.size(); i++) {
            defineVariable(func->parameters[i], argValues[i]);
        }
        
        // Execute function body
        hasReturned = false;
        for (auto& stmt : func->body) {
            stmt->accept(*this);
            if (hasReturned) break;
        }
        
        popScope();
        hasReturned = false;
    } else {
        throw RuntimeError("Unknown function: " + node.name);
    }
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
    else if (node.index) {
        ValueType arrayVar = getVariable(node.variable);
        if (std::holds_alternative<ArrayValue>(arrayVar)) {
            ArrayValue array = std::get<ArrayValue>(arrayVar);
            ValueType indexValue = evaluate(*node.index);
            int index = std::holds_alternative<int>(indexValue) ? 
                std::get<int>(indexValue) : static_cast<int>(std::get<double>(indexValue));
            
            // Convert ValueType to simple variant for storage
            if (std::holds_alternative<int>(value)) {
                array.elements[index] = std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                array.elements[index] = std::get<double>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                array.elements[index] = std::get<std::string>(value);
            } else if (std::holds_alternative<bool>(value)) {
                array.elements[index] = std::get<bool>(value);
            }
            
            setVariable(node.variable, array);  // Use setVariable instead of defineVariable
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
        if (input_text.find('.') != std::string::npos) {
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
        // Execute then branch without creating new scope for now
        for (auto& stmt : node.thenBranch) {
            stmt->accept(*this);
            if (hasReturned) {
                return;
            }
        }
    } else {
        // Execute else branch without creating new scope for now
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
            if (hasReturned) {
                return;
            }
        }
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
        // Array declaration - create an empty array
        ArrayValue array;
        defineVariable(node.variable, array);
    }
}

void Interpreter::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (hasReturned) break;
    }
}

} // namespace rbasic