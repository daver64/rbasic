#include "repl.h"
#include "common.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace rbasic {

// Helper function to remove quotes from filename
std::string removeQuotes(const std::string& str) {
    std::string result = str;
    if (result.size() >= 2 && 
        ((result.front() == '"' && result.back() == '"') ||
         (result.front() == '\'' && result.back() == '\''))) {
        result = result.substr(1, result.size() - 2);
    }
    return result;
}

REPL::REPL() : inMultilineMode(false), lineNumber(1) {
}

int REPL::run() {
    Terminal::initialize();
    showWelcome();
    
    std::string line;
    while (true) {
        std::string prompt = getPrompt();
        
        // Use terminal getline for better input handling
        line = Terminal::getline(prompt, Colour::CYAN);
        
        // Handle Ctrl+D (empty input on EOF)
        if (std::cin.eof()) {
            std::cout << "\nGoodbye!\n";
            break;
        }
        
        // Skip empty lines in normal mode
        if (line.empty() && !inMultilineMode) {
            continue;
        }
        
        // Add to history (except meta commands)
        if (!line.empty() && line.size() > 0 && line[0] != ':') {
            history.push_back(line);
        }
        
        processLine(line);
    }
    
    Terminal::cleanup();
    return 0;
}

void REPL::showWelcome() {
    Terminal::println("rbasic Interactive REPL v1.0.0", Colour::BRIGHT_GREEN);
    Terminal::println("Type :help for commands, :quit to exit", Colour::DEFAULT);
    Terminal::println("");
}

void REPL::showHelp() {
    Terminal::println("REPL Commands:", Colour::BRIGHT_YELLOW);
    Terminal::println("  :help                 - Show this help", Colour::DEFAULT);
    Terminal::println("  :list                 - List all defined variables and functions", Colour::DEFAULT);
    Terminal::println("  :clear                - Clear all variables and functions", Colour::DEFAULT);
    Terminal::println("  :load <file>          - Load and execute a .bas file", Colour::DEFAULT);
    Terminal::println("  :save <file>          - Save current session to file", Colour::DEFAULT);
    Terminal::println("  :history              - Show command history", Colour::DEFAULT);
    Terminal::println("  :quit or :exit        - Exit REPL", Colour::DEFAULT);
    Terminal::println("");
    Terminal::println("Code Execution:", Colour::BRIGHT_YELLOW);
    Terminal::println("  - Single line statements execute immediately", Colour::DEFAULT);
    Terminal::println("  - Multi-line blocks (functions, if, for, while) auto-detect completion", Colour::DEFAULT);
    Terminal::println("  - Use empty line to force execution of incomplete multi-line block", Colour::DEFAULT);
    Terminal::println("");
}

std::string REPL::getPrompt() const {
    if (inMultilineMode) {
        return "     ... | ";
    } else {
        return "rbasic> ";
    }
}

void REPL::processLine(const std::string& line) {
    // Handle meta commands
    if (!line.empty() && line[0] == ':') {
        handleMetaCommand(line);
        return;
    }
    
    // Add line to multiline buffer
    if (!currentMultilineBuffer.empty()) {
        currentMultilineBuffer += "\n";
    }
    currentMultilineBuffer += line;
    
    // Check if statement is complete or if user wants to force execution
    if (line.empty() || isCompleteStatement(currentMultilineBuffer)) {
        if (!currentMultilineBuffer.empty()) {
            executeBuffer();
        }
        currentMultilineBuffer.clear();
        inMultilineMode = false;
    } else {
        inMultilineMode = true;
    }
}

bool REPL::isCompleteStatement(const std::string& code) {
    // Simple brace counting for now
    int braceCount = countBraces(code);
    
    // If braces are balanced, statement might be complete
    if (braceCount == 0) {
        // Check for keywords that typically start multi-line blocks
        std::string trimmed = code;
        // Remove leading whitespace
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        
        // If line ends with certain keywords, it's likely incomplete
        if (trimmed.length() >= 8 && trimmed.substr(trimmed.length() - 8) == "function") return false;
        if (trimmed.length() >= 6 && trimmed.substr(trimmed.length() - 6) == "struct") return false;
        if (trimmed.length() >= 2 && trimmed.substr(trimmed.length() - 2) == "if") return false;
        if (trimmed.length() >= 4 && trimmed.substr(trimmed.length() - 4) == "else") return false;
        if (trimmed.length() >= 3 && trimmed.substr(trimmed.length() - 3) == "for") return false;
        if (trimmed.length() >= 5 && trimmed.substr(trimmed.length() - 5) == "while") return false;
        if (trimmed.length() >= 1 && trimmed.substr(trimmed.length() - 1) == "{") return false;
        
        return true;
    }
    
    return false; // Unmatched braces
}

int REPL::countBraces(const std::string& code) {
    int count = 0;
    bool inString = false;
    bool inComment = false;
    
    for (size_t i = 0; i < code.length(); i++) {
        char c = code[i];
        
        // Handle string literals
        if (c == '"' && !inComment) {
            if (i == 0 || code[i-1] != '\\') {
                inString = !inString;
            }
            continue;
        }
        
        if (inString) continue;
        
        // Handle comments
        if (c == '/' && i + 1 < code.length()) {
            if (code[i + 1] == '/') {
                // Line comment - skip to end of line
                while (i < code.length() && code[i] != '\n') i++;
                continue;
            } else if (code[i + 1] == '*') {
                // Block comment
                inComment = true;
                i++; // Skip the '*'
                continue;
            }
        }
        
        if (inComment) {
            if (c == '*' && i + 1 < code.length() && code[i + 1] == '/') {
                inComment = false;
                i++; // Skip the '/'
            }
            continue;
        }
        
        // Count braces
        if (c == '{') count++;
        else if (c == '}') count--;
    }
    
    return count;
}

void REPL::executeBuffer() {
    try {
        // Create temporary program from buffer
        Lexer lexer(currentMultilineBuffer);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        
        // Execute in existing interpreter context
        interpreter.interpret(*program);
        
    } catch (const SyntaxError& e) {
        Terminal::println("Syntax Error: " + std::string(e.what()), Colour::RED);
    } catch (const RuntimeError& e) {
        Terminal::println("Runtime Error: " + std::string(e.what()), Colour::RED);
    } catch (const std::exception& e) {
        Terminal::println("Error: " + std::string(e.what()), Colour::RED);
    }
}

void REPL::handleMetaCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == ":help" || cmd == ":h") {
        showHelp();
    } else if (cmd == ":quit" || cmd == ":exit" || cmd == ":q") {
        std::cout << "Goodbye!\n";
        std::exit(0);
    } else if (cmd == ":list" || cmd == ":ls") {
        listVariables();
    } else if (cmd == ":clear") {
        clearSession();
    } else if (cmd == ":history" || cmd == ":hist") {
        showHistory();
    } else if (cmd == ":load") {
        std::string filename;
        iss >> filename;
        if (!filename.empty()) {
            filename = removeQuotes(filename);
            loadFile(filename);
        } else {
            Terminal::println("Usage: :load <filename>", Colour::YELLOW);
        }
    } else if (cmd == ":save") {
        std::string filename;
        iss >> filename;
        if (!filename.empty()) {
            filename = removeQuotes(filename);
            saveSession(filename);
        } else {
            Terminal::println("Usage: :save <filename>", Colour::YELLOW);
        }
    } else {
        Terminal::println("Unknown command: " + cmd, Colour::RED);
        Terminal::println("Type :help for available commands", Colour::DEFAULT);
    }
}

void REPL::listVariables() {
    Terminal::println("Current Session State:", Colour::BRIGHT_YELLOW);
    Terminal::println("(Note: Variable inspection not yet implemented)", Colour::YELLOW);
    Terminal::println("Use the interpreter's built-in variable system", Colour::DEFAULT);
    Terminal::println("");
}

void REPL::clearSession() {
    // Create a new interpreter instance to clear all state
    interpreter = Interpreter();
    currentMultilineBuffer.clear();
    inMultilineMode = false;
    Terminal::println("Session cleared - all variables and functions removed", Colour::GREEN);
}

void REPL::loadFile(const std::string& filename) {
    // Try to resolve the file path
    std::string resolvedPath = filename;
    
    // If not an absolute path, try different search locations
    if (!std::filesystem::path(filename).is_absolute()) {
        // Try current working directory first
        if (std::filesystem::exists(filename)) {
            resolvedPath = filename;
        }
        // Try relative to examples directory
        else {
            std::filesystem::path examplesPath = std::filesystem::current_path() / "examples" / filename;
            if (std::filesystem::exists(examplesPath)) {
                resolvedPath = examplesPath.string();
            }
        }
    }
    
    std::ifstream file(resolvedPath);
    if (!file.is_open()) {
        Terminal::println("Error: Could not open file '" + filename + "'", Colour::RED);
        if (resolvedPath != filename) {
            Terminal::println("  Tried: " + resolvedPath, Colour::YELLOW);
        }
        return;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    try {
        Lexer lexer(content);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto program = parser.parse();
        
        interpreter.interpret(*program);
        Terminal::println("Loaded and executed: " + resolvedPath, Colour::GREEN);
        
    } catch (const std::exception& e) {
        Terminal::println("Error loading '" + filename + "': " + e.what(), Colour::RED);
    }
}

void REPL::saveSession(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Terminal::println("Error: Could not create file '" + filename + "'", Colour::RED);
        return;
    }
    
    file << "// rbasic REPL session saved on " << __DATE__ << "\n\n";
    
    for (const auto& line : history) {
        file << line << "\n";
    }
    
    file.close();
    Terminal::println("Session saved to: " + filename, Colour::GREEN);
}

void REPL::showHistory() {
    if (history.empty()) {
        Terminal::println("No command history", Colour::YELLOW);
        return;
    }
    
    Terminal::println("Command History:", Colour::BRIGHT_YELLOW);
    for (size_t i = 0; i < history.size(); i++) {
        std::cout << "  " << (i + 1) << ": " << history[i] << "\n";
    }
    std::cout << "\n";
}

} // namespace rbasic