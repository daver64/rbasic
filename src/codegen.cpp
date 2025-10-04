#include "codegen.h"

namespace rbasic {

CodeGenerator::CodeGenerator() : indentLevel(0), tempVarCounter(0), usesSDL(false), usesSQLite(false) {}

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
    functionDeclarations = "";
    usesSDL = false;
    usesSQLite = false;
    tempVarCounter = 0;
    indentLevel = 0;
    
    // First pass: collect function declarations and detect SDL usage
    program.accept(*this);
    
    // Clear output after first pass - we only wanted to collect functions and SDL usage
    output.str("");
    output.clear();
    
    generateIncludes();
    
    // Output function declarations
    output << functionDeclarations;
    
    generateMain();
    
    indentLevel = 1;
    // Second pass: generate main program
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
    if (usesSDL) {
        writeLine("    init_runtime_sdl(); // SDL support detected");
    } else {
        writeLine("    init_runtime();");
    }
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
    if (node.index) {
        // Array access: array[index]
        write("get_array_element(variables[\"" + node.name + "\"], ");
        node.index->accept(*this);
        write(")");
    } else {
        // Regular variable access
        write("variables[\"" + node.name + "\"]");
    }
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
    } else if (node.operator_ == "mod") {
        write("mod_val(");
        node.left->accept(*this);
        write(", ");
        node.right->accept(*this);
        write(")");
    } else if (node.operator_ == "and") {
        write("(to_bool(");
        node.left->accept(*this);
        write(") && to_bool(");
        node.right->accept(*this);
        write("))");
    } else if (node.operator_ == "or") {
        write("(to_bool(");
        node.left->accept(*this);
        write(") || to_bool(");
        node.right->accept(*this);
        write("))");
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
        if (node.arguments.size() == 0) {
            // Handle empty print - just print a newline
            write("basic_runtime::print_line();return BasicValue(0);})()");
        } else {
            for (size_t i = 0; i < node.arguments.size(); i++) {
                write("basic_runtime::print(");
                node.arguments[i]->accept(*this);
                write(");");
                if (i < node.arguments.size() - 1) {
                    write("basic_runtime::print(BasicValue(\" \"));");
                }
            }
            write("basic_runtime::print_line();return BasicValue(0);})()");
        }
        return;
    }
    
    if (node.name == "input" && node.arguments.size() == 0) {
        write("basic_runtime::input()");
        return;
    }
    
    // Graphics functions (SDL)
    if (node.name == "graphics_mode" && node.arguments.size() == 2) {
        usesSDL = true;
        write("graphics_mode(to_int(");
        node.arguments[0]->accept(*this);
        write("), to_int(");
        node.arguments[1]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "text_mode" && node.arguments.size() == 0) {
        usesSDL = true;
        write("text_mode()");
        return;
    }
    
    if (node.name == "clear_screen" && node.arguments.size() == 0) {
        usesSDL = true;
        write("clear_screen()");
        return;
    }
    
    if (node.name == "set_color" && node.arguments.size() == 3) {
        usesSDL = true;
        write("set_color(to_int(");
        node.arguments[0]->accept(*this);
        write("), to_int(");
        node.arguments[1]->accept(*this);
        write("), to_int(");
        node.arguments[2]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "draw_pixel" && node.arguments.size() == 2) {
        usesSDL = true;
        write("draw_pixel(to_int(");
        node.arguments[0]->accept(*this);
        write("), to_int(");
        node.arguments[1]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "draw_line" && node.arguments.size() == 4) {
        usesSDL = true;
        write("draw_line(to_int(");
        node.arguments[0]->accept(*this);
        write("), to_int(");
        node.arguments[1]->accept(*this);
        write("), to_int(");
        node.arguments[2]->accept(*this);
        write("), to_int(");
        node.arguments[3]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "draw_rect" && (node.arguments.size() == 4 || node.arguments.size() == 5)) {
        usesSDL = true;
        write("draw_rect(to_int(");
        node.arguments[0]->accept(*this);
        write("), to_int(");
        node.arguments[1]->accept(*this);
        write("), to_int(");
        node.arguments[2]->accept(*this);
        write("), to_int(");
        node.arguments[3]->accept(*this);
        if (node.arguments.size() == 5) {
            write("), to_bool(");
            node.arguments[4]->accept(*this);
            write("))");
        } else {
            write("), false)");
        }
        return;
    }
    
    if (node.name == "refresh_screen" && node.arguments.size() == 0) {
        usesSDL = true;
        write("refresh_screen()");
        return;
    }
    
    if (node.name == "key_pressed" && node.arguments.size() == 1) {
        usesSDL = true;
        write("key_pressed(");
        node.arguments[0]->accept(*this);
        write(")");
        return;
    }
    
    if (node.name == "quit_requested" && node.arguments.size() == 0) {
        usesSDL = true;
        write("quit_requested()");
        return;
    }
    
    if (node.name == "sleep_ms" && node.arguments.size() == 1) {
        usesSDL = true;
        write("sleep_ms(to_int(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "get_ticks" && node.arguments.size() == 0) {
        usesSDL = true;
        write("get_ticks()");
        return;
    }
    
    // Database functions (SQLite)
    if (node.name == "db_open" && node.arguments.size() == 1) {
        usesSQLite = true;
        write("db_open(to_string(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "db_close" && node.arguments.size() == 0) {
        usesSQLite = true;
        write("db_close()");
        return;
    }
    
    if (node.name == "db_exec" && node.arguments.size() == 1) {
        usesSQLite = true;
        write("db_exec(to_string(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "db_query" && node.arguments.size() == 1) {
        usesSQLite = true;
        write("db_query(to_string(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }
    
    if (node.name == "db_error" && node.arguments.size() == 0) {
        usesSQLite = true;
        write("db_error()");
        return;
    }
    
    if (node.name == "db_escape" && node.arguments.size() == 1) {
        usesSQLite = true;
        write("db_escape(to_string(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }
    
    // User-defined function calls
    write("func_" + node.name + "(");
    for (size_t i = 0; i < node.arguments.size(); i++) {
        node.arguments[i]->accept(*this);
        if (i < node.arguments.size() - 1) {
            write(", ");
        }
    }
    write(")");
}

void CodeGenerator::visit(ExpressionStmt& node) {
    indent();
    node.expression->accept(*this);
    write(";\n");
}

void CodeGenerator::visit(VarStmt& node) {
    indent();
    if (node.index) {
        // Array assignment: array[index] = value
        write("set_array_element(variables[\"" + node.variable + "\"], ");
        node.index->accept(*this);
        write(", ");
        node.value->accept(*this);
        write(");\n");
    } else {
        // Regular variable assignment
        write("variables[\"" + node.variable + "\"] = ");
        node.value->accept(*this);
        write(";\n");
    }
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
    // Generate function declaration outside of main
    // We need to move this to the beginning of the file
    functionDeclarations += "BasicValue func_" + node.name + "(";
    
    // Parameters
    for (size_t i = 0; i < node.parameters.size(); i++) {
        functionDeclarations += "BasicValue " + node.parameters[i];
        if (i < node.parameters.size() - 1) {
            functionDeclarations += ", ";
        }
    }
    functionDeclarations += ") {\n";
    
    // Create local variable scope for function parameters
    functionDeclarations += "    std::map<std::string, BasicValue> function_vars;\n";
    for (const auto& param : node.parameters) {
        functionDeclarations += "    function_vars[\"" + param + "\"] = " + param + ";\n";
    }
    
    // Save current output to preserve main function generation
    std::stringstream savedOutput;
    savedOutput << output.str();
    output.str("");
    output.clear();
    
    // Generate function body
    int savedIndent = indentLevel;
    indentLevel = 1;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel = savedIndent;
    
    // Add function body to function declarations
    std::string functionBody = output.str();
    // Replace variable access with function_vars, but handle boolean constants specially
    size_t pos = 0;
    while ((pos = functionBody.find("variables[", pos)) != std::string::npos) {
        size_t endPos = functionBody.find("]", pos);
        if (endPos != std::string::npos) {
            std::string varContent = functionBody.substr(pos + 10, endPos - pos - 10);
            if (varContent == "\"true\"" || varContent == "\"false\"") {
                // Replace boolean constants with direct BasicValue calls
                std::string boolValue = (varContent == "\"true\"") ? "BasicValue(true)" : "BasicValue(false)";
                functionBody.replace(pos, endPos - pos + 1, boolValue);
                pos += boolValue.length();
            } else {
                // Replace regular variables with function_vars
                functionBody.replace(pos, 10, "function_vars[");
                pos += 14;
            }
        } else {
            break;
        }
    }
    
    functionDeclarations += functionBody;
    
    // Ensure function has a return statement if none provided
    if (functionBody.find("return ") == std::string::npos) {
        functionDeclarations += "    return BasicValue(0);\n";
    }
    
    functionDeclarations += "}\n\n";
    
    // Restore output
    output.str("");
    output.clear();
    output << savedOutput.str();
}

void CodeGenerator::visit(StructDecl& node) {
    // TODO: Implement struct generation
    writeLine("/* Struct: " + node.name + " */");
}

void CodeGenerator::visit(DimStmt& node) {
    indent();
    if (!node.dimensions.empty()) {
        // Array declaration
        write("variables[\"" + node.variable + "\"] = BasicArray({");
        for (size_t i = 0; i < node.dimensions.size(); i++) {
            node.dimensions[i]->accept(*this);
            if (i < node.dimensions.size() - 1) {
                write(", ");
            }
        }
        write("});\n");
    } else {
        // Regular variable declaration
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
}

void CodeGenerator::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    
    writeLine("");
    writeLine("return 0;");
}

} // namespace rbasic