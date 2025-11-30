#ifdef SQLITE3_SUPPORT_ENABLED

#include "sqlite3_wrapper.h"
#include <cstring>

namespace basic_runtime {


// SQLite3_ResourceManager implementation
SQLite3_ResourceManager& SQLite3_ResourceManager::instance() {
    static SQLite3_ResourceManager instance;
    return instance;
}

int SQLite3_ResourceManager::addDatabase(sqlite3* db) {
    int id = nextDbId++;
    databases[id] = db;
    return id;
}

sqlite3* SQLite3_ResourceManager::getDatabase(int handle) {
    auto it = databases.find(handle);
    return (it != databases.end()) ? it->second : nullptr;
}

void SQLite3_ResourceManager::removeDatabase(int handle) {
    databases.erase(handle);
}

int SQLite3_ResourceManager::addStatement(sqlite3_stmt* stmt) {
    int id = nextStmtId++;
    statements[id] = stmt;
    return id;
}

sqlite3_stmt* SQLite3_ResourceManager::getStatement(int handle) {
    auto it = statements.find(handle);
    return (it != statements.end()) ? it->second : nullptr;
}

void SQLite3_ResourceManager::removeStatement(int handle) {
    statements.erase(handle);
}

void SQLite3_ResourceManager::cleanup() {
    // Clean up all statements
    for (auto& pair : statements) {
        if (pair.second) {
            sqlite3_finalize(pair.second);
        }
    }
    statements.clear();
    
    // Clean up all databases
    for (auto& pair : databases) {
        if (pair.second) {
            sqlite3_close(pair.second);
        }
    }
    databases.clear();
}

// Core database functions
int sqlite_open(const std::string& filename) {
    sqlite3* db = nullptr;
    int result = sqlite3_open(filename.c_str(), &db);
    
    if (result != SQLITE_OK) {
        if (db) {
            sqlite3_close(db);
        }
        return -1;
    }
    
    return SQLite3_ResourceManager::instance().addDatabase(db);
}

void sqlite_close(int db_handle) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (db) {
        sqlite3_close(db);
        SQLite3_ResourceManager::instance().removeDatabase(db_handle);
    }
}

int sqlite_exec(int db_handle, const std::string& sql) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (!db) {
        return SQLITE_ERROR;
    }
    
    char* error_msg = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_msg);
    
    if (error_msg) {
        sqlite3_free(error_msg);
    }
    
    return result;
}

std::string sqlite_errmsg(int db_handle) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (!db) {
        return "Invalid database handle";
    }
    
    const char* msg = sqlite3_errmsg(db);
    return msg ? std::string(msg) : std::string();
}

int sqlite_get_last_error_code(int db_handle) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (!db) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_errcode(db);
}

long long sqlite_last_insert_rowid(int db_handle) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (!db) {
        return -1;
    }
    
    return sqlite3_last_insert_rowid(db);
}

int sqlite_changes(int db_handle) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (!db) {
        return 0;
    }
    
    return sqlite3_changes(db);
}

// Prepared statement functions
int sqlite_prepare(int db_handle, const std::string& sql) {
    sqlite3* db = SQLite3_ResourceManager::instance().getDatabase(db_handle);
    if (!db) {
        return -1;
    }
    
    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        if (stmt) {
            sqlite3_finalize(stmt);
        }
        return -1;
    }
    
    return SQLite3_ResourceManager::instance().addStatement(stmt);
}

int sqlite_finalize(int stmt_handle) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    int result = sqlite3_finalize(stmt);
    SQLite3_ResourceManager::instance().removeStatement(stmt_handle);
    return result;
}

int sqlite_reset(int stmt_handle) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_reset(stmt);
}

int sqlite_clear_bindings(int stmt_handle) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_clear_bindings(stmt);
}

// Binding functions
int sqlite_bind_int(int stmt_handle, int index, int value) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_bind_int(stmt, index, value);
}

int sqlite_bind_int64(int stmt_handle, int index, long long value) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_bind_int64(stmt, index, value);
}

int sqlite_bind_double(int stmt_handle, int index, double value) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_bind_double(stmt, index, value);
}

int sqlite_bind_text(int stmt_handle, int index, const std::string& value) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

int sqlite_bind_null(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_bind_null(stmt, index);
}

// Step and column access
int sqlite_step(int stmt_handle) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_ERROR;
    }
    
    return sqlite3_step(stmt);
}

int sqlite_column_count(int stmt_handle) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return 0;
    }
    
    return sqlite3_column_count(stmt);
}

int sqlite_column_type(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return SQLITE_NULL;
    }
    
    return sqlite3_column_type(stmt, index);
}

std::string sqlite_column_name(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return "";
    }
    
    const char* name = sqlite3_column_name(stmt, index);
    return name ? std::string(name) : std::string();
}

// Column retrieval functions
int sqlite_column_int(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return 0;
    }
    
    return sqlite3_column_int(stmt, index);
}

long long sqlite_column_int64(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return 0;
    }
    
    return sqlite3_column_int64(stmt, index);
}

double sqlite_column_double(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return 0.0;
    }
    
    return sqlite3_column_double(stmt, index);
}

std::string sqlite_column_text(int stmt_handle, int index) {
    sqlite3_stmt* stmt = SQLite3_ResourceManager::instance().getStatement(stmt_handle);
    if (!stmt) {
        return "";
    }
    
    const unsigned char* text = sqlite3_column_text(stmt, index);
    return text ? std::string(reinterpret_cast<const char*>(text)) : std::string();
}

// Utility functions
std::string sqlite_version() {
    return std::string(sqlite3_libversion());
}

int sqlite_threadsafe() {
    return sqlite3_threadsafe();
}

// Transaction helpers
int sqlite_begin_transaction(int db_handle) {
    return sqlite_exec(db_handle, "BEGIN TRANSACTION");
}

int sqlite_commit_transaction(int db_handle) {
    return sqlite_exec(db_handle, "COMMIT");
}

int sqlite_rollback_transaction(int db_handle) {
    return sqlite_exec(db_handle, "ROLLBACK");
}

} // namespace basic_runtime

#endif // SQLITE3_SUPPORT_ENABLED
