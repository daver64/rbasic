#pragma once

#include "common.h"
#include "ast.h"
#include "io_handler.h"
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <memory>

namespace rbasic {

class Interpreter : public ASTVisitor {
private:
    std::map<std::string, ValueType> globals;
    std::vector<std::map<std::string, ValueType>> scopes;
    std::map<std::string, std::unique_ptr<FunctionDecl>> functions;
    std::map<std::string, std::unique_ptr<StructDecl>> structs;
    std::map<std::string, std::map<std::string, ValueType>> structInstances;
    std::set<std::string> importedFiles;     // Track imported files to prevent re-importing
    std::set<std::string> importStack;       // Track current import chain for circular detection
    std::string currentFile;                 // Track current file being processed
    ValueType lastValue;
    bool hasReturned;
    std::unique_ptr<IOHandler> ioHandler;
    SourcePosition currentPosition;  // Track current source position for error reporting
    
    void defineVariable(const std::string& name, const ValueType& value);
    ValueType getVariable(const std::string& name);
    bool variableExists(const std::string& name);
    void setVariable(const std::string& name, const ValueType& value);
    
    void pushScope();
    void popScope();
    
    // Import resolution helper
    std::string resolveImportPath(const std::string& filename);
    std::string getCurrentExecutablePath();
    
public:
    Interpreter(std::unique_ptr<IOHandler> io = nullptr);
    
    void interpret(Program& program);
    ValueType evaluate(Expression& expr);
    
    // Set current file for import path resolution
    void setCurrentFile(const std::string& filepath) { currentFile = filepath; }
    
    // Get the IO handler (for external access if needed)
    IOHandler* getIOHandler() const;
    
    // Position tracking for better error messages
    void setCurrentPosition(const SourcePosition& pos) { currentPosition = pos; }
    const SourcePosition& getCurrentPosition() const { return currentPosition; }
    
    // Function call dispatch methods
    bool handleIOFunctions(CallExpr& node);
    bool handleMathFunctions(CallExpr& node);
    bool handleStringFunctions(CallExpr& node);
    bool handleArrayFunctions(CallExpr& node);
    bool handleFileFunctions(CallExpr& node);
    bool handleTerminalFunctions(CallExpr& node);
    bool handleRPIFunctions(CallExpr& node);
    bool handleSDL2Functions(CallExpr& node);
    bool handleSQLite3Functions(CallExpr& node);
    bool handleUserDefinedFunction(CallExpr& node);
    
    // Visitor methods
    void visit(LiteralExpr& node) override;
    void visit(VariableExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(AssignExpr& node) override;
    void visit(ComponentAssignExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(CallExpr& node) override;
    void visit(StructLiteralExpr& node) override;
    void visit(GLMConstructorExpr& node) override;
    void visit(GLMComponentAccessExpr& node) override;
    void visit(MemberAccessExpr& node) override;
    
    void visit(ExpressionStmt& node) override;
    void visit(VarStmt& node) override;
    void visit(PrintStmt& node) override;
    void visit(InputStmt& node) override;
    void visit(ImportStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(ModernForStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(ReturnStmt& node) override;
    void visit(FunctionDecl& node) override;
    void visit(StructDecl& node) override;
    void visit(DimStmt& node) override;
    
    void visit(Program& node) override;
};

} // namespace rbasic