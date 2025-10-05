#include "runtime.h"
#include <cmath>
#include <sstream>

#ifdef RBASIC_SQLITE_SUPPORT
#include "sqlite_handler.h"
#endif

namespace rbasic {

// Built-in functions
ValueType Runtime::basicPrint(const std::vector<ValueType>& args) {
    for (const auto& arg : args) {
        std::cout << valueToString(arg);
    }
    return 0;
}

ValueType Runtime::basicInput(const std::vector<ValueType>& args) {
    std::string line;
    std::getline(std::cin, line);
    
    // Try to parse as number
    try {
        if (hasDecimalPoint(line)) {
            return std::stod(line);
        } else {
            return std::stoi(line);
        }
    } catch (const std::exception&) {
        return line; // Return as string if not a number
    }
}

ValueType Runtime::basicLen(const std::vector<ValueType>& args) {
    if (args.empty()) return 0;
    return static_cast<int>(toString(args[0]).length());
}

ValueType Runtime::basicMid(const std::vector<ValueType>& args) {
    if (args.size() < 2) return std::string("");
    
    std::string str = toString(args[0]);
    int start = toInt(args[1]) - 1; // BASIC is 1-indexed
    int length = (args.size() > 2) ? toInt(args[2]) : str.length() - start;
    
    if (start < 0 || start >= static_cast<int>(str.length())) {
        return std::string("");
    }
    
    return str.substr(start, length);
}

ValueType Runtime::basicLeft(const std::vector<ValueType>& args) {
    if (args.size() < 2) return std::string("");
    
    std::string str = toString(args[0]);
    int length = toInt(args[1]);
    
    return str.substr(0, length);
}

ValueType Runtime::basicRight(const std::vector<ValueType>& args) {
    if (args.size() < 2) return std::string("");
    
    std::string str = toString(args[0]);
    int length = toInt(args[1]);
    int start = std::max(0, static_cast<int>(str.length()) - length);
    
    return str.substr(start);
}

ValueType Runtime::basicAbs(const std::vector<ValueType>& args) {
    if (args.empty()) return 0;
    
    if (std::holds_alternative<int>(args[0])) {
        return std::abs(std::get<int>(args[0]));
    } else if (std::holds_alternative<double>(args[0])) {
        return std::abs(std::get<double>(args[0]));
    }
    
    return 0;
}

ValueType Runtime::basicSqrt(const std::vector<ValueType>& args) {
    if (args.empty()) return 0.0;
    return std::sqrt(toDouble(args[0]));
}

ValueType Runtime::basicInt(const std::vector<ValueType>& args) {
    if (args.empty()) return 0;
    return toInt(args[0]);
}

ValueType Runtime::basicRnd(const std::vector<ValueType>& args) {
    return static_cast<double>(std::rand()) / RAND_MAX;
}

// Type conversion functions
int Runtime::toInt(const ValueType& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return static_cast<int>(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1 : 0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stoi(std::get<std::string>(value));
        } catch (const std::exception&) {
            return 0;
        }
    }
    return 0;
}

double Runtime::toDouble(const ValueType& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value));
        } catch (const std::exception&) {
            return 0.0;
        }
    }
    return 0.0;
}

std::string Runtime::toString(const ValueType& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    }
    return "";
}

bool Runtime::toBool(const ValueType& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    }
    return false;
}

#ifdef RBASIC_SQLITE_SUPPORT
#include "sqlite_handler.h"

ValueType Runtime::basicDbOpen(const std::vector<ValueType>& args) {
    if (args.empty()) return false;
    
    if (!g_sqlite_handler) {
        initialiseSQLiteHandler();
    }
    
    std::string database_path = toString(args[0]);
    return g_sqlite_handler->open(database_path);
}

ValueType Runtime::basicDbClose(const std::vector<ValueType>& args) {
    if (g_sqlite_handler) {
        g_sqlite_handler->close();
    }
    return true;
}

ValueType Runtime::basicDbExec(const std::vector<ValueType>& args) {
    if (args.empty() || !g_sqlite_handler) return false;
    
    std::string sql = toString(args[0]);
    return g_sqlite_handler->exec(sql);
}

ValueType Runtime::basicDbQuery(const std::vector<ValueType>& args) {
    if (args.empty() || !g_sqlite_handler) return std::string("");
    
    std::string sql = toString(args[0]);
    auto results = g_sqlite_handler->query(sql);
    
    // Convert results to string format (tab-separated columns, newline-separated rows)
    std::ostringstream output;
    for (size_t i = 0; i < results.size(); i++) {
        if (i > 0) output << "\n";
        
        const auto& row = results[i];
        for (size_t j = 0; j < row.size(); j++) {
            if (j > 0) output << "\t";
            output << row[j];
        }
    }
    
    return output.str();
}

ValueType Runtime::basicDbError(const std::vector<ValueType>& args) {
    if (g_sqlite_handler) {
        return g_sqlite_handler->getLastError();
    }
    return std::string("");
}

ValueType Runtime::basicDbEscape(const std::vector<ValueType>& args) {
    if (args.empty()) return std::string("");
    
    if (g_sqlite_handler) {
        return g_sqlite_handler->escapeString(toString(args[0]));
    }
    
    // Fallback escaping if handler not available
    std::string str = toString(args[0]);
    std::string escaped;
    escaped.reserve(str.length() * 2);
    
    for (char c : str) {
        if (c == '\'') {
            escaped += "''";
        } else {
            escaped += c;
        }
    }
    
    return escaped;
}
#endif

} // namespace rbasic