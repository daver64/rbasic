#include "codegen.h"

namespace rbasic {

CodeGenerator::CodeGenerator() : indentLevel(0), tempVarCounter(0) {}

void CodeGenerator::indent() {
    for (int i = 0; i < indentLevel; i++) {
        output << "    ";
    }
}

void CodeGenerator::writeLine(const std::string& line) {
    indent();
    output << line << std::endl;
}

void CodeGenerator::write(const std::string& text) {
    output << text;
}

std::string CodeGenerator::generateVariableName(const std::string& basicName) {
    return "var_" + basicName;
}

std::string CodeGenerator::generateTempVar() {
    return "temp_" + std::to_string(tempVarCounter++);
}

std::string CodeGenerator::generate(Program& program) {
    output.str("");
    output.clear();
    tempVarCounter = 0;
    indentLevel = 0;
    
    generateIncludes();
    generateMain();
    
    indentLevel = 1;
    program.accept(*this);
    indentLevel = 0;
    
    writeLine("}");
    
    return output.str();
}

void CodeGenerator::generateIncludes() {
    writeLine("#include \"runtime/basic_runtime.h\"");
    writeLine("#include <iostream>");
    writeLine("#include <map>");
    writeLine("#include <string>");
    writeLine("");
    writeLine("using namespace basic_runtime;");
    writeLine("");
}

void CodeGenerator::generateMain() {
    writeLine("int main() {");
    writeLine("    init_runtime();");
    writeLine("    std::map<std::string, BasicValue> variables;");
    writeLine("    ");
    writeLine("    // Initialize boolean constants");
    writeLine("    variables[\"true\"] = BasicValue(true);");
    writeLine("    variables[\"false\"] = BasicValue(false);");
    writeLine("");
}

// Visitor implementations
void CodeGenerator::visit(LiteralExpr& node) {
    if (std::holds_alternative<int>(node.value)) {
        write(std::to_string(std::get<int>(node.value)));
    } else if (std::holds_alternative<double>(node.value)) {
        write(std::to_string(std::get<double>(node.value)));
    } else if (std::holds_alternative<std::string>(node.value)) {
        write("\"" + std::get<std::string>(node.value) + "\"");
    } else if (std::holds_alternative<bool>(node.value)) {
        write(std::get<bool>(node.value) ? "true" : "false");
    }
}

void CodeGenerator::visit(VariableExpr& node) {
    write("variables[\"" + node.name + "\"]");
}

void CodeGenerator::visit(BinaryExpr& node) {
    if (node.operator_ == "+") {
        write("add(");
        node.left->accept(*this);
        write(", ");
        node.right->accept(*this);
        write(")");
    } else if (node.operator_ == "-") {
        write("subtract(");
        node.left->accept(*this);
        write(", ");
        node.right->accept(*this);
        write(")");
    } else if (node.operator_ == "*") {
        write("multiply(");
        node.left->accept(*this);
        write(", ");
        node.right->accept(*this);
        write(")");
    } else if (node.operator_ == "/") {
        write("divide(");
        node.left->accept(*this);
        write(", ");
        node.right->accept(*this);
        write(")");
    } else {
        // For comparison operators, generate boolean result
        write("(");
        if (node.operator_ == "==" || node.operator_ == "=") {
            write("equal(");
        } else if (node.operator_ == "<>" || node.operator_ == "!=") {
            write("not_equal(");
        } else if (node.operator_ == "<") {
            write("less_than(");
        } else if (node.operator_ == "<=") {
            write("less_equal(");
        } else if (node.operator_ == ">") {
            write("greater_than(");
        } else if (node.operator_ == ">=") {
            write("greater_equal(");
        }
        
        node.left->accept(*this);
        write(", ");
        node.right->accept(*this);
        write(") ? BasicValue(true) : BasicValue(false))");
    }
}

void CodeGenerator::visit(AssignExpr& node) {
    write("(variables[\"" + node.variable + "\"] = ");
    node.value->accept(*this);
    write(")");
}

void CodeGenerator::visit(UnaryExpr& node) {
    if (node.operator_ == "-") {
        write("subtract(BasicValue(0), ");
        node.operand->accept(*this);
        write(")");
    } else if (node.operator_ == "not") {
        write("(!to_bool(");
        node.operand->accept(*this);
        write(") ? BasicValue(true) : BasicValue(false))");
    }
}

void CodeGenerator::visit(CallExpr& node) {
    // Single-argument math functions
    if (node.arguments.size() == 1) {
        if (node.name == "sqr" || node.name == "sqrt") {
            write("sqrt_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "abs") {
            write("abs_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "sin") {
            write("sin_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "cos") {
            write("cos_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "tan") {
            write("tan_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "asin") {
            write("asin_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "acos") {
            write("acos_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "atan") {
            write("atan_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "log") {
            write("log_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "log10") {
            write("log10_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "exp") {
            write("exp_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "floor") {
            write("floor_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "ceil") {
            write("ceil_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "round") {
            write("round_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        } else if (node.name == "int") {
            write("int_val(");
            node.arguments[0]->accept(*this);
            write(")");
            return;
        }
    }
    
    // Two-argument functions
    if (node.arguments.size() == 2) {
        if (node.name == "pow") {
            write("pow_val(");
            node.arguments[0]->accept(*this);
            write(", ");
            node.arguments[1]->accept(*this);
            write(")");
            return;
        } else if (node.name == "atan2") {
            write("atan2_val(");
            node.arguments[0]->accept(*this);
            write(", ");
            node.arguments[1]->accept(*this);
            write(")");
            return;
        } else if (node.name == "mod") {
            write("mod_val(");
            node.arguments[0]->accept(*this);
            write(", ");
            node.arguments[1]->accept(*this);
            write(")");
            return;
        }
    }
    
    // Zero-argument functions
    if (node.arguments.size() == 0) {
        if (node.name == "rnd" || node.name == "random") {
            write("rnd()");
            return;
        } else if (node.name == "pi") {
            write("pi_val()");
            return;
        }
    }
    
    // Built-in I/O functions
    if (node.name == "print") {
        write("([&](){");
        for (size_t i = 0; i < node.arguments.size(); i++) {
            write("basic_runtime::print(");
            node.arguments[i]->accept(*this);
            write(");");
            if (i < node.arguments.size() - 1) {
                write("basic_runtime::print(BasicValue(\" \"));");
            }
        }
        write("basic_runtime::print_line();return BasicValue(0);})()");
        return;
    }
    
    if (node.name == "input" && node.arguments.size() == 0) {
        write("basic_runtime::input()");
        return;
    }
    
    // User-defined function calls would go here
    write("/* Function call: " + node.name + " */");
}

void CodeGenerator::visit(ExpressionStmt& node) {
    indent();
    node.expression->accept(*this);
    write(";\n");
}

void CodeGenerator::visit(VarStmt& node) {
    indent();
    write("variables[\"" + node.variable + "\"] = ");
    node.value->accept(*this);
    write(";\n");
}

void CodeGenerator::visit(PrintStmt& node) {
    for (size_t i = 0; i < node.expressions.size(); i++) {
        indent();
        write("print(");
        node.expressions[i]->accept(*this);
        write(");\n");
        if (i < node.expressions.size() - 1) {
            indent();
            write("print(BasicValue(\" \"));\n");
        }
    }
    indent();
    write("print_line();\n");
}

void CodeGenerator::visit(InputStmt& node) {
    indent();
    write("variables[\"" + node.variable + "\"] = input();\n");
}

void CodeGenerator::visit(IfStmt& node) {
    indent();
    write("if (to_bool(");
    node.condition->accept(*this);
    write(")) {\n");
    
    indentLevel++;
    for (auto& stmt : node.thenBranch) {
        stmt->accept(*this);
    }
    indentLevel--;
    
    if (!node.elseBranch.empty()) {
        writeLine("} else {");
        indentLevel++;
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
        }
        indentLevel--;
    }
    
    writeLine("}");
}

void CodeGenerator::visit(ForStmt& node) {
    std::string loopVar = generateTempVar();
    
    indent();
    write("for (int " + loopVar + " = to_int(");
    node.start->accept(*this);
    write("); " + loopVar + " <= to_int(");
    node.end->accept(*this);
    write("); " + loopVar + " += to_int(");
    node.step->accept(*this);
    write(")) {\n");
    
    indentLevel++;
    indent();
    write("variables[\"" + node.variable + "\"] = BasicValue(" + loopVar + ");\n");
    
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    
    writeLine("}");
}

void CodeGenerator::visit(ModernForStmt& node) {
    indent();
    write("// Modern for loop: ");
    write(node.variable);
    write("\n");
    
    // Generate initialization
    indent();
    write("variables[\"" + node.variable + "\"] = ");
    node.initialization->accept(*this);
    write(";\n");
    
    // Generate while loop with condition
    indent();
    write("while (to_bool(");
    node.condition->accept(*this);
    write(")) {\n");
    
    indentLevel++;
    
    // Generate body
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    
    // Generate increment
    indent();
    node.increment->accept(*this);
    write(";\n");
    
    indentLevel--;
    indent();
    write("}\n");
}

void CodeGenerator::visit(WhileStmt& node) {
    indent();
    write("while (to_bool(");
    node.condition->accept(*this);
    write(")) {\n");
    
    indentLevel++;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    
    writeLine("}");
}

void CodeGenerator::visit(ReturnStmt& node) {
    indent();
    if (node.value) {
        write("return to_int(");
        node.value->accept(*this);
        write(");\n");
    } else {
        write("return 0;\n");
    }
}

void CodeGenerator::visit(FunctionDecl& node) {
    // TODO: Implement function generation
    writeLine("/* Function: " + node.name + " */");
}

void CodeGenerator::visit(StructDecl& node) {
    // TODO: Implement struct generation
    writeLine("/* Struct: " + node.name + " */");
}

void CodeGenerator::visit(DimStmt& node) {
    indent();
    if (node.type == "integer") {
        write("variables[\"" + node.variable + "\"] = BasicValue(0);\n");
    } else if (node.type == "double") {
        write("variables[\"" + node.variable + "\"] = BasicValue(0.0);\n");
    } else if (node.type == "string") {
        write("variables[\"" + node.variable + "\"] = BasicValue(std::string(\"\"));\n");
    } else if (node.type == "boolean") {
        write("variables[\"" + node.variable + "\"] = BasicValue(false);\n");
    } else {
        write("variables[\"" + node.variable + "\"] = BasicValue(0); // " + node.type + "\n");
    }
}

void CodeGenerator::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    
    writeLine("");
    writeLine("return 0;");
}

} // namespace rbasic