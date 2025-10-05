#include "command_builder.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace rbasic {

CommandBuilder& CommandBuilder::compiler(const std::string& compiler) {
    compiler_ = compiler;
    return *this;
}

CommandBuilder& CommandBuilder::compileFlag(const std::string& flag) {
    compileFlags_.push_back(flag);
    return *this;
}

CommandBuilder& CommandBuilder::compileFlags(const std::vector<std::string>& flags) {
    compileFlags_.insert(compileFlags_.end(), flags.begin(), flags.end());
    return *this;
}

CommandBuilder& CommandBuilder::linkFlag(const std::string& flag) {
    linkFlags_.push_back(flag);
    return *this;
}

CommandBuilder& CommandBuilder::linkFlags(const std::vector<std::string>& flags) {
    linkFlags_.insert(linkFlags_.end(), flags.begin(), flags.end());
    return *this;
}

CommandBuilder& CommandBuilder::input(const std::string& inputFile) {
    inputFile_ = inputFile;
    return *this;
}

CommandBuilder& CommandBuilder::output(const std::string& outputFile) {
    outputFile_ = outputFile;
    return *this;
}

CommandBuilder& CommandBuilder::library(const std::string& library) {
    libraries_.push_back(library);
    return *this;
}

std::string CommandBuilder::escapeArgument(const std::string& arg) const {
    if (arg.empty()) {
        return "\"\"";
    }
    
    // Check if escaping is needed
    bool needsQuotes = false;
    for (char c : arg) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '"') {
            needsQuotes = true;
            break;
        }
    }
    
    if (!needsQuotes) {
        return arg;
    }
    
    // Escape the argument
    std::string escaped = "\"";
    for (char c : arg) {
        if (c == '"' || c == '\\') {
            escaped += '\\';
        }
        escaped += c;
    }
    escaped += "\"";
    
    return escaped;
}

bool CommandBuilder::isValidPath(const std::string& path) const {
    // Basic path validation - reject dangerous characters
    const std::string dangerous = "|&;()<>`$";
    return path.find_first_of(dangerous) == std::string::npos;
}

bool CommandBuilder::isGccLikeCompiler() const {
    // Check if the compiler is GCC, G++, MinGW, or Clang
    std::string compilerName = compiler_;
    std::transform(compilerName.begin(), compilerName.end(), compilerName.begin(), 
                  [](char c) { return static_cast<char>(std::tolower(c)); });
    
    return compilerName.find("gcc") != std::string::npos ||
           compilerName.find("g++") != std::string::npos ||
           compilerName.find("clang") != std::string::npos ||
           compilerName.find("mingw") != std::string::npos;
}

std::string CommandBuilder::build() const {
    if (compiler_.empty()) {
        throw std::runtime_error("Compiler not specified");
    }
    
    // Validate all paths
    if (!inputFile_.empty() && !isValidPath(inputFile_)) {
        throw std::runtime_error("Invalid input file path");
    }
    if (!outputFile_.empty() && !isValidPath(outputFile_)) {
        throw std::runtime_error("Invalid output file path");
    }
    
    std::ostringstream cmd;
    cmd << escapeArgument(compiler_);
    
    // Add compile flags
    for (const auto& flag : compileFlags_) {
        cmd << " " << escapeArgument(flag);
    }
    
    // Add input file
    if (!inputFile_.empty()) {
        cmd << " " << escapeArgument(inputFile_);
    }
    
    // Add output specification
    if (!outputFile_.empty()) {
        if (isGccLikeCompiler()) {
            // Use GCC/MinGW syntax
            cmd << " -o " << escapeArgument(outputFile_);
        } else {
#ifdef _WIN32
            // Use MSVC syntax
            cmd << " /Fe:" << escapeArgument(outputFile_);
#else
            // Default to GCC syntax on non-Windows
            cmd << " -o " << escapeArgument(outputFile_);
#endif
        }
    }
    
    // Add libraries
    for (const auto& lib : libraries_) {
        if (!isValidPath(lib)) {
            throw std::runtime_error("Invalid library path: " + lib);
        }
        cmd << " " << escapeArgument(lib);
    }
    
    // Add link flags
    if (!linkFlags_.empty()) {
        if (isGccLikeCompiler()) {
            // Use GCC/MinGW syntax - link flags come directly
            for (const auto& flag : linkFlags_) {
                cmd << " " << escapeArgument(flag);
            }
        } else {
#ifdef _WIN32
            // Use MSVC syntax - link flags after /link
            cmd << " /link";
            for (const auto& flag : linkFlags_) {
                cmd << " " << escapeArgument(flag);
            }
#else
            // Default to GCC syntax on non-Windows
            for (const auto& flag : linkFlags_) {
                cmd << " " << escapeArgument(flag);
            }
#endif
        }
    }
    
    return cmd.str();
}

int CommandBuilder::execute() const {
    std::string command = build();
    std::cout << "Executing: " << command << std::endl;
    
#ifdef _WIN32
    // Use CreateProcess for better security on Windows
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    
    // CreateProcess modifies the command line, so we need a mutable copy
    std::string mutableCommand = command;
    
    BOOL success = CreateProcessA(
        nullptr,                    // Application name
        &mutableCommand[0],         // Command line
        nullptr,                    // Process security attributes
        nullptr,                    // Thread security attributes
        FALSE,                      // Inherit handles
        0,                          // Creation flags
        nullptr,                    // Environment
        nullptr,                    // Current directory
        &si,                        // Startup info
        &pi                         // Process info
    );
    
    if (!success) {
        return -1;
    }
    
    // Wait for the process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return static_cast<int>(exitCode);
#else
    // On Unix, we'll still use system() but with better validation
    // In a production system, you'd want to use exec() family functions
    return std::system(command.c_str());
#endif
}

} // namespace rbasic