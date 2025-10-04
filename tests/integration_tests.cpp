#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/io_handler.h"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace rbasic;

void test_integration() {
    // Test simple variable assignment and arithmetic
    {
        std::string code = "var x = 10; var y = x + 5; print y;";
        
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        // Redirect cout to test output
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());
        
        Interpreter interpreter(createIOHandler("console"));
        interpreter.interpret(*program);
        
        // Restore cout
        std::cout.rdbuf(old_cout);
        
        assert(output.str() == "15\n");
    }
    
    // Test conditional logic
    {
        std::string code = R"(
            var x = 5;
            if x > 3 then
                print "greater";
            else
                print "not greater";
            end if;
        )";
        
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());
        
        Interpreter interpreter(createIOHandler("console"));
        interpreter.interpret(*program);
        
        std::cout.rdbuf(old_cout);
        
        assert(output.str() == "greater\n");
    }
    
    // Test string operations
    {
        std::string code = R"(
            var name = "World";
            var greeting = "Hello, " + name + "!";
            print greeting;
        )";
        
        Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        std::ostringstream output;
        std::streambuf* old_cout = std::cout.rdbuf(output.rdbuf());
        
        Interpreter interpreter(createIOHandler("console"));
        interpreter.interpret(*program);
        
        std::cout.rdbuf(old_cout);
        
        assert(output.str() == "Hello, World!\n");
    }
}