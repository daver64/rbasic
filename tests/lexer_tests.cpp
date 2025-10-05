#include "../include/lexer.h"
#include <cassert>
#include <iostream>

using namespace rbasic;

void test_lexer() {
    // Test basic tokenization
    {
        Lexer lexer("var x = 42;");
        auto tokens = lexer.tokenize();
        
        assert(tokens.size() == 5); // var, x, =, 42, ;, EOF
        assert(tokens[0].type == TokenType::VAR);
        assert(tokens[1].type == TokenType::IDENTIFIER);
        assert(tokens[1].value == "x");
        assert(tokens[2].type == TokenType::ASSIGN);
        assert(tokens[3].type == TokenType::NUMBER);
        assert(tokens[3].value == "42");
        assert(tokens[4].type == TokenType::SEMICOLON);
    }
    
    // Test string literals  
    {
        Lexer lexer("print(\"Hello, World!\");");
        auto tokens = lexer.tokenize();
        
        assert(tokens.size() == 6); // print, (, "Hello, World!", ), ;, EOF
        assert(tokens[0].type == TokenType::PRINT);
        assert(tokens[1].type == TokenType::LPAREN);
        assert(tokens[2].type == TokenType::STRING);
        assert(tokens[2].value == "Hello, World!");
        assert(tokens[3].type == TokenType::RPAREN);
        assert(tokens[4].type == TokenType::SEMICOLON);
    }
    
    // Test keywords (lowercase as per current C-style syntax)
    {
        Lexer lexer("if var for while function");
        auto tokens = lexer.tokenize();
        
        assert(tokens[0].type == TokenType::IF);
        assert(tokens[1].type == TokenType::VAR);
        assert(tokens[2].type == TokenType::FOR);
        assert(tokens[3].type == TokenType::WHILE);
        assert(tokens[4].type == TokenType::FUNCTION);
    }
    
    // Test operators
    {
        Lexer lexer("+ - * / = <> < <= > >=");
        auto tokens = lexer.tokenize();
        
        assert(tokens[0].type == TokenType::PLUS);
        assert(tokens[1].type == TokenType::MINUS);
        assert(tokens[2].type == TokenType::MULTIPLY);
        assert(tokens[3].type == TokenType::DIVIDE);
        assert(tokens[4].type == TokenType::ASSIGN);
        assert(tokens[5].type == TokenType::NOT_EQUAL);
        assert(tokens[6].type == TokenType::LESS_THAN);
        assert(tokens[7].type == TokenType::LESS_EQUAL);
        assert(tokens[8].type == TokenType::GREATER_THAN);
        assert(tokens[9].type == TokenType::GREATER_EQUAL);
    }
}