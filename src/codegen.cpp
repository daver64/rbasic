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

std::string CodeGenerator::escapeString(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string CodeGenerator::generate(Program& program) {
    output.str("");
    output.clear();
    functionDeclarations = "";
    tempVarCounter = 0;
    indentLevel = 0;
    
    // First pass: collect function declarations
    program.accept(*this);
    
    // Clear output after first pass - we only wanted to collect functions
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
        write("BasicValue(" + std::to_string(std::get<int>(node.value)) + ")");
    } else if (std::holds_alternative<double>(node.value)) {
        write("BasicValue(" + std::to_string(std::get<double>(node.value)) + ")");
    } else if (std::holds_alternative<std::string>(node.value)) {
        write("BasicValue(\"" + escapeString(std::get<std::string>(node.value)) + "\")");
    } else if (std::holds_alternative<bool>(node.value)) {
        write("BasicValue(" + std::string(std::get<bool>(node.value) ? "true" : "false") + ")");
    }
}

void CodeGenerator::visit(VariableExpr& node) {
    if (!node.indices.empty()) {
        // Array access: array[index1, index2, ...]
        write("get_array_element(variables[\"" + node.name + "\"], std::vector<BasicValue>{");
        for (size_t i = 0; i < node.indices.size(); ++i) {
            if (i > 0) write(", ");
            node.indices[i]->accept(*this);
        }
        write("})");
    } else if (!node.member.empty()) {
        // Struct member access: struct.member
        write("get_struct_field(std::get<BasicStruct>(variables[\"" + node.name + "\"]), \"" + node.member + "\")");
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
        write(")");
    }
}

void CodeGenerator::visit(AssignExpr& node) {
    if (!node.indices.empty()) {
        // Array assignment: arr[index1, index2, ...] = value
        write("set_array_element(variables[\"" + node.variable + "\"], std::vector<BasicValue>{");
        for (size_t i = 0; i < node.indices.size(); ++i) {
            if (i > 0) write(", ");
            node.indices[i]->accept(*this);
        }
        write("}, ");
        node.value->accept(*this);
        write(")");
    } else {
        // Simple variable assignment: var = value
        write("(variables[\"" + node.variable + "\"] = ");
        node.value->accept(*this);
        write(")");
    }
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
        if (node.arguments.size() == 0) {
            // Handle empty print - just print a newline
            write("basic_runtime::print_line()");
        } else {
            for (size_t i = 0; i < node.arguments.size(); i++) {
                write("basic_runtime::print(");
                node.arguments[i]->accept(*this);
                write(")");
                if (i < node.arguments.size() - 1) {
                    write("; basic_runtime::print(BasicValue(\" \")); ");
                }
            }
            write("; basic_runtime::print_line()");
        }
        return;
    }
    
    if (node.name == "debug_print") {
        if (node.arguments.size() == 0) {
            // Handle empty debug_print - just print a newline
            write("basic_runtime::debug_print(BasicValue(\"\"))");
        } else {
            for (size_t i = 0; i < node.arguments.size(); i++) {
                write("basic_runtime::debug_print(");
                node.arguments[i]->accept(*this);
                write(")");
                if (i < node.arguments.size() - 1) {
                    write("; ");
                }
            }
        }
        return;
    }
    
    if (node.name == "input" && node.arguments.size() == 0) {
        write("basic_runtime::input()");
        return;
    }
    
    // Note: External functions (graphics, database, etc.) will be handled via FFI
    
    // String conversion functions
    if (node.name == "str" && node.arguments.size() == 1) {
        write("to_string(");
        node.arguments[0]->accept(*this);
        write(")");
        return;
    }

    if (node.name == "val" && node.arguments.size() == 1) {
        write("val(");
        node.arguments[0]->accept(*this);
        write(")");
        return;
    }

    // String functions
    if (node.name == "len" && node.arguments.size() == 1) {
        write("len(");
        node.arguments[0]->accept(*this);
        write(")");
        return;
    }

    if (node.name == "mid" && (node.arguments.size() == 2 || node.arguments.size() == 3)) {
        write("mid(");
        node.arguments[0]->accept(*this);
        write(", to_int(");
        node.arguments[1]->accept(*this);
        if (node.arguments.size() == 3) {
            write("), to_int(");
            node.arguments[2]->accept(*this);
            write("))");
        } else {
            write("), -1)");
        }
        return;
    }

    if (node.name == "left" && node.arguments.size() == 2) {
        write("left(");
        node.arguments[0]->accept(*this);
        write(", to_int(");
        node.arguments[1]->accept(*this);
        write("))");
        return;
    }

    if (node.name == "right" && node.arguments.size() == 2) {
        write("right(");
        node.arguments[0]->accept(*this);
        write(", to_int(");
        node.arguments[1]->accept(*this);
        write("))");
        return;
    }

    // FFI functions
    if (node.name == "load_library" && node.arguments.size() == 1) {
        write("basic_runtime::load_library(to_string(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }

    if (node.name == "unload_library" && node.arguments.size() == 1) {
        write("basic_runtime::unload_library(");
        node.arguments[0]->accept(*this);
        write(")");
        return;
    }

    if (node.name == "is_library_loaded" && node.arguments.size() == 1) {
        write("BasicValue(basic_runtime::is_library_loaded(");
        node.arguments[0]->accept(*this);
        write("))");
        return;
    }

    // Check if this is an FFI function call
    auto ffiIt = ffiFunctions.find(node.name);
    if (ffiIt != ffiFunctions.end()) {
        const auto& ffiFunc = *ffiIt->second;
        
        // Generate FFI function call
        write("basic_runtime::call_ffi_function(\"" + ffiFunc.library + "\", \"" + ffiFunc.name + "\"");
        
        // Add arguments
        for (size_t i = 0; i < node.arguments.size(); i++) {
            write(", ");
            node.arguments[i]->accept(*this);
        }
        
        write(")");
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

void CodeGenerator::visit(StructLiteralExpr& node) {
    // Generate struct construction code with field initialization
    std::string tempVar = generateTempVar();
    
    // Create a struct constructor call
    write("([&]() { BasicStruct " + tempVar + " = create_struct(\"" + node.structName + "\"); ");
    
    // Get the struct declaration to access field names
    auto structIt = structs.find(node.structName);
    if (structIt != structs.end() && structIt->second) {
        const auto& structDecl = *structIt->second;
        
        // Initialize fields with provided values using actual field names
        for (size_t i = 0; i < node.values.size() && i < structDecl.fields.size(); i++) {
            write("set_struct_field(" + tempVar + ", \"" + structDecl.fields[i] + "\", ");
            node.values[i]->accept(*this);
            write("); ");
        }
    } else {
        // Fallback to generic field names if struct not found
        for (size_t i = 0; i < node.values.size(); i++) {
            write("set_struct_field(" + tempVar + ", \"field" + std::to_string(i) + "\", ");
            node.values[i]->accept(*this);
            write("); ");
        }
    }
    
    write("return BasicValue(" + tempVar + "); })()");
}

void CodeGenerator::visit(ExpressionStmt& node) {
    indent();
    node.expression->accept(*this);
    write(";\n");
}

void CodeGenerator::visit(VarStmt& node) {
    indent();
    if (!node.indices.empty()) {
        // Array assignment: array[index1, index2, ...] = value
        write("set_array_element(variables[\"" + node.variable + "\"], std::vector<BasicValue>{");
        for (size_t i = 0; i < node.indices.size(); ++i) {
            if (i > 0) write(", ");
            node.indices[i]->accept(*this);
        }
        write("}, ");
        node.value->accept(*this);
        write(");\n");
    } else if (!node.member.empty()) {
        // Struct member assignment: struct.member = value
        write("set_struct_field(std::get<BasicStruct>(variables[\"" + node.variable + "\"]), \"" + node.member + "\", ");
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
        write("return ");
        node.value->accept(*this);
        write(";\n");
    } else {
        write("return BasicValue(0);\n");
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
    // Store a copy of the struct declaration for later use
    structs[node.name] = std::make_unique<StructDecl>(node.name, node.fields, node.fieldTypes);
    
    // Generate struct comment for documentation
    writeLine("/* Struct: " + node.name + " */");
}

void CodeGenerator::visit(DimStmt& node) {
    indent();
    if (!node.dimensions.empty()) {
        // Array declaration
        write("variables[\"" + node.variable + "\"] = BasicArray(std::vector<int>{");
        for (size_t i = 0; i < node.dimensions.size(); i++) {
            write("to_int(");
            node.dimensions[i]->accept(*this);
            write(")");
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

void CodeGenerator::visit(ImportStmt& node) {
    // Import statements are resolved at compile time before parsing,
    // so we can safely skip them in the generated code since the
    // imported content has already been inlined.
    
    // Generate a comment for reference
    writeLine("// Import statement resolved at compile time: " + node.filename);
}

void CodeGenerator::visit(FFIFunctionDecl& node) {
    // Store the FFI function declaration for use in call generation
    ffiFunctions[node.name] = std::make_unique<FFIFunctionDecl>(node.name, node.library, node.returnType, node.parameters);
    
    // Generate comment in the output
    writeLine("// FFI Function Declaration: " + node.name + " from " + node.library);
}

void CodeGenerator::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    
    writeLine("");
    writeLine("return 0;");
}

bool CodeGenerator::isParallelizable(ModernForStmt& node) {
    // Conservative parallelization analysis
    // For initial implementation, we'll use simple heuristics
    
    // Check 1: Loop variable must be simple (integer-like)
    // For now, we assume all for loops use integer variables
    
    // Check 2: Look for potential data dependencies
    // This is a simplified analysis - we'll be conservative
    
    // Check 3: Must have simple increment pattern
    // Check if increment is just i = i + 1 or similar
    if (auto assignment = dynamic_cast<AssignExpr*>(node.increment.get())) {
        if (assignment->variable != node.variable) {
            return false; // Increment must modify the loop variable
        }
    } else {
        return false; // Only simple assignments for now
    }
    
    // Check 4: Body should not contain complex control flow
    // Avoid parallelizing loops with return statements
    for (auto& stmt : node.body) {
        // For initial implementation, be conservative about what we parallelize
        if (dynamic_cast<ReturnStmt*>(stmt.get())) {
            return false; // Conservative: avoid returns in parallel loops
        }
        // Note: We'll allow function calls for now, but this could be refined
    }
    
    // Check 5: Estimate iteration count heuristic
    // Only parallelize if likely to have enough iterations to overcome overhead
    // This is a rough heuristic based on the condition
    if (auto binaryExpr = dynamic_cast<BinaryExpr*>(node.condition.get())) {
        if (binaryExpr->operator_ == "<" || binaryExpr->operator_ == "<=" || 
            binaryExpr->operator_ == ">" || binaryExpr->operator_ == ">=") {
            // If condition involves literal numbers, check if iteration count is large enough
            if (auto literal = dynamic_cast<LiteralExpr*>(binaryExpr->right.get())) {
                if (std::holds_alternative<int>(literal->value)) {
                    int limit = std::get<int>(literal->value);
                    if (limit < 1000) {
                        return false; // Too few iterations for parallelism to be beneficial
                    }
                }
            }
        }
    }
    
    // If we made it here, the loop seems safe to parallelize
    return true;
}

} // namespace rbasic