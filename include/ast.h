#pragma once

#include "common.h"
#include "lexer.h"  // For TokenType
#include <memory>
#include <vector>

namespace rbasic {

// Forward declaration
class ASTVisitor;

// Base AST Node
class ASTNode {
protected:
    SourcePosition position_;
    
public:
    ASTNode(const SourcePosition& pos = SourcePosition()) : position_(pos) {}
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    
    const SourcePosition& getPosition() const { return position_; }
    void setPosition(const SourcePosition& pos) { position_ = pos; }
};

// Expression nodes
class Expression : public ASTNode {
public:
    Expression(const SourcePosition& pos = SourcePosition()) : ASTNode(pos) {}
    virtual ~Expression() = default;
};

class LiteralExpr : public Expression {
public:
    ValueType value;
    
    explicit LiteralExpr(ValueType v, const SourcePosition& pos = SourcePosition()) 
        : Expression(pos), value(std::move(v)) {}
    void accept(ASTVisitor& visitor) override;
};

class VariableExpr : public Expression {
public:
    std::string name;
    std::vector<std::unique_ptr<Expression>> indices; // For multidimensional array access
    std::string member;                               // For struct member access
    
    explicit VariableExpr(std::string n, std::vector<std::unique_ptr<Expression>> idx = {}, 
                         std::string mem = "", const SourcePosition& pos = SourcePosition())
        : Expression(pos), name(std::move(n)), indices(std::move(idx)), member(std::move(mem)) {}
    void accept(ASTVisitor& visitor) override;
};

class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    std::string operator_;
    std::unique_ptr<Expression> right;
    
    BinaryExpr(std::unique_ptr<Expression> l, std::string op, std::unique_ptr<Expression> r,
               const SourcePosition& pos = SourcePosition())
        : Expression(pos), left(std::move(l)), operator_(std::move(op)), right(std::move(r)) {}
    void accept(ASTVisitor& visitor) override;
};

class AssignExpr : public Expression {
public:
    std::string variable;
    std::unique_ptr<Expression> value;
    std::vector<std::unique_ptr<Expression>> indices;  // For multidimensional array assignment
    
    AssignExpr(std::string var, std::unique_ptr<Expression> val, std::vector<std::unique_ptr<Expression>> idx = {})
        : variable(std::move(var)), value(std::move(val)), indices(std::move(idx)) {}
    void accept(ASTVisitor& visitor) override;
};

class ComponentAssignExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string component;
    std::unique_ptr<Expression> value;
    
    ComponentAssignExpr(std::unique_ptr<Expression> obj, std::string comp, std::unique_ptr<Expression> val)
        : object(std::move(obj)), component(std::move(comp)), value(std::move(val)) {}
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

class StructLiteralExpr : public Expression {
public:
    std::string structName;
    std::vector<std::unique_ptr<Expression>> values;
    
    StructLiteralExpr(std::string name, std::vector<std::unique_ptr<Expression>> vals)
        : structName(std::move(name)), values(std::move(vals)) {}
    void accept(ASTVisitor& visitor) override;
};

class GLMConstructorExpr : public Expression {
public:
    TokenType glmType;  // VEC2, VEC3, VEC4, MAT3, MAT4, QUAT
    std::vector<std::unique_ptr<Expression>> arguments;
    
    GLMConstructorExpr(TokenType type, std::vector<std::unique_ptr<Expression>> args, 
                      const SourcePosition& pos = SourcePosition())
        : Expression(pos), glmType(type), arguments(std::move(args)) {}
    void accept(ASTVisitor& visitor) override;
};

class GLMComponentAccessExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string component;  // "x", "y", "z", "w"
    
    GLMComponentAccessExpr(std::unique_ptr<Expression> obj, std::string comp,
                          const SourcePosition& pos = SourcePosition())
        : Expression(pos), object(std::move(obj)), component(std::move(comp)) {}
    void accept(ASTVisitor& visitor) override;
};

class MemberAccessExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string member;  // struct member name
    
    MemberAccessExpr(std::unique_ptr<Expression> obj, std::string mem,
                    const SourcePosition& pos = SourcePosition())
        : Expression(pos), object(std::move(obj)), member(std::move(mem)) {}
    void accept(ASTVisitor& visitor) override;
};

// Statement nodes
class Statement : public ASTNode {
public:
    Statement(const SourcePosition& pos = SourcePosition()) : ASTNode(pos) {}
    virtual ~Statement() = default;
};

class ExpressionStmt : public Statement {
public:
    std::unique_ptr<Expression> expression;
    
    explicit ExpressionStmt(std::unique_ptr<Expression> expr, const SourcePosition& pos = SourcePosition())
        : Statement(pos), expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

class VarStmt : public Statement {
public:
    std::string variable;
    std::vector<std::unique_ptr<Expression>> indices; // For multidimensional array assignment
    std::string member;                                // For struct member assignment
    std::unique_ptr<Expression> value;
    
    VarStmt(std::string var, std::unique_ptr<Expression> val, 
            std::vector<std::unique_ptr<Expression>> idx = {}, std::string mem = "")
        : variable(std::move(var)), indices(std::move(idx)), member(std::move(mem)), value(std::move(val)) {}
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
                 std::vector<std::string> paramTypes_, std::string retType,
                 std::vector<std::unique_ptr<Statement>> stmts)
        : name(std::move(n)), parameters(std::move(params)), 
          paramTypes(std::move(paramTypes_)), returnType(std::move(retType)), 
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

// Import Statement  
class ImportStmt : public Statement {
public:
    std::string filename;
    
    explicit ImportStmt(std::string file)
        : filename(std::move(file)) {}
    void accept(ASTVisitor& visitor) override;
};

// FFI Function Declaration
class FFIFunctionDecl : public Statement {
public:
    std::string name;
    std::string library;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters; // name, type pairs
    
    FFIFunctionDecl(std::string n, std::string lib, std::string retType, 
                    std::vector<std::pair<std::string, std::string>> params)
        : name(std::move(n)), library(std::move(lib)), returnType(std::move(retType)), 
          parameters(std::move(params)) {}
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
    virtual void visit(ComponentAssignExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(CallExpr& node) = 0;
    virtual void visit(StructLiteralExpr& node) = 0;
    virtual void visit(GLMConstructorExpr& node) = 0;
    virtual void visit(GLMComponentAccessExpr& node) = 0;
    virtual void visit(MemberAccessExpr& node) = 0;
    
    virtual void visit(ExpressionStmt& node) = 0;
    virtual void visit(VarStmt& node) = 0;
    virtual void visit(PrintStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(ModernForStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;
    virtual void visit(FunctionDecl& node) = 0;
    virtual void visit(StructDecl& node) = 0;
    virtual void visit(DimStmt& node) = 0;
    virtual void visit(InputStmt& node) = 0;
    virtual void visit(ImportStmt& node) = 0;
    virtual void visit(FFIFunctionDecl& node) = 0;
    
    virtual void visit(Program& node) = 0;
};

} // namespace rbasic