#pragma once

#include "common.h"
#include "lexer.h"
#include "ast.h"
#include <vector>
#include <memory>

namespace rbasic {

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    Token consumeIdentifierOrKeyword(const std::string& message); // Allow keywords as identifiers in FFI context
    void synchronize();
    
    // Expression parsing (precedence climbing)
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logical_or();
    std::unique_ptr<Expression> logical_and();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
    
    // Statement parsing
    std::unique_ptr<Statement> statement();
    std::unique_ptr<Statement> varStatement();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> forStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<Statement> functionDeclaration();
    std::unique_ptr<Statement> structDeclaration();
    std::unique_ptr<Statement> dimStatement();
    std::unique_ptr<Statement> importStatement();
    std::unique_ptr<Statement> declareStatement();
    std::unique_ptr<Statement> ffiStatement();
    std::unique_ptr<Statement> expressionStatement();
    
    // Helper methods
    std::vector<std::unique_ptr<Expression>> arguments();
    std::vector<std::unique_ptr<Statement>> blockUntil(TokenType endToken);
    
public:
    explicit Parser(std::vector<Token> tokens);
    
    std::unique_ptr<Program> parse();
};

} // namespace rbasic