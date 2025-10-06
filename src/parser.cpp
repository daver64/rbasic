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

Token Parser::consumeIdentifierOrKeyword(const std::string& message) {
    Token current = peek();
    
    // Allow identifiers and keywords that can serve as parameter names
    if (current.type == TokenType::IDENTIFIER ||
        current.type == TokenType::TYPE ||
        current.type == TokenType::VAR ||
        current.type == TokenType::FUNCTION) {
        return advance();
    }
    
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
            std::vector<std::unique_ptr<Expression>> indices;
            
            // Check if this is an array assignment
            if (!varExpr->indices.empty()) {
                // Extract the index expressions
                indices = std::move(varExpr->indices);
            }
            
            auto value = assignment(); // Right associative
            expr.release(); // Release the variable expression since we're not using it
            return std::make_unique<AssignExpr>(variable, std::move(value), std::move(indices));
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
        } else if (match({TokenType::LEFT_BRACE})) {
            // Handle struct literal: StructName { value1, value2, ... }
            std::vector<std::unique_ptr<Expression>> values;
            
            if (!check(TokenType::RIGHT_BRACE)) {
                do {
                    values.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            
            consume(TokenType::RIGHT_BRACE, "Expected '}' after struct values");
            
            if (auto var = dynamic_cast<VariableExpr*>(expr.get())) {
                std::string name = var->name;
                expr.release(); // Release ownership
                expr = std::make_unique<StructLiteralExpr>(name, std::move(values));
            } else {
                throw SyntaxError("Invalid struct literal");
            }
        } else if (match({TokenType::LEFT_BRACKET})) {
            std::vector<std::unique_ptr<Expression>> indices;
            
            // Parse first index
            indices.push_back(expression());
            
            // Parse additional indices separated by commas
            while (match({TokenType::COMMA})) {
                indices.push_back(expression());
            }
            
            consume(TokenType::RIGHT_BRACKET, "Expected ']' after array indices");
            
            if (auto var = dynamic_cast<VariableExpr*>(expr.get())) {
                var->indices = std::move(indices);
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
        if (hasDecimalPoint(value)) {
            return std::make_unique<LiteralExpr>(std::stod(value));
        } else {
            return std::make_unique<LiteralExpr>(std::stoi(value));
        }
    }
    
    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().value);
    }
    
    if (match({TokenType::NULL_LITERAL})) {
        return std::make_unique<LiteralExpr>(static_cast<void*>(nullptr));
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
        if (match({TokenType::DECLARE})) return declareStatement();
        if (match({TokenType::FFI})) return ffiStatement();
        if (match({TokenType::IMPORT})) return importStatement();
        
        return expressionStatement();
    } catch (const std::exception&) {
        synchronize();
        throw;
    }
}

std::unique_ptr<Statement> Parser::varStatement() {
    auto name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    // Check for array declaration: var array[size];
    if (match({TokenType::LEFT_BRACKET})) {
        auto size = expression();
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after array size");
        consume(TokenType::SEMICOLON, "Expected ';' after array declaration");
        
        std::vector<std::unique_ptr<Expression>> dimensions;
        dimensions.push_back(std::move(size));
        return std::make_unique<DimStmt>(name.value, "variant", std::move(dimensions));
    }
    
    // Check for struct member assignment: var struct.member = value
    std::string member = "";
    if (match({TokenType::DOT})) {
        auto memberToken = consume(TokenType::IDENTIFIER, "Expected member name after '.'");
        member = memberToken.value;
    }
    
    // Check for array assignment: var array[index] = value
    std::vector<std::unique_ptr<Expression>> indices;
    if (match({TokenType::LEFT_BRACKET})) {
        // Parse first index
        indices.push_back(expression());
        
        // Parse additional indices separated by commas
        while (match({TokenType::COMMA})) {
            indices.push_back(expression());
        }
        
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after array indices");
    }
    
    consume(TokenType::ASSIGN, "Expected '=' after variable name");
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VarStmt>(name.value, std::move(value), std::move(indices), member);
}

std::unique_ptr<Statement> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition");
    
    // C-style braces: if (condition) { statements }
    consume(TokenType::LEFT_BRACE, "Expected '{' after if condition");
    auto thenBranch = blockUntil(TokenType::RIGHT_BRACE);
    consume(TokenType::RIGHT_BRACE, "Expected '}' after if body");
    
    std::vector<std::unique_ptr<Statement>> elseBranch;
    if (match({TokenType::ELSE})) {
        if (check(TokenType::IF)) {
            // Handle else-if as a nested if statement
            consume(TokenType::IF, "Expected 'if' in 'else if'");
            elseBranch.push_back(ifStatement());
        } else {
            // Handle regular else with braces
            consume(TokenType::LEFT_BRACE, "Expected '{' after 'else'");
            elseBranch = blockUntil(TokenType::RIGHT_BRACE);
            consume(TokenType::RIGHT_BRACE, "Expected '}' after else body");
        }
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'");
    
    // Parse initialization: var i = 1 OR i = 1
    Token variable = peek(); // Initialize with current token
    if (check(TokenType::VAR)) {
        // New variable declaration: var i = 1
        advance(); // consume 'var'
        variable = consume(TokenType::IDENTIFIER, "Expected variable name after 'var'");
        consume(TokenType::ASSIGN, "Expected '=' after for variable");
    } else {
        // Existing variable assignment: i = 1
        variable = consume(TokenType::IDENTIFIER, "Expected variable name in for loop");
        consume(TokenType::ASSIGN, "Expected '=' after for variable");
    }
    
    auto initialization = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after for initialization");
    
    // Parse condition: i <= 10
    auto condition = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after for condition");
    
    // Parse increment: i = i + 1
    auto increment = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after for increment");
    
    // C-style braces only: for(...) { statements }
    consume(TokenType::LEFT_BRACE, "Expected '{' after for statement");
    auto body = blockUntil(TokenType::RIGHT_BRACE);
    consume(TokenType::RIGHT_BRACE, "Expected '}' after for body");
    
    return std::make_unique<ModernForStmt>(variable.value, std::move(initialization), 
                                          std::move(condition), std::move(increment), 
                                          std::move(body));
}

std::unique_ptr<Statement> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition");
    
    // C-style braces only: while(condition) { statements }
    consume(TokenType::LEFT_BRACE, "Expected '{' after while condition");
    auto body = blockUntil(TokenType::RIGHT_BRACE);
    consume(TokenType::RIGHT_BRACE, "Expected '}' after while body");
    
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
    
    // C-style braces: function name() { statements }
    consume(TokenType::LEFT_BRACE, "Expected '{' after function signature");
    auto body = blockUntil(TokenType::RIGHT_BRACE);
    consume(TokenType::RIGHT_BRACE, "Expected '}' after function body");
    
    return std::make_unique<FunctionDecl>(name.value, std::move(parameters), std::move(paramTypes), returnType, std::move(body));
}

std::unique_ptr<Statement> Parser::structDeclaration() {
    auto name = consume(TokenType::IDENTIFIER, "Expected struct name");
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after struct name");
    
    std::vector<std::string> fields;
    std::vector<std::string> fieldTypes;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto field = consume(TokenType::IDENTIFIER, "Expected field name");
        fields.push_back(field.value);
        fieldTypes.push_back("variant"); // Default type for modern syntax
        
        if (match({TokenType::COMMA})) {
            // Continue to next field
        } else if (check(TokenType::RIGHT_BRACE)) {
            // End of struct body
            break;
        } else {
            throw std::runtime_error("Expected ',' or '}' after field name");
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after struct body");
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

std::unique_ptr<Statement> Parser::importStatement() {
    // Parse: import 'filename.bas';
    auto filename = consume(TokenType::STRING, "Expected filename string after 'import'");
    consume(TokenType::SEMICOLON, "Expected ';' after import statement");
    
    return std::make_unique<ImportStmt>(filename.value);
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

std::unique_ptr<Statement> Parser::declareStatement() {
    // Parse: declare [ffi] function FunctionName [from "library" | lib "library"] (param1 as type1, ...) as returnType;
    
    // Check for optional 'ffi' keyword
    bool isFFI = false;
    if (check(TokenType::FFI)) {
        isFFI = true;
        advance(); // consume FFI token
    }
    
    consume(TokenType::FUNCTION, "Expected 'function' after 'declare' [ffi]");
    
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected function name");
    std::string functionName = nameToken.value;
    
    // Handle both 'from' and 'lib' keywords for library specification
    if (isFFI) {
        if (match({TokenType::FROM})) {
            // declare ffi function Name from "library" 
        } else {
            consume(TokenType::LIB, "Expected 'from' or 'lib' after function name in FFI declaration");
        }
    } else {
        consume(TokenType::LIB, "Expected 'lib' after function name");
    }
    
    Token libToken = consume(TokenType::STRING, "Expected library name as string");
    std::string libraryName = libToken.value;
    // Remove quotes from library name
    if (libraryName.length() >= 2 && libraryName[0] == '"' && libraryName.back() == '"') {
        libraryName = libraryName.substr(1, libraryName.length() - 2);
    }
    
    // Parse parameters: (param1 as type1, param2 as type2, ...)
    std::vector<std::pair<std::string, std::string>> parameters;
    if (check(TokenType::LEFT_PAREN)) {
        consume(TokenType::LEFT_PAREN, "Expected '(' for parameters");
        
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                Token paramName = consumeIdentifierOrKeyword("Expected parameter name");
                consume(TokenType::AS, "Expected 'as' after parameter name");
                
                // Parse parameter type (may include 'pointer' keyword)
                std::string paramType;
                if (check(TokenType::POINTER)) {
                    advance(); // consume 'pointer'
                    paramType = "pointer";
                } else {
                    Token paramTypeToken = consumeIdentifierOrKeyword("Expected parameter type");
                    paramType = paramTypeToken.value;
                    
                    // Check for pointer suffix (e.g., "SDL_Window*")
                    if (check(TokenType::MULTIPLY)) {
                        advance(); // consume '*'
                        paramType += "*";
                    }
                }
                
                parameters.emplace_back(paramName.value, paramType);
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    }
    
    // Parse return type: as returnType
    consume(TokenType::AS, "Expected 'as' for return type");
    
    std::string returnType;
    if (check(TokenType::POINTER)) {
        advance(); // consume 'pointer'
        returnType = "pointer";
    } else {
        Token returnTypeToken = consumeIdentifierOrKeyword("Expected return type");
        returnType = returnTypeToken.value;
        
        // Check for pointer suffix
        if (check(TokenType::MULTIPLY)) {
            advance(); // consume '*'
            returnType += "*";
        }
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after declare statement");
    
    return std::make_unique<FFIFunctionDecl>(functionName, libraryName, returnType, parameters);
}

// Parse direct FFI syntax: ffi "library" FunctionName(params) as returnType;
std::unique_ptr<Statement> Parser::ffiStatement() {
    // Parse: ffi returnType FunctionName(param1 as type1, param2 as type2, ...) from "library";
    
    // Parse return type (can be identifier or 'pointer' keyword)
    std::string returnType;
    if (check(TokenType::POINTER)) {
        advance(); // consume 'pointer' keyword
        returnType = "pointer";
    } else {
        Token returnTypeToken = consume(TokenType::IDENTIFIER, "Expected return type after 'ffi'");
        returnType = returnTypeToken.value;
    }
    
    // Check for pointer syntax (e.g., "integer*")
    if (check(TokenType::MULTIPLY)) {
        advance(); // consume '*'
        returnType += "*";
    }
    
    // Parse function name
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected function name");
    std::string functionName = nameToken.value;
    
    // Parse parameters
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    std::vector<std::pair<std::string, std::string>> parameters;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            // Parse parameter name
            Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
            std::string paramNameStr = paramName.value;
            
            // Parse 'as' keyword
            consume(TokenType::AS, "Expected 'as' after parameter name");
            
            // Parse parameter type (can be identifier or 'pointer' keyword)
            std::string typeStr;
            if (check(TokenType::POINTER)) {
                advance(); // consume 'pointer' keyword
                typeStr = "pointer";
            } else {
                Token paramType = consume(TokenType::IDENTIFIER, "Expected parameter type after 'as'");
                typeStr = paramType.value;
            }
            
            // Check for pointer syntax (e.g., "integer*")
            if (check(TokenType::MULTIPLY)) {
                advance(); // consume '*'
                typeStr += "*";
            }
            
            parameters.emplace_back(paramNameStr, typeStr);
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    
    // Parse 'from' keyword and library name
    consume(TokenType::FROM, "Expected 'from' after function parameters");
    Token libToken = consume(TokenType::STRING, "Expected library name string after 'from'");
    std::string libraryName = libToken.value;
    
    // Remove quotes from library name
    if (libraryName.length() >= 2 && libraryName[0] == '"' && libraryName.back() == '"') {
        libraryName = libraryName.substr(1, libraryName.length() - 2);
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after FFI declaration");
    
    return std::make_unique<FFIFunctionDecl>(functionName, libraryName, returnType, parameters);
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