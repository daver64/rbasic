#pragma once

#include "common.h"
#include <string>
#include <vector>

namespace rbasic {

enum class TokenType {
    // Literals
    NUMBER,
    STRING,
    IDENTIFIER,
    
    // Keywords
    VAR,
    IF,
    THEN,
    ELSE,
    END,
    FOR,
    TO,
    NEXT,
    WHILE,
    WEND,
    FUNCTION,
    RETURN,
    SUB,
    GOSUB,
    GOTO,
    DIM,
    STRUCT,
    TYPE,
    AS,
    CALL,
    CONST,
    DECLARE,
    LIB,
    MOD,
    
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    POWER,
    
    // Comparison
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    
    // Assignment
    ASSIGN,
    
    // Logical
    AND,
    OR,
    NOT,
    
    // Punctuation
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    SEMICOLON,
    COLON,
    DOT,
    
    // Special
    NEWLINE,
    EOF_TOKEN,
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l = 1, int c = 1)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t current;
    int line;
    int column;
    
    char peek(int offset = 0) const;
    char advance();
    void skipWhitespace();
    void skipComment();
    void skipBlockComment();
    Token makeNumber();
    Token makeString();
    Token makeIdentifier();
    TokenType getKeywordType(const std::string& text) const;
    
public:
    explicit Lexer(const std::string& source);
    
    std::vector<Token> tokenize();
    Token nextToken();
    bool isAtEnd() const;
};

} // namespace rbasic