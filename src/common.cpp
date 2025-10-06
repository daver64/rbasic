#include "common.h"
#include <sstream>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <set>
#include <algorithm>
#include <functional>

namespace rbasic {

std::string valueToString(const ValueType& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<ArrayValue>(value)) {
        return "[Array]";  // Simple representation for now
    } else if (std::holds_alternative<StructValue>(value)) {
        const auto& structVal = std::get<StructValue>(value);
        return "[" + structVal.typeName + " struct]";  // Simple representation for now
    }
    return "";
}

bool isTruthy(const ValueType& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    } else if (std::holds_alternative<ArrayValue>(value)) {
        return !std::get<ArrayValue>(value).elements.empty();
    }
    return false;
}

ValueType addValues(const ValueType& left, const ValueType& right) {
    // String concatenation
    if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
        return valueToString(left) + valueToString(right);
    }
    
    // Numeric addition
    bool hasDouble = std::holds_alternative<double>(left) || std::holds_alternative<double>(right);
    
    if (hasDouble) {
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
        return leftVal + rightVal;
    } else {
        int leftVal = std::holds_alternative<int>(left) ? std::get<int>(left) : 0;
        int rightVal = std::holds_alternative<int>(right) ? std::get<int>(right) : 0;
        return leftVal + rightVal;
    }
}

ValueType subtractValues(const ValueType& left, const ValueType& right) {
    bool hasDouble = std::holds_alternative<double>(left) || std::holds_alternative<double>(right);
    
    if (hasDouble) {
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
        return leftVal - rightVal;
    } else {
        int leftVal = std::holds_alternative<int>(left) ? std::get<int>(left) : 0;
        int rightVal = std::holds_alternative<int>(right) ? std::get<int>(right) : 0;
        return leftVal - rightVal;
    }
}

ValueType multiplyValues(const ValueType& left, const ValueType& right) {
    bool hasDouble = std::holds_alternative<double>(left) || std::holds_alternative<double>(right);
    
    if (hasDouble) {
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
        return leftVal * rightVal;
    } else {
        int leftVal = std::holds_alternative<int>(left) ? std::get<int>(left) : 0;
        int rightVal = std::holds_alternative<int>(right) ? std::get<int>(right) : 0;
        return leftVal * rightVal;
    }
}

ValueType divideValues(const ValueType& left, const ValueType& right) {
    double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                    std::holds_alternative<int>(left) ? static_cast<double>(std::get<int>(left)) : 0.0;
    double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                     std::holds_alternative<int>(right) ? static_cast<double>(std::get<int>(right)) : 0.0;
    
    if (rightVal == 0.0) {
        throw RuntimeError("Division by zero");
    }
    
    return leftVal / rightVal;
}

ValueType compareValues(const ValueType& left, const ValueType& right, const std::string& op) {
    // For numeric comparison
    if ((std::holds_alternative<int>(left) || std::holds_alternative<double>(left)) &&
        (std::holds_alternative<int>(right) || std::holds_alternative<double>(right))) {
        
        double leftVal = std::holds_alternative<double>(left) ? std::get<double>(left) : 
                        static_cast<double>(std::get<int>(left));
        double rightVal = std::holds_alternative<double>(right) ? std::get<double>(right) : 
                         static_cast<double>(std::get<int>(right));
        
        if (op == "==") return leftVal == rightVal;
        if (op == "!=") return leftVal != rightVal;
        if (op == "<") return leftVal < rightVal;
        if (op == "<=") return leftVal <= rightVal;
        if (op == ">") return leftVal > rightVal;
        if (op == ">=") return leftVal >= rightVal;
    }
    
    // For string comparison
    std::string leftStr = valueToString(left);
    std::string rightStr = valueToString(right);
    
    if (op == "==") return leftStr == rightStr;
    if (op == "!=") return leftStr != rightStr;
    if (op == "<") return leftStr < rightStr;
    if (op == "<=") return leftStr <= rightStr;
    if (op == ">") return leftStr > rightStr;
    if (op == ">=") return leftStr >= rightStr;
    
    return false;
}

// Import resolution implementation
std::string resolveImportPath(const std::string& filename, const std::string& currentFile) {
    // Same path resolution logic as interpreter
    std::vector<std::string> searchPaths;
    
    // 1. Relative to current file directory (if we have one)
    if (!currentFile.empty()) {
        std::filesystem::path currentPath(currentFile);
        if (currentPath.has_parent_path()) {
            searchPaths.push_back(currentPath.parent_path().string());
        }
    }
    
    // 2. Current working directory
    searchPaths.push_back(".");
    
    // 3. Executable directory
    try {
        auto execPath = std::filesystem::canonical("/proc/self/exe");
        if (execPath.has_parent_path()) {
            searchPaths.push_back(execPath.parent_path().string());
        }
    } catch (...) {
        // Fallback for systems without /proc/self/exe
        searchPaths.push_back(".");
    }
    
    // 4. Common library paths
    searchPaths.push_back("lib");
    searchPaths.push_back("stdlib");
    searchPaths.push_back("library");
    
    // Try each search path
    for (const auto& searchPath : searchPaths) {
        std::filesystem::path fullPath = std::filesystem::path(searchPath) / filename;
        if (std::filesystem::exists(fullPath)) {
            return std::filesystem::canonical(fullPath).string();
        }
    }
    
    return ""; // Not found
}

ImportResolutionResult resolveImports(const std::string& source, const std::string& baseFile) {
    ImportResolutionResult result(true);
    std::set<std::string> processedFiles;
    std::vector<std::string> fileStack; // For circular import detection
    
    std::function<bool(const std::string&, const std::string&, std::string&)> processFile = 
        [&](const std::string& content, const std::string& currentFile, std::string& output) -> bool {
        
        // Canonicalize current file path for proper tracking
        std::string canonicalCurrentFile;
        if (!currentFile.empty()) {
            try {
                canonicalCurrentFile = std::filesystem::canonical(currentFile).string();
            } catch (...) {
                canonicalCurrentFile = currentFile;
            }
            
            // Check for circular imports
            if (std::find(fileStack.begin(), fileStack.end(), canonicalCurrentFile) != fileStack.end()) {
                result.errorMessage = "Circular import detected: " + canonicalCurrentFile;
                result.success = false;
                return false;
            }
            fileStack.push_back(canonicalCurrentFile);
        }
        
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // Simple import detection - look for 'import "filename";'
            std::string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));
            trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
            
            if (trimmed.length() > 7 && trimmed.substr(0, 6) == "import") {
                // Parse import statement: import "filename";
                size_t firstQuote = trimmed.find('"');
                size_t lastQuote = trimmed.rfind('"');
                
                if (firstQuote != std::string::npos && lastQuote != std::string::npos && 
                    firstQuote < lastQuote) {
                    
                    std::string importFile = trimmed.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    std::string resolvedPath = resolveImportPath(importFile, currentFile);
                    
                    if (resolvedPath.empty()) {
                        result.errorMessage = "Import file not found: " + importFile + 
                                           " (at " + currentFile + ":" + std::to_string(lineNumber) + ")";
                        result.success = false;
                        return false;
                    }
                    
                    // Canonicalize path to avoid duplicate imports
                    std::string canonicalPath;
                    try {
                        canonicalPath = std::filesystem::canonical(resolvedPath).string();
                    } catch (...) {
                        canonicalPath = resolvedPath;
                    }
                    
                    // Skip if already processed
                    if (processedFiles.find(canonicalPath) != processedFiles.end()) {
                        output += "// " + line + " (already imported)\n";
                        continue;
                    }
                    
                    // Mark as processed and add to imported files list
                    processedFiles.insert(canonicalPath);
                    result.importedFiles.push_back(canonicalPath);
                    
                    // Read and process the imported file
                    std::ifstream importStream(canonicalPath);
                    if (!importStream.is_open()) {
                        result.errorMessage = "Failed to read import file: " + canonicalPath;
                        result.success = false;
                        return false;
                    }
                    
                    std::string importContent((std::istreambuf_iterator<char>(importStream)),
                                            std::istreambuf_iterator<char>());
                    importStream.close();
                    
                    // Add comment indicating the imported file
                    output += "// === BEGIN IMPORT: " + importFile + " ===\n";
                    
                    // Recursively process the imported file
                    if (!processFile(importContent, canonicalPath, output)) {
                        return false;
                    }
                    
                    output += "// === END IMPORT: " + importFile + " ===\n";
                } else {
                    result.errorMessage = "Invalid import syntax: " + line + 
                                       " (at " + currentFile + ":" + std::to_string(lineNumber) + ")";
                    result.success = false;
                    return false;
                }
            } else {
                // Regular line, copy as-is
                output += line + "\n";
            }
        }
        
        // Remove current file from processing stack
        if (!canonicalCurrentFile.empty()) {
            fileStack.pop_back();
        }
        
        return true;
    };
    
    // Process the main file
    if (!processFile(source, baseFile, result.resolvedSource)) {
        result.success = false;
    }
    
    return result;
}

} // namespace rbasic