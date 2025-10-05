#include "../include/parser.h"
#include "../include/lexer.h"
#include <cassert>
#include <iostream>

using namespace rbasic;

void test_parser() {
    // Test variable declaration
    {
        Lexer lexer("var x = 42;");
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        assert(program->statements.size() == 1);
        auto varStmt = dynamic_cast<VarStmt*>(program->statements[0].get());
        assert(varStmt != nullptr);
        assert(varStmt->variable == "x");
    }
    
    // Test print statement
    {
        Lexer lexer("print(\"Hello\");");
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        assert(program->statements.size() == 1);
        auto printStmt = dynamic_cast<PrintStmt*>(program->statements[0].get());
        assert(printStmt != nullptr);
        assert(printStmt->expressions.size() == 1);
    }
    
    // Test binary expression
    {
        Lexer lexer("var result = 2 + 3 * 4;");
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        assert(program->statements.size() == 1);
        auto varStmt = dynamic_cast<VarStmt*>(program->statements[0].get());
        assert(varStmt != nullptr);
        
        auto binaryExpr = dynamic_cast<BinaryExpr*>(varStmt->value.get());
        assert(binaryExpr != nullptr);
        assert(binaryExpr->operator_ == "+");
    }
    
    // Test if statement  
    {
        Lexer lexer("if (x > 0) { print(\"positive\"); }");
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        assert(program->statements.size() == 1);
        auto ifStmt = dynamic_cast<IfStmt*>(program->statements[0].get());
        assert(ifStmt != nullptr);
    }
}