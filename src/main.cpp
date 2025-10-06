#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "codegen.h"
#include "io_handler.h"
#include "command_builder.h"
#include "terminal.h"
#include "repl.h"

// Future: FFI support will be added here

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <filesystem>

using namespace rbasic;

void printUsage(const std::string& programName) {
    std::cout << "rbasic - BASIC Interpreter and Compiler\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " -i <file.bas> [--io <type>]     # Interpret mode\n";
    std::cout << "  " << programName << " -c <file.bas> [-o <output>]     # Compile mode\n";
    std::cout << "  " << programName << " -r | --repl                    # Interactive REPL mode\n";
    std::cout << "  " << programName << " --help                         # Show this help\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --interpret    Interpret the BASIC program directly\n";
    std::cout << "  -c, --compile      Compile BASIC program to C++ executable\n";
    std::cout << "  -r, --repl         Start interactive REPL (Read-Eval-Print Loop)\n";
    std::cout << "  -o, --output       Specify output filename (compile mode only)\n";
    std::cout << "  --io <type>        I/O handler type: console (default: console)\n";
    std::cout << "  --keep-cpp         Keep generated C++ file (compile mode only)\n";
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

bool compileToExecutable(const std::string& cppFile, const std::string& outputFile, const char* exePath) {
    try {
        CommandBuilder builder;
        
#ifdef _WIN32
        // Windows: Check for bundled MinGW64 first, fallback to MSVC
        std::filesystem::path executablePath = std::filesystem::path(exePath).parent_path();
        std::filesystem::path mingwPath = executablePath / "mingw64" / "bin" / "g++.exe";
        
        bool useMingw = std::filesystem::exists(mingwPath);
        
        if (useMingw) {
            // Use bundled MinGW64 compiler
            std::string mingwCompiler = mingwPath.string();
            builder.compiler(mingwCompiler)
                   .compileFlags({"-std=c++17", "-O2", "-static-libgcc", "-static-libstdc++", "-mconsole", "-fopenmp"})
                   .input(cppFile)
                   .output(outputFile + ".exe")
                   .library("runtime\\librbasic_runtime.a")
                   .linkFlags({"-Wl,--subsystem,console", "-lkernel32", "-luser32", "-lgomp"});
            
            std::cout << "Compiling with bundled MinGW64 (OpenMP enabled)..." << std::endl;
        } else {
            // Fallback to Microsoft Visual C++ compiler
            builder.compiler("cl")
                   .compileFlags({"/EHsc", "/std:c++17", "/openmp"})
                   .input(cppFile)
                   .output(outputFile)
                   .library("runtime\\Release\\rbasic_runtime.lib")
                   .linkFlags({"/SUBSYSTEM:CONSOLE", "kernel32.lib", "user32.lib"});
            
            std::cout << "Compiling with MSVC (OpenMP enabled)..." << std::endl;
        }
#else
        // Linux/Unix: Use g++ compiler
        builder.compiler("g++")
               .compileFlags({"-std=c++17", "-O2", "-fopenmp"})
               .input(cppFile)
               .output(outputFile)
               .library("runtime/librbasic_runtime.a")
               .linkFlags({"-lstdc++fs", "-lgomp"});  // Link with filesystem and OpenMP libraries
        
        std::cout << "Compiling with g++ (OpenMP enabled)..." << std::endl;
#endif
        
        int result = builder.execute();
        
        if (result == 0) {
            std::cout << "Successfully compiled to: " << outputFile << std::endl;
            return true;
        } else {
            std::cerr << "Compilation failed with exit code: " << result << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    // Register terminal cleanup to ensure proper state restoration
    std::atexit([]() {
        rbasic::Terminal::cleanup();
    });
    
    try {
        if (argc < 2) {
            printUsage(argv[0]);
            return 1;
        }
        
        std::string mode;
        std::string inputFile;
        std::string outputFile;
        std::string ioType = "console";
        bool keepCppFile = false;
        
        // Parse command line arguments
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-r" || arg == "--repl") {
                // Handle REPL mode immediately
                REPL repl;
                return repl.run();
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
            } else if (arg == "--keep-cpp") {
                keepCppFile = true;
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
        
        // For compile mode, resolve all imports first
        if (mode == "compile") {
            std::cout << "=== Resolving imports for " << inputFile << " ===\n";
            
            auto importResult = resolveImports(source, inputFile);
            if (!importResult.success) {
                std::cerr << "Import resolution failed: " << importResult.errorMessage << std::endl;
                return 1;
            }
            
            if (!importResult.importedFiles.empty()) {
                std::cout << "Resolved " << importResult.importedFiles.size() << " import(s):\n";
                for (const auto& file : importResult.importedFiles) {
                    std::cout << "  - " << file << "\n";
                }
            }
            
            source = importResult.resolvedSource;
        }
        
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        Parser parser(std::move(tokens));
        auto program = parser.parse();
        
        if (mode == "interpret") {
            std::cout << "=== Interpreting " << inputFile << " ===\n";
            
            // Create appropriate I/O handler
            auto ioHandler = createIOHandler(ioType);
            
            Interpreter interpreter(std::move(ioHandler));
            interpreter.setCurrentFile(inputFile);
            interpreter.interpret(*program);
        } else if (mode == "compile") {
            std::cout << "=== Compiling " << inputFile << " ===\n";
            
            CodeGenerator generator;
            std::string cppCode = generator.generate(*program);
            
            // Write generated C++ code to temporary file
            std::string tempCppFile = "temp_" + std::filesystem::path(inputFile).stem().string() + ".cpp";
            writeFile(tempCppFile, cppCode);
            
            std::cout << "Generated C++ code written to: " << tempCppFile << std::endl;
            
            // Compile to executable
            if (compileToExecutable(tempCppFile, outputFile, argv[0])) {
                // Clean up temporary file unless user wants to keep it
                if (!keepCppFile) {
                    std::filesystem::remove(tempCppFile);
                } else {
                    std::cout << "Generated C++ code preserved in: " << tempCppFile << std::endl;
                }
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