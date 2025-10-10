#include "ast.h"

namespace rbasic {

void InputStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ImportStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FFIFunctionDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Program::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Expression implementations
void LiteralExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VariableExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void AssignExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ComponentAssignExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void CallExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void StructLiteralExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// Statement implementations
void ExpressionStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VarStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void PrintStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void IfStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ModernForStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void WhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void StructDecl::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void DimStmt::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void GLMConstructorExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void GLMComponentAccessExpr::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace rbasic