#pragma once

#include "common.h"
#include <memory>
#include <vector>

namespace rbasic {

// Forward declaration
class ASTVisitor;

// Base AST Node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Expression nodes
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class LiteralExpr : public Expression {
public:
    ValueType value;
    
    explicit LiteralExpr(ValueType v) : value(std::move(v)) {}
    void accept(ASTVisitor& visitor) override;
};

class VariableExpr : public Expression {
public:
    std::string name;
    std::unique_ptr<Expression> index; // For array access
    std::string member;                // For struct member access
    
    explicit VariableExpr(std::string n, std::unique_ptr<Expression> idx = nullptr, std::string mem = "")
        : name(std::move(n)), index(std::move(idx)), member(std::move(mem)) {}
    void accept(ASTVisitor& visitor) override;
};

class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string operator_;
    std::unique_ptr<Expression> right;
    
    BinaryExpr(std::unique_ptr<Expression> l, std::string op, std::unique_ptr<Expression> r)
        : left(std::move(l)), operator_(std::move(op)), right(std::move(r)) {}
    void accept(ASTVisitor& visitor) override;
};

class AssignExpr : public Expression {
public:
    std::string variable;
    std::unique_ptr<Expression> value;
    
    AssignExpr(std::string var, std::unique_ptr<Expression> val)
        : variable(std::move(var)), value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override;
};

class UnaryExpr : public Expression {
public:
    std::string operator_;
    std::unique_ptr<Expression> operand;
    
    UnaryExpr(std::string op, std::unique_ptr<Expression> expr)
        : operator_(std::move(op)), operand(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

class CallExpr : public Expression {
public:
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpr(std::string n, std::vector<std::unique_ptr<Expression>> args)
        : name(std::move(n)), arguments(std::move(args)) {}
    void accept(ASTVisitor& visitor) override;
};

// Statement nodes
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ExpressionStmt : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    explicit ExpressionStmt(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

class VarStmt : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> index; // For array assignment
    std::string member;                // For struct member assignment
    std::unique_ptr<Expression> value;
    
    VarStmt(std::string var, std::unique_ptr<Expression> val, 
            std::unique_ptr<Expression> idx = nullptr, std::string mem = "")
        : variable(std::move(var)), index(std::move(idx)), member(std::move(mem)), value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override;
};

class PrintStmt : public Statement {
public:
    std::vector<std::unique_ptr<Expression>> expressions;
    
    explicit PrintStmt(std::vector<std::unique_ptr<Expression>> exprs)
        : expressions(std::move(exprs)) {}
    void accept(ASTVisitor& visitor) override;
};

class IfStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseBranch;
    
    IfStmt(std::unique_ptr<Expression> cond, 
           std::vector<std::unique_ptr<Statement>> thenStmts,
           std::vector<std::unique_ptr<Statement>> elseStmts = {})
        : condition(std::move(cond)), 
          thenBranch(std::move(thenStmts)), 
          elseBranch(std::move(elseStmts)) {}
    void accept(ASTVisitor& visitor) override;
};

class ForStmt : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;
    std::unique_ptr<Expression> step;
    std::vector<std::unique_ptr<Statement>> body;
    
    ForStmt(std::string var, 
            std::unique_ptr<Expression> startExpr,
            std::unique_ptr<Expression> endExpr,
            std::unique_ptr<Expression> stepExpr,
            std::vector<std::unique_ptr<Statement>> stmts)
        : variable(std::move(var)),
          start(std::move(startExpr)),
          end(std::move(endExpr)),
          step(std::move(stepExpr)),
          body(std::move(stmts)) {}
    void accept(ASTVisitor& visitor) override;
};

class ModernForStmt : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> initialization;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> increment;
    std::vector<std::unique_ptr<Statement>> body;
    
    ModernForStmt(std::string var,
                  std::unique_ptr<Expression> init,
                  std::unique_ptr<Expression> cond,
                  std::unique_ptr<Expression> incr,
                  std::vector<std::unique_ptr<Statement>> stmts)
        : variable(std::move(var)),
          initialization(std::move(init)),
          condition(std::move(cond)),
          increment(std::move(incr)),
          body(std::move(stmts)) {}
    void accept(ASTVisitor& visitor) override;
};

class WhileStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> body;
    
    WhileStmt(std::unique_ptr<Expression> cond, std::vector<std::unique_ptr<Statement>> stmts)
        : condition(std::move(cond)), body(std::move(stmts)) {}
    void accept(ASTVisitor& visitor) override;
};

class ReturnStmt : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    explicit ReturnStmt(std::unique_ptr<Expression> val = nullptr)
        : value(std::move(val)) {}
    void accept(ASTVisitor& visitor) override;
};

class FunctionDecl : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> paramTypes;
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;
    
    FunctionDecl(std::string n, std::vector<std::string> params, 
                 std::vector<std::string> paramTypes, std::string retType,
                 std::vector<std::unique_ptr<Statement>> stmts)
        : name(std::move(n)), parameters(std::move(params)), 
          paramTypes(std::move(paramTypes)), returnType(std::move(retType)), 
          body(std::move(stmts)) {}
    void accept(ASTVisitor& visitor) override;
};

class StructDecl : public Statement {
public:
    std::string name;
    std::vector<std::string> fields;
    std::vector<std::string> fieldTypes;
    
    StructDecl(std::string n, std::vector<std::string> f, std::vector<std::string> ft)
        : name(std::move(n)), fields(std::move(f)), fieldTypes(std::move(ft)) {}
    void accept(ASTVisitor& visitor) override;
};

class DimStmt : public Statement {
public:
    std::string variable;
    std::string type;
    std::vector<std::unique_ptr<Expression>> dimensions; // For arrays
    
    DimStmt(std::string var, std::string t, std::vector<std::unique_ptr<Expression>> dims = {})
        : variable(std::move(var)), type(std::move(t)), dimensions(std::move(dims)) {}
    void accept(ASTVisitor& visitor) override;
};

class InputStmt : public Statement {
public:
    std::string variable;
    
    explicit InputStmt(std::string var)
        : variable(std::move(var)) {}
    void accept(ASTVisitor& visitor) override;
};

// Program node
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    explicit Program(std::vector<std::unique_ptr<Statement>> stmts)
        : statements(std::move(stmts)) {}
    void accept(ASTVisitor& visitor) override;
};

// Visitor pattern
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(LiteralExpr& node) = 0;
    virtual void visit(VariableExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(AssignExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(CallExpr& node) = 0;
    
    virtual void visit(ExpressionStmt& node) = 0;
    virtual void visit(VarStmt& node) = 0;
    virtual void visit(PrintStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(ForStmt& node) = 0;
    virtual void visit(ModernForStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;
    virtual void visit(FunctionDecl& node) = 0;
    virtual void visit(StructDecl& node) = 0;
    virtual void visit(DimStmt& node) = 0;
    virtual void visit(InputStmt& node) = 0;
    
    virtual void visit(Program& node) = 0;
};

} // namespace rbasic