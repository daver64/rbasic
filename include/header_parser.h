#pragma once

#include "common.h"
#include <string>
#include <vector>
#include <map>

namespace rbasic {
namespace ffi {

// FFI Type Information
struct FFIType {
    std::string name;
    size_t size;
    bool isPointer;
    std::string baseType;
    
    FFIType() : size(0), isPointer(false) {}
    FFIType(const std::string& n, size_t s, bool ptr = false, const std::string& base = "") 
        : name(n), size(s), isPointer(ptr), baseType(base) {}
};

// FFI Struct Definition
struct FFIStruct {
    std::string name;
    std::vector<std::pair<std::string, FFIType>> fields;
    size_t totalSize;
    
    FFIStruct() : totalSize(0) {}
    FFIStruct(const std::string& n) : name(n), totalSize(0) {}
};

// FFI Constant Definition
struct FFIConstant {
    std::string name;
    std::string value;
    std::string type;
    
    FFIConstant() = default;
    FFIConstant(const std::string& n, const std::string& v, const std::string& t = "int") 
        : name(n), value(v), type(t) {}
};

// Header Parser for automatic FFI definitions
class HeaderParser {
private:
    std::map<std::string, FFIStruct> structs;
    std::map<std::string, FFIConstant> constants;
    std::map<std::string, FFIType> types;
    
    void initializeBuiltinTypes();
    void parseDefine(const std::string& line);
    void parseStruct(const std::string& content, size_t& pos);
    void parseTypedef(const std::string& line);
    std::string extractIdentifier(const std::string& content, size_t& pos);
    std::string extractValue(const std::string& content, size_t& pos);
    void skipWhitespace(const std::string& content, size_t& pos);
    
public:
    HeaderParser();
    
    // Parse header file and extract FFI definitions
    bool parseHeaderFile(const std::string& headerPath);
    bool parseHeaderContent(const std::string& content);
    
    // Generate rbasic FFI definitions
    std::string generateFFIDefinitions();
    
    // Access parsed data
    const std::map<std::string, FFIStruct>& getStructs() const { return structs; }
    const std::map<std::string, FFIConstant>& getConstants() const { return constants; }
    const std::map<std::string, FFIType>& getTypes() const { return types; }
    
    // Add predefined SDL2 definitions
    void addSDL2Definitions();
};

} // namespace ffi
} // namespace rbasic