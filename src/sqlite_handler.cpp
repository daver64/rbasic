#include "sqlite_handler.h"

#ifdef RBASIC_SQLITE_SUPPORT

#include <iostream>
#include <sstream>

namespace rbasic {

// Global SQLite handler instance
std::unique_ptr<SQLiteHandler> g_sqlite_handler;

SQLiteHandler::SQLiteHandler() 
    : db_(nullptr), last_error_(""), last_error_code_(SQLITE_OK) {
}

SQLiteHandler::~SQLiteHandler() {
    close();
}

bool SQLiteHandler::open(const std::string& database_path) {
    close(); // Close any existing connection
    
    int result = sqlite3_open(database_path.c_str(), &db_);
    if (result != SQLITE_OK) {
        last_error_ = sqlite3_errmsg(db_);
        last_error_code_ = result;
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }
    
    last_error_.clear();
    last_error_code_ = SQLITE_OK;
    return true;
}

void SQLiteHandler::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
    last_error_.clear();
    last_error_code_ = SQLITE_OK;
}

bool SQLiteHandler::isOpen() const {
    return db_ != nullptr;
}

bool SQLiteHandler::exec(const std::string& sql) {
    if (!db_) {
        last_error_ = "Database not open";
        last_error_code_ = SQLITE_MISUSE;
        return false;
    }
    
    char* error_msg = nullptr;
    int result = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error_msg);
    
    if (result != SQLITE_OK) {
        last_error_ = error_msg ? error_msg : "Unknown error";
        last_error_code_ = result;
        if (error_msg) {
            sqlite3_free(error_msg);
        }
        return false;
    }
    
    last_error_.clear();
    last_error_code_ = SQLITE_OK;
    return true;
}

std::vector<std::vector<std::string>> SQLiteHandler::query(const std::string& sql) {
    std::vector<std::vector<std::string>> results;
    
    if (!db_) {
        last_error_ = "Database not open";
        last_error_code_ = SQLITE_MISUSE;
        return results;
    }
    
    char* error_msg = nullptr;
    int result = sqlite3_exec(db_, sql.c_str(), queryCallback, &results, &error_msg);
    
    if (result != SQLITE_OK) {
        last_error_ = error_msg ? error_msg : "Unknown error";
        last_error_code_ = result;
        if (error_msg) {
            sqlite3_free(error_msg);
        }
        results.clear();
    } else {
        last_error_.clear();
        last_error_code_ = SQLITE_OK;
    }
    
    return results;
}

std::string SQLiteHandler::getLastError() const {
    return last_error_;
}

int SQLiteHandler::getLastErrorCode() const {
    return last_error_code_;
}

std::string SQLiteHandler::escapeString(const std::string& str) const {
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

int SQLiteHandler::queryCallback(void* data, int argc, char** argv, char** col_names) {
    auto* results = static_cast<std::vector<std::vector<std::string>>*>(data);
    
    std::vector<std::string> row;
    row.reserve(argc);
    
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? argv[i] : "");
    }
    
    results->push_back(std::move(row));
    return 0;
}

void initialiseSQLiteHandler() {
    g_sqlite_handler = std::make_unique<SQLiteHandler>();
}

void cleanupSQLiteHandler() {
    g_sqlite_handler.reset();
}

} // namespace rbasic

#endif // RBASIC_SQLITE_SUPPORT