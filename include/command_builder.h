#pragma once

#include <string>
#include <vector>

namespace rbasic {

class CommandBuilder {
private:
    std::string compiler_;
    std::vector<std::string> compileFlags_;
    std::vector<std::string> linkFlags_;
    std::string inputFile_;
    std::string outputFile_;
    std::vector<std::string> libraries_;

public:
    CommandBuilder& compiler(const std::string& compiler);
    CommandBuilder& compileFlag(const std::string& flag);
    CommandBuilder& compileFlags(const std::vector<std::string>& flags);
    CommandBuilder& linkFlag(const std::string& flag);
    CommandBuilder& linkFlags(const std::vector<std::string>& flags);
    CommandBuilder& input(const std::string& inputFile);
    CommandBuilder& output(const std::string& outputFile);
    CommandBuilder& library(const std::string& library);
    
    std::string build() const;
    int execute() const;

private:
    std::string escapeArgument(const std::string& arg) const;
    bool isValidPath(const std::string& path) const;
};

} // namespace rbasic