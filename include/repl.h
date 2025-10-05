#pragma once

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "terminal.h"
#include <string>
#include <vector>

namespace rbasic {

class REPL {
private:
    Interpreter interpreter;
    std::vector<std::string> history;
    std::string currentMultilineBuffer;
    bool inMultilineMode;
    int lineNumber;
    
    // Helper methods
    void showWelcome();
    void showHelp();
    void processLine(const std::string& line);
    bool isCompleteStatement(const std::string& code);
    void executeBuffer();
    void handleMetaCommand(const std::string& command);
    void listVariables();
    void clearSession();
    void loadFile(const std::string& filename);
    void saveSession(const std::string& filename);
    void showHistory();
    std::string getPrompt() const;
    int countBraces(const std::string& code);
    
public:
    REPL();
    int run();
};

} // namespace rbasic