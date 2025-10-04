#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "codegen.h"
#include "io_handler.h"

#ifdef RBASIC_SQLITE_SUPPORT
#include "sqlite_handler.h"
#endif

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace rbasic;

void printUsage(const std::string& programName) {
    std::cout << "rbasic - BASIC Interpreter and Compiler\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " -i <file.bas> [--io <type>]     # Interpret mode\n";
    std::cout << "  " << programName << " -c <file.bas> [-o <output>]     # Compile mode\n";
    std::cout << "  " << programName << " --help                         # Show this help\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --interpret    Interpret the BASIC program directly\n";
    std::cout << "  -c, --compile      Compile BASIC program to C++ executable\n";
    std::cout << "  -o, --output       Specify output filename (compile mode only)\n";
    std::cout << "  --io <type>        I/O handler type: console, sdl (default: console)\n";
    std::cout << "  --help             Show this help message\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not create file: " + filename);
    }
    file << content;
}

bool compileToExecutable(const std::string& cppFile, const std::string& outputFile, bool usesSDL = false, bool usesSQLite = false) {
    // Use cl compiler directly since it's set up globally
    std::string command = "cl /EHsc /std:c++17 \"" + cppFile + "\" /Fe:\"" + outputFile + "\" runtime\\Release\\rbasic_runtime.lib";
    
    // If runtime library was built with SDL support, we must always link SDL libraries
    // but only add SDL compilation flags if the program actually uses graphics
#ifdef RBASIC_SDL_SUPPORT
    const char* sdl2_root = std::getenv("SDL2_ROOT");
    if (sdl2_root) {
        // Always link SDL libraries (runtime needs them)
        command += " \"" + std::string(sdl2_root) + "\\lib\\x64\\SDL2.lib\"";
        
        // Only add SDL compilation flags and main if program uses graphics
        if (usesSDL) {
            command += " /I\"" + std::string(sdl2_root) + "\\include\"";
            command += " \"" + std::string(sdl2_root) + "\\lib\\x64\\SDL2main.lib\"";
            command += " /DRBASIC_SDL_SUPPORT";
        }
    }
#endif
    
    // Add SQLite support if the program uses database functions
    if (usesSQLite) {
#ifdef RBASIC_SQLITE_SUPPORT
        // SQLite is compiled directly from source, just add the source file
        command += " \"3rd_party\\sqlite\\sqlite3.c\"";
        command += " /I\"3rd_party\\sqlite\"";
        command += " /DRBASIC_SQLITE_SUPPORT";
#endif
    }
    
    // Add linker flags at the end
    command += " /link /SUBSYSTEM:CONSOLE kernel32.lib user32.lib";
    
    std::cout << "Compiling with MSVC: " << command << std::endl;
    int result = std::system(command.c_str());
    
    if (result == 0) {
        std::cout << "Successfully compiled to: " << outputFile << std::endl;
        return true;
    } else {
        std::cerr << "Compilation failed with exit code: " << result << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            printUsage(argv[0]);
            return 1;
        }
        
        std::string mode;
        std::string inputFile;
        std::string outputFile;
        std::string ioType = "console";
        
        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-i" || arg == "--interpret") {
                mode = "interpret";
                if (i + 1 < argc) {
                    inputFile = argv[++i];
                }
            } else if (arg == "-c" || arg == "--compile") {
                mode = "compile";
                if (i + 1 < argc) {
                    inputFile = argv[++i];
                }
            } else if (arg == "-o" || arg == "--output") {
                if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg == "--io") {
                if (i + 1 < argc) {
                    ioType = argv[++i];
                }
            } else if (inputFile.empty()) {
                inputFile = arg;
                if (mode.empty()) {
                    mode = "interpret"; // Default mode
                }
            }
        }
        
        if (inputFile.empty()) {
            std::cerr << "Error: No input file specified.\n";
            printUsage(argv[0]);
            return 1;
        }
        
        // Set default output file for compile mode
        if (mode == "compile" && outputFile.empty()) {
            std::filesystem::path path(inputFile);
            outputFile = path.stem().string();
#ifdef _WIN32
            outputFile += ".exe";
#endif
        }
        
        // Read and parse the BASIC program
        std::string source = readFile(inputFile);
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        if (mode == "interpret") {
            std::cout << "=== Interpreting " << inputFile << " ===\n";
            
#ifdef RBASIC_SQLITE_SUPPORT
            // Initialize SQLite handler for interpreted mode
            initializeSQLiteHandler();
#endif
            
            // Create appropriate I/O handler
            auto ioHandler = createIOHandler(ioType);
            
            Interpreter interpreter(std::move(ioHandler));
            interpreter.interpret(*program);
            
#ifdef RBASIC_SQLITE_SUPPORT
            // Cleanup SQLite handler
            cleanupSQLiteHandler();
#endif
        } else if (mode == "compile") {
            std::cout << "=== Compiling " << inputFile << " ===\n";
            
            CodeGenerator generator;
            std::string cppCode = generator.generate(*program);
            bool usesSDL = generator.getUsesSDL();
            bool usesSQLite = generator.getUsesSQLite();
            
            // Write generated C++ code to temporary file
            std::string tempCppFile = "temp_" + std::filesystem::path(inputFile).stem().string() + ".cpp";
            writeFile(tempCppFile, cppCode);
            
            std::cout << "Generated C++ code written to: " << tempCppFile << std::endl;
            
            // Compile to executable
            if (compileToExecutable(tempCppFile, outputFile, usesSDL, usesSQLite)) {
                // Clean up temporary file
                std::filesystem::remove(tempCppFile);
            } else {
                std::cerr << "Compilation failed. Generated C++ code preserved in: " << tempCppFile << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown mode. Use -i for interpret or -c for compile.\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}