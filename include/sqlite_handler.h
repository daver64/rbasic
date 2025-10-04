#pragma once

#ifdef RBASIC_SQLITE_SUPPORT

#include "common.h"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>

namespace rbasic {

class SQLiteHandler {
public:
    SQLiteHandler();
    ~SQLiteHandler();
    
    // Database operations
    bool open(const std::string& database_path);
    void close();
    bool isOpen() const;
    
    // SQL execution
    bool exec(const std::string& sql);
    std::vector<std::vector<std::string>> query(const std::string& sql);
    
    // Error handling
    std::string getLastError() const;
    int getLastErrorCode() const;
    
    // Utility functions
    std::string escapeString(const std::string& str) const;
    
private:
    sqlite3* db_;
    std::string last_error_;
    int last_error_code_;
    
    // Callback for query results
    static int queryCallback(void* data, int argc, char** argv, char** col_names);
};

// Global SQLite handler instance
extern std::unique_ptr<SQLiteHandler> g_sqlite_handler;

// Initialize/cleanup functions
void initializeSQLiteHandler();
void cleanupSQLiteHandler();

} // namespace rbasic

#endif // RBASIC_SQLITE_SUPPORT