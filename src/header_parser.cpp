#include "header_parser.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <cctype>

namespace rbasic {
namespace ffi {

HeaderParser::HeaderParser() {
    initializeBuiltinTypes();
}

void HeaderParser::initializeBuiltinTypes() {
    // Basic C types
    types["int"] = FFIType("int", sizeof(int));
    types["uint32_t"] = FFIType("uint32_t", sizeof(uint32_t));
    types["uint16_t"] = FFIType("uint16_t", sizeof(uint16_t));
    types["uint8_t"] = FFIType("uint8_t", sizeof(uint8_t));
    types["char"] = FFIType("char", sizeof(char));
    types["float"] = FFIType("float", sizeof(float));
    types["double"] = FFIType("double", sizeof(double));
    types["void"] = FFIType("void", 0);
    
    // Pointer types
    types["void*"] = FFIType("void*", sizeof(void*), true, "void");
    types["char*"] = FFIType("char*", sizeof(char*), true, "char");
    types["const char*"] = FFIType("const char*", sizeof(char*), true, "char");
}

bool HeaderParser::parseHeaderFile(const std::string& headerPath) {
    std::ifstream file(headerPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseHeaderContent(buffer.str());
}

bool HeaderParser::parseHeaderContent(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty() || line[0] == '/' || line.substr(0, 2) == "//") {
            continue; // Skip empty lines and comments
        }
        
        // Parse #define constants
        if (line.substr(0, 7) == "#define") {
            parseDefine(line);
        }
        // Parse typedef
        else if (line.substr(0, 7) == "typedef") {
            parseTypedef(line);
        }
        // Parse struct (simplified)
        else if (line.substr(0, 6) == "struct" || line.substr(0, 8) == "typedef ") {
            size_t pos = 0;
            parseStruct(content, pos);
        }
    }
    
    return true;
}

void HeaderParser::parseDefine(const std::string& line) {
    // Simple #define parsing: #define NAME VALUE
    std::regex defineRegex(R"(#define\s+([A-Z_][A-Z0-9_]*)\s+(.+))");
    std::smatch match;
    
    if (std::regex_match(line, match, defineRegex)) {
        std::string name = match[1].str();
        std::string value = match[2].str();
        
        // Remove trailing comments
        size_t commentPos = value.find("//");
        if (commentPos != std::string::npos) {
            value = value.substr(0, commentPos);
        }
        commentPos = value.find("/*");
        if (commentPos != std::string::npos) {
            value = value.substr(0, commentPos);
        }
        
        // Trim whitespace
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        constants[name] = FFIConstant(name, value);
    }
}

void HeaderParser::parseStruct(const std::string& content, size_t& pos) {
    // Simplified struct parsing - this would need to be more robust for real SDL2 headers
    // For now, we'll add predefined SDL2 structs
}

void HeaderParser::parseTypedef(const std::string& line) {
    // Simple typedef parsing
    std::regex typedefRegex(R"(typedef\s+(.+?)\s+([A-Za-z_][A-Za-z0-9_]*)\s*;)");
    std::smatch match;
    
    if (std::regex_match(line, match, typedefRegex)) {
        std::string baseType = match[1].str();
        std::string newType = match[2].str();
        
        // Handle pointer types
        bool isPointer = baseType.find('*') != std::string::npos;
        if (isPointer) {
            baseType.erase(std::remove(baseType.begin(), baseType.end(), '*'), baseType.end());
            baseType.erase(0, baseType.find_first_not_of(" \t"));
            baseType.erase(baseType.find_last_not_of(" \t") + 1);
        }
        
        auto baseIt = types.find(baseType);
        if (baseIt != types.end()) {
            types[newType] = FFIType(newType, baseIt->second.size, isPointer, baseType);
        }
    }
}

std::string HeaderParser::generateFFIDefinitions() {
    std::stringstream output;
    
    output << "// Auto-generated FFI definitions\n\n";
    
    // Generate constants
    output << "// Constants\n";
    for (const auto& [name, constant] : constants) {
        output << "const " << name << " = " << constant.value << ";\n";
    }
    output << "\n";
    
    // Generate struct definitions
    output << "// Structures\n";
    for (const auto& [name, structDef] : structs) {
        output << "struct " << name << " {\n";
        for (const auto& [fieldName, fieldType] : structDef.fields) {
            output << "    " << fieldName << " as " << fieldType.name << ";\n";
        }
        output << "}\n\n";
    }
    
    return output.str();
}

void HeaderParser::addSDL2Definitions() {
    // Add common SDL2 constants
    constants["SDL_INIT_VIDEO"] = FFIConstant("SDL_INIT_VIDEO", "32");
    constants["SDL_WINDOWPOS_UNDEFINED"] = FFIConstant("SDL_WINDOWPOS_UNDEFINED", "536805376");
    constants["SDL_WINDOW_SHOWN"] = FFIConstant("SDL_WINDOW_SHOWN", "4");
    constants["SDL_WINDOW_RESIZABLE"] = FFIConstant("SDL_WINDOW_RESIZABLE", "32");
    constants["SDL_RENDERER_ACCELERATED"] = FFIConstant("SDL_RENDERER_ACCELERATED", "2");
    constants["SDL_RENDERER_PRESENTVSYNC"] = FFIConstant("SDL_RENDERER_PRESENTVSYNC", "4");
    constants["SDL_TEXTUREACCESS_TARGET"] = FFIConstant("SDL_TEXTUREACCESS_TARGET", "2");
    constants["SDL_PIXELFORMAT_RGBA8888"] = FFIConstant("SDL_PIXELFORMAT_RGBA8888", "373694468");
    
    // Add SDL2 struct definitions
    FFIStruct rectStruct("SDL_Rect");
    rectStruct.fields.push_back({"x", types["int"]});
    rectStruct.fields.push_back({"y", types["int"]});
    rectStruct.fields.push_back({"w", types["int"]});
    rectStruct.fields.push_back({"h", types["int"]});
    rectStruct.totalSize = 4 * sizeof(int);
    structs["SDL_Rect"] = rectStruct;
    
    FFIStruct colourStruct("SDL_Colour");
    colourStruct.fields.push_back({"r", types["uint8_t"]});
    colourStruct.fields.push_back({"g", types["uint8_t"]});
    colourStruct.fields.push_back({"b", types["uint8_t"]});
    colourStruct.fields.push_back({"a", types["uint8_t"]});
    colourStruct.totalSize = 4 * sizeof(uint8_t);
    structs["SDL_Colour"] = colourStruct;
    
    FFIStruct pointStruct("SDL_Point");
    pointStruct.fields.push_back({"x", types["int"]});
    pointStruct.fields.push_back({"y", types["int"]});
    pointStruct.totalSize = 2 * sizeof(int);
    structs["SDL_Point"] = pointStruct;
    
    // Add SDL2 pointer types
    types["SDL_Window*"] = FFIType("SDL_Window*", sizeof(void*), true, "SDL_Window");
    types["SDL_Renderer*"] = FFIType("SDL_Renderer*", sizeof(void*), true, "SDL_Renderer");
    types["SDL_Texture*"] = FFIType("SDL_Texture*", sizeof(void*), true, "SDL_Texture");
    types["SDL_Surface*"] = FFIType("SDL_Surface*", sizeof(void*), true, "SDL_Surface");
    types["SDL_Event*"] = FFIType("SDL_Event*", sizeof(void*), true, "SDL_Event");
}

void HeaderParser::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.length() && std::isspace(content[pos])) {
        pos++;
    }
}

std::string HeaderParser::extractIdentifier(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    size_t start = pos;
    
    while (pos < content.length() && 
           (std::isalnum(content[pos]) || content[pos] == '_')) {
        pos++;
    }
    
    return content.substr(start, pos - start);
}

std::string HeaderParser::extractValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    size_t start = pos;
    
    // Extract until semicolon, comma, or end of line
    while (pos < content.length() && 
           content[pos] != ';' && content[pos] != ',' && content[pos] != '\n') {
        pos++;
    }
    
    std::string value = content.substr(start, pos - start);
    // Trim trailing whitespace
    value.erase(value.find_last_not_of(" \t") + 1);
    return value;
}

} // namespace ffi
} // namespace rbasic