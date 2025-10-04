#include "parser.h"

namespace rbasic {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {}

Token Parser::peek() const {
    if (current >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", -1, -1);
    }
    return tokens[current];
}

Token Parser::previous() const {
    if (current == 0) {
        return Token(TokenType::EOF_TOKEN, "", -1, -1);
    }
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    Token current = peek();
    throw SyntaxError(message + " at '" + current.value + "'", current.line);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
            case TokenType::FUNCTION:
            case TokenType::STRUCT:
            case TokenType::DIM:
                return;
            default:
                break;
        }
        
        advance();
    }
}

// Expression parsing
std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expression> Parser::assignment() {
    auto expr = logical_or();
    
    if (match({TokenType::ASSIGN})) {
        // This should be a variable expression
        if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            std::string variable = varExpr->name;
            auto value = assignment(); // Right associative
            expr.release(); // Release the variable expression since we're not using it
            return std::make_unique<AssignExpr>(variable, std::move(value));
        } else {
            throw SyntaxError("Invalid assignment target");
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logical_or() {
    auto expr = logical_and();
    
    while (match({TokenType::OR})) {
        std::string op = previous().value;
        auto right = logical_and();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::logical_and() {
    auto expr = equality();
    
    while (match({TokenType::AND})) {
        std::string op = previous().value;
        auto right = equality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
        std::string op = previous().value;
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = term();
    
    while (match({TokenType::GREATER_THAN, TokenType::GREATER_EQUAL, 
                  TokenType::LESS_THAN, TokenType::LESS_EQUAL})) {
        std::string op = previous().value;
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr = factor();
    
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        std::string op = previous().value;
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr = unary();
    
    while (match({TokenType::DIVIDE, TokenType::MULTIPLY, TokenType::MODULO})) {
        std::string op = previous().value;
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        std::string op = previous().value;
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    
    return call();
}

std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            auto args = arguments();
            consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");
            
            if (auto var = dynamic_cast<VariableExpr*>(expr.get())) {
                std::string name = var->name;
                expr.release(); // Release ownership
                expr = std::make_unique<CallExpr>(name, std::move(args));
            } else {
                throw SyntaxError("Invalid function call");
            }
        } else if (match({TokenType::LEFT_BRACKET})) {
            auto index = expression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after array index");
            
            if (auto var = dynamic_cast<VariableExpr*>(expr.get())) {
                var->index = std::move(index);
            }
        } else if (match({TokenType::DOT})) {
            auto member = consume(TokenType::IDENTIFIER, "Expected member name after '.'");
            
            if (auto var = dynamic_cast<VariableExpr*>(expr.get())) {
                var->member = member.value;
            }
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    if (match({TokenType::NUMBER})) {
        std::string value = previous().value;
        if (value.find('.') != std::string::npos) {
            return std::make_unique<LiteralExpr>(std::stod(value));
        } else {
            return std::make_unique<LiteralExpr>(std::stoi(value));
        }
    }
    
    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().value);
    }
    
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous().value);
    }
    
    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw SyntaxError("Expected expression");
}

std::vector<std::unique_ptr<Expression>> Parser::arguments() {
    std::vector<std::unique_ptr<Expression>> args;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    
    return args;
}

// Statement parsing
std::unique_ptr<Statement> Parser::statement() {
    try {
        if (match({TokenType::VAR})) return varStatement();
        if (match({TokenType::IF})) return ifStatement();
        if (match({TokenType::FOR})) return forStatement();
        if (match({TokenType::WHILE})) return whileStatement();
        if (match({TokenType::RETURN})) return returnStatement();
        if (match({TokenType::FUNCTION})) return functionDeclaration();
        if (match({TokenType::STRUCT})) return structDeclaration();
        if (match({TokenType::DIM})) return dimStatement();
        
        return expressionStatement();
    } catch (const std::exception&) {
        synchronize();
        throw;
    }
}

std::unique_ptr<Statement> Parser::varStatement() {
    auto name = consume(TokenType::IDENTIFIER, "Expected variable name");
    consume(TokenType::ASSIGN, "Expected '=' after variable name");
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VarStmt>(name.value, std::move(value));
}

std::unique_ptr<Statement> Parser::ifStatement() {
    auto condition = expression();
    consume(TokenType::THEN, "Expected 'then' after if condition");
    
    auto thenBranch = blockUntil(TokenType::ELSE);
    std::vector<std::unique_ptr<Statement>> elseBranch;
    
    if (match({TokenType::ELSE})) {
        elseBranch = blockUntil(TokenType::END);
    }
    
    consume(TokenType::END, "Expected 'end' after if statement");
    consume(TokenType::IF, "Expected 'if' after 'end'");
    consume(TokenType::SEMICOLON, "Expected ';' after end if");
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    
    // Parse initialization: var i = 1
    consume(TokenType::VAR, "Expected 'var' in for loop initialization");
    auto variable = consume(TokenType::IDENTIFIER, "Expected variable name in for loop");
    consume(TokenType::ASSIGN, "Expected '=' after for variable");
    auto initialization = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after for initialization");
    
    // Parse condition: i <= 10
    auto condition = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after for condition");
    
    // Parse increment: i = i + 1
    auto increment = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after for increment");
    
    auto body = blockUntil(TokenType::END);
    consume(TokenType::END, "Expected 'end' after for body");
    consume(TokenType::SEMICOLON, "Expected ';' after end statement");
    
    return std::make_unique<ModernForStmt>(variable.value, std::move(initialization), 
                                          std::move(condition), std::move(increment), 
                                          std::move(body));
}

std::unique_ptr<Statement> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition");
    auto body = blockUntil(TokenType::WEND);
    consume(TokenType::WEND, "Expected 'wend' after while body");
    consume(TokenType::SEMICOLON, "Expected ';' after wend");
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::returnStatement() {
    std::unique_ptr<Expression> value = nullptr;
    
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Statement> Parser::functionDeclaration() {
    auto name = consume(TokenType::IDENTIFIER, "Expected function name");
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    std::vector<std::string> parameters;
    std::vector<std::string> paramTypes;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            auto param = consume(TokenType::IDENTIFIER, "Expected parameter name");
            parameters.push_back(param.value);
            
            if (match({TokenType::AS})) {
                auto type = consume(TokenType::IDENTIFIER, "Expected parameter type");
                paramTypes.push_back(type.value);
            } else {
                paramTypes.push_back("variant");
            }
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    
    std::string returnType = "variant";
    if (match({TokenType::AS})) {
        auto type = consume(TokenType::IDENTIFIER, "Expected return type");
        returnType = type.value;
    }
    
    auto body = blockUntil(TokenType::END);
    consume(TokenType::END, "Expected 'end' after function body");
    consume(TokenType::FUNCTION, "Expected 'function' after 'end'");
    consume(TokenType::SEMICOLON, "Expected ';' after function declaration");
    
    return std::make_unique<FunctionDecl>(name.value, std::move(parameters), std::move(paramTypes), returnType, std::move(body));
}

std::unique_ptr<Statement> Parser::structDeclaration() {
    auto name = consume(TokenType::IDENTIFIER, "Expected struct name");
    
    std::vector<std::string> fields;
    std::vector<std::string> fieldTypes;
    
    while (!check(TokenType::END) && !isAtEnd()) {
        auto field = consume(TokenType::IDENTIFIER, "Expected field name");
        fields.push_back(field.value);
        
        if (match({TokenType::AS})) {
            auto type = consume(TokenType::IDENTIFIER, "Expected field type");
            fieldTypes.push_back(type.value);
        } else {
            fieldTypes.push_back("variant");
        }
        
        consume(TokenType::SEMICOLON, "Expected ';' after field declaration");
    }
    
    consume(TokenType::END, "Expected 'end' after struct body");
    consume(TokenType::STRUCT, "Expected 'struct' after 'end'");
    consume(TokenType::SEMICOLON, "Expected ';' after struct declaration");
    
    return std::make_unique<StructDecl>(name.value, std::move(fields), std::move(fieldTypes));
}

std::unique_ptr<Statement> Parser::dimStatement() {
    auto variable = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    std::vector<std::unique_ptr<Expression>> dimensions;
    if (match({TokenType::LEFT_PAREN})) {
        do {
            dimensions.push_back(expression());
        } while (match({TokenType::COMMA}));
        consume(TokenType::RIGHT_PAREN, "Expected ')' after array dimensions");
    }
    
    std::string type = "variant";
    if (match({TokenType::AS})) {
        auto typeToken = consume(TokenType::IDENTIFIER, "Expected type name");
        type = typeToken.value;
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after dim statement");
    
    return std::make_unique<DimStmt>(variable.value, type, std::move(dimensions));
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Statement>> Parser::blockUntil(TokenType endToken) {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!check(endToken) && !isAtEnd()) {
        statements.push_back(statement());
    }
    
    return statements;
}

std::unique_ptr<Program> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!isAtEnd()) {
        try {
            statements.push_back(statement());
        } catch (const std::exception& e) {
            // Error recovery - skip to next statement
            synchronize();
        }
    }
    
    return std::make_unique<Program>(std::move(statements));
}

} // namespace rbasic