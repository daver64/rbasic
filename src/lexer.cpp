#include "lexer.h"
#include <cctype>
#include <unordered_map>
#include <algorithm>

namespace rbasic {

Lexer::Lexer(const std::string& source) 
    : source(source), current(0), line(1), column(1) {}

char Lexer::peek(int offset) const {
    size_t pos = current + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    if (current >= source.length()) return '\0';
    
    char c = source[current++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (current < source.length()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    // Skip until end of line for single-line comments
    while (peek() != '\0' && peek() != '\n') {
        advance();
    }
}

void Lexer::skipBlockComment() {
    // Skip until */
    while (peek() != '\0') {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // Skip *
            advance(); // Skip /
            break;
        }
        advance();
    }
}

Token Lexer::makeNumber() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    
    while (std::isdigit(peek()) || peek() == '.') {
        value += advance();
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn);
}

Token Lexer::makeString() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    
    advance(); // Skip opening quote
    
    while (peek() != '\0' && peek() != '"') {
        if (peek() == '\\') {
            advance(); // Skip backslash
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default: value += escaped; break;
            }
        } else {
            value += advance();
        }
    }
    
    if (peek() == '"') advance(); // Skip closing quote
    
    return Token(TokenType::STRING, value, startLine, startColumn);
}

Token Lexer::makeIdentifier() {
    std::string value;
    int startLine = line;
    int startColumn = column;
    
    while (std::isalnum(peek()) || peek() == '_') {
        value += advance();
    }
    
    // Convert to lowercase for case-insensitive keywords
    std::string lowercase = value;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
    
    TokenType type = getKeywordType(lowercase);
    if (type == TokenType::INVALID) {
        type = TokenType::IDENTIFIER;
    }
    
    return Token(type, value, startLine, startColumn);
}

TokenType Lexer::getKeywordType(const std::string& text) const {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"var", TokenType::VAR},
        {"if", TokenType::IF},
        {"then", TokenType::THEN},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"function", TokenType::FUNCTION},
        {"return", TokenType::RETURN},
        {"dim", TokenType::DIM},
        {"struct", TokenType::STRUCT},
        {"type", TokenType::TYPE},
        {"pointer", TokenType::POINTER},
        {"as", TokenType::AS},
        {"call", TokenType::CALL},
        {"const", TokenType::CONST},
        {"declare", TokenType::DECLARE},
        {"lib", TokenType::LIB},
        {"ffi", TokenType::FFI},
        {"from", TokenType::FROM},
        {"import", TokenType::IMPORT},
        {"mod", TokenType::MODULO},
        {"null", TokenType::NULL_LITERAL},
        {"vec2", TokenType::VEC2},
        {"vec3", TokenType::VEC3},
        {"vec4", TokenType::VEC4},
        {"mat3", TokenType::MAT3},
        {"mat4", TokenType::MAT4},
        {"quat", TokenType::QUAT},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT},
        {"true", TokenType::IDENTIFIER},  // Let these be handled as identifiers for now
        {"false", TokenType::IDENTIFIER}
    };
    
    auto it = keywords.find(text);
    return (it != keywords.end()) ? it->second : TokenType::INVALID;
}

bool Lexer::isAtEnd() const {
    return current >= source.length();
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line, column);
    }
    
    char c = peek();
    int startLine = line;
    int startColumn = column;
    
    // Comments
    if (c == '/') {
        if (peek(1) == '/') {
            // Single-line comment //
            advance(); // Skip first /
            advance(); // Skip second /
            skipComment();
            return nextToken(); // Skip comment and get next token
        } else if (peek(1) == '*') {
            // Block comment /* ... */
            advance(); // Skip /
            advance(); // Skip *
            skipBlockComment();
            return nextToken(); // Skip comment and get next token
        }
        // Otherwise it's a division operator, handle below
    }
    
    // Newlines
    if (c == '\n') {
        advance();
        return Token(TokenType::NEWLINE, "\n", startLine, startColumn);
    }
    
    // Numbers
    if (std::isdigit(c)) {
        return makeNumber();
    }
    
    // Strings
    if (c == '"') {
        return makeString();
    }
    
    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        return makeIdentifier();
    }
    
    // Single-character tokens
    advance();
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", startLine, startColumn);
        case '-': return Token(TokenType::MINUS, "-", startLine, startColumn);
        case '*': return Token(TokenType::MULTIPLY, "*", startLine, startColumn);
        case '/': return Token(TokenType::DIVIDE, "/", startLine, startColumn);
        case '%': return Token(TokenType::MODULO, "%", startLine, startColumn);
        case '^': return Token(TokenType::POWER, "^", startLine, startColumn);
        case '(': return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn);
        case '[': return Token(TokenType::LEFT_BRACKET, "[", startLine, startColumn);
        case ']': return Token(TokenType::RIGHT_BRACKET, "]", startLine, startColumn);
        case '{': return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn);
        case '}': return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn);
        case ',': return Token(TokenType::COMMA, ",", startLine, startColumn);
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case ':': return Token(TokenType::COLON, ":", startLine, startColumn);
        case '.': return Token(TokenType::DOT, ".", startLine, startColumn);
        case '=': 
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQUAL, "==", startLine, startColumn);
            }
            return Token(TokenType::ASSIGN, "=", startLine, startColumn);
        case '<':
            if (peek() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", startLine, startColumn);
            } else if (peek() == '>') {
                advance();
                return Token(TokenType::NOT_EQUAL, "<>", startLine, startColumn);
            }
            return Token(TokenType::LESS_THAN, "<", startLine, startColumn);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", startLine, startColumn);
            }
            return Token(TokenType::GREATER_THAN, ">", startLine, startColumn);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", startLine, startColumn);
            }
            return Token(TokenType::INVALID, "!", startLine, startColumn);
    }
    
    return Token(TokenType::INVALID, std::string(1, c), startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        if (token.type != TokenType::NEWLINE) { // Skip newlines for now
            tokens.push_back(token);
        }
        if (token.type == TokenType::EOF_TOKEN) break;
    }
    
    return tokens;
}

} // namespace rbasic