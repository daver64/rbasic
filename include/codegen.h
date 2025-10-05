#pragma once

#include "common.h"
#include "ast.h"
#include <sstream>

namespace rbasic {

class CodeGenerator : public ASTVisitor {
private:
    std::ostringstream output;
    std::string functionDeclarations;
    int indentLevel;
    std::map<std::string, std::unique_ptr<StructDecl>> structs; // Store struct declarations
    
    void indent();
    void writeLine(const std::string& line);
    void write(const std::string& text);
    std::string generateVariableName(const std::string& basicName);
    std::string generateTempVar();
    std::string escapeString(const std::string& str);
    int tempVarCounter;
    
public:
    CodeGenerator();
    
    std::string generate(Program& program);
    
    // Visitor methods
    void visit(LiteralExpr& node) override;
    void visit(VariableExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(AssignExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(CallExpr& node) override;
    void visit(StructLiteralExpr& node) override;
    
    void visit(ExpressionStmt& node) override;
    void visit(VarStmt& node) override;
    void visit(PrintStmt& node) override;
    void visit(InputStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(ModernForStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(ReturnStmt& node) override;
    void visit(FunctionDecl& node) override;
    void visit(StructDecl& node) override;
    void visit(DimStmt& node) override;
    
    void visit(Program& node) override;
    
private:
    void generateIncludes();
    void generateMain();
};

} // namespace rbasic