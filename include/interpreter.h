#pragma once

#include "common.h"
#include "ast.h"
#include "io_handler.h"
#include <map>
#include <stack>
#include <memory>

namespace rbasic {

class Interpreter : public ASTVisitor {
private:
    std::map<std::string, ValueType> globals;
    std::stack<std::map<std::string, ValueType>> scopes;
    std::map<std::string, std::unique_ptr<FunctionDecl>> functions;
    std::map<std::string, std::unique_ptr<StructDecl>> structs;
    std::map<std::string, std::map<std::string, ValueType>> structInstances;
    ValueType lastValue;
    bool hasReturned;
    std::unique_ptr<IOHandler> ioHandler;
    
    void defineVariable(const std::string& name, const ValueType& value);
    ValueType getVariable(const std::string& name);
    void setVariable(const std::string& name, const ValueType& value);
    
    void pushScope();
    void popScope();
    
public:
    Interpreter(std::unique_ptr<IOHandler> io = nullptr);
    
    void interpret(Program& program);
    ValueType evaluate(Expression& expr);
    
    // Get the IO handler (for external access if needed)
    IOHandler* getIOHandler() const;
    
    // Visitor methods
    void visit(LiteralExpr& node) override;
    void visit(VariableExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(AssignExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(CallExpr& node) override;
    
    void visit(ExpressionStmt& node) override;
    void visit(VarStmt& node) override;
    void visit(PrintStmt& node) override;
    void visit(InputStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(ForStmt& node) override;
    void visit(ModernForStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(ReturnStmt& node) override;
    void visit(FunctionDecl& node) override;
    void visit(StructDecl& node) override;
    void visit(DimStmt& node) override;
    
    void visit(Program& node) override;
};

} // namespace rbasic