#include "interpreter.h"
#include "runtime.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace rbasic {

Interpreter::Interpreter() : hasReturned(false) {
    // Initialize boolean constants
    globals["true"] = true;
    globals["false"] = false;
}

void Interpreter::defineVariable(const std::string& name, const ValueType& value) {
    if (!scopes.empty()) {
        scopes.top()[name] = value;
    } else {
        globals[name] = value;
    }
}

ValueType Interpreter::getVariable(const std::string& name) {
    if (!scopes.empty()) {
        auto& scope = scopes.top();
        if (scope.find(name) != scope.end()) {
            return scope[name];
        }
    }
    
    if (globals.find(name) != globals.end()) {
        return globals[name];
    }
    
    throw RuntimeError("Undefined variable '" + name + "'");
}

void Interpreter::setVariable(const std::string& name, const ValueType& value) {
    if (!scopes.empty()) {
        auto& scope = scopes.top();
        if (scope.find(name) != scope.end()) {
            scope[name] = value;
            return;
        }
    }
    
    if (globals.find(name) != globals.end()) {
        globals[name] = value;
    } else {
        defineVariable(name, value);
    }
}

void Interpreter::pushScope() {
    scopes.push(std::map<std::string, ValueType>());
}

void Interpreter::popScope() {
    if (!scopes.empty()) {
        scopes.pop();
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

// Visitor implementations
void Interpreter::visit(LiteralExpr& node) {
    lastValue = node.value;
}

void Interpreter::visit(VariableExpr& node) {
    ValueType value = getVariable(node.name);
    
    // Handle array access
    if (node.index) {
        // TODO: Implement array access
        throw RuntimeError("Array access not implemented yet");
    }
    
    // Handle struct member access
    if (!node.member.empty()) {
        // TODO: Implement struct member access
        throw RuntimeError("Struct member access not implemented yet");
    }
    
    lastValue = value;
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
        // TODO: Implement modulo
        lastValue = 0;
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
    setVariable(node.variable, value);
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
            std::cout << valueToString(value);
            if (i < node.arguments.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
        lastValue = 0; // print returns 0
        return;
    }
    
    if (node.name == "input" && node.arguments.size() == 0) {
        std::string input;
        std::getline(std::cin, input);
        
        // Try to parse as number first
        ValueType value;
        try {
            if (input.find('.') != std::string::npos) {
                value = std::stod(input);
            } else {
                value = std::stoi(input);
            }
        } catch (...) {
            // If not a number, store as string
            value = input;
        }
        
        lastValue = value;
        return;
    }

    // Built-in math functions
    if (node.arguments.size() == 1) {
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
    
    // Two-argument functions
    if (node.arguments.size() == 2) {
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
        }
    }
    
    // Zero-argument functions
    if (node.arguments.size() == 0) {
        if (node.name == "rnd" || node.name == "random") {
            lastValue = static_cast<double>(std::rand()) / RAND_MAX;
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

void Interpreter::visit(ExpressionStmt& node) {
    evaluate(*node.expression);
}

void Interpreter::visit(VarStmt& node) {
    ValueType value = evaluate(*node.value);
    defineVariable(node.variable, value);
}

void Interpreter::visit(PrintStmt& node) {
    for (size_t i = 0; i < node.expressions.size(); i++) {
        ValueType value = evaluate(*node.expressions[i]);
        std::cout << valueToString(value);
        if (i < node.expressions.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void Interpreter::visit(InputStmt& node) {
    std::string input;
    std::getline(std::cin, input);
    
    // Try to parse as number first
    ValueType value;
    try {
        if (input.find('.') != std::string::npos) {
            // Contains decimal point, parse as double
            value = std::stod(input);
        } else {
            // Try to parse as integer
            value = std::stoi(input);
        }
    } catch (const std::exception&) {
        // If parsing fails, store as string
        value = input;
    }
    
    setVariable(node.variable, value);
}

void Interpreter::visit(IfStmt& node) {
    ValueType condition = evaluate(*node.condition);
    
    if (isTruthy(condition)) {
        for (auto& stmt : node.thenBranch) {
            stmt->accept(*this);
            if (hasReturned) break;
        }
    } else {
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
            if (hasReturned) break;
        }
    }
}

void Interpreter::visit(ForStmt& node) {
    ValueType startVal = evaluate(*node.start);
    ValueType endVal = evaluate(*node.end);
    ValueType stepVal = evaluate(*node.step);
    
    int start = std::holds_alternative<int>(startVal) ? 
        std::get<int>(startVal) : static_cast<int>(std::get<double>(startVal));
    int end = std::holds_alternative<int>(endVal) ? 
        std::get<int>(endVal) : static_cast<int>(std::get<double>(endVal));
    int step = std::holds_alternative<int>(stepVal) ? 
        std::get<int>(stepVal) : static_cast<int>(std::get<double>(stepVal));
    
    for (int i = start; i <= end; i += step) {
        defineVariable(node.variable, i);
        
        for (auto& stmt : node.body) {
            stmt->accept(*this);
            if (hasReturned) return;
        }
    }
}

void Interpreter::visit(ModernForStmt& node) {
    // Execute initialization: var i = 1
    ValueType initValue = evaluate(*node.initialization);
    defineVariable(node.variable, initValue);
    
    // Loop while condition is true
    while (isTruthy(evaluate(*node.condition))) {
        // Execute body
        for (auto& stmt : node.body) {
            stmt->accept(*this);
            if (hasReturned) return;
        }
        
        // Execute increment: i = i + 1
        // The increment expression should update the variable
        evaluate(*node.increment);
    }
}

void Interpreter::visit(WhileStmt& node) {
    while (isTruthy(evaluate(*node.condition))) {
        for (auto& stmt : node.body) {
            stmt->accept(*this);
            if (hasReturned) return;
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
    functions[node.name] = std::unique_ptr<FunctionDecl>(
        new FunctionDecl(node.name, node.parameters, node.paramTypes, node.returnType, {}));
    
    // Move the body statements
    for (auto& stmt : node.body) {
        functions[node.name]->body.push_back(std::move(stmt));
    }
}

void Interpreter::visit(StructDecl& node) {
    structs[node.name] = std::unique_ptr<StructDecl>(
        new StructDecl(node.name, node.fields, node.fieldTypes));
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
            // Struct or unknown type - initialize to 0 for now
            defineVariable(node.variable, 0);
        }
    } else {
        // Array declaration - TODO: implement arrays
        defineVariable(node.variable, 0);
    }
}

void Interpreter::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (hasReturned) break;
    }
}

} // namespace rbasic