#ifndef SQLITE3_WRAPPER_H
#define SQLITE3_WRAPPER_H

#ifdef SQLITE3_SUPPORT_ENABLED

#include <sqlite3.h>
#include <string>
#include <map>
#include <memory>

namespace basic_runtime {

// SQLite3 wrapper functions

// Handle management structures
struct DatabaseHandle {
    sqlite3* db;
    int id;
};

struct StatementHandle {
    sqlite3_stmt* stmt;
    int id;
};

// Resource manager
class SQLite3_ResourceManager {
public:
    static SQLite3_ResourceManager& instance();
    
    // Database management
    int addDatabase(sqlite3* db);
    sqlite3* getDatabase(int handle);
    void removeDatabase(int handle);
    
    // Statement management
    int addStatement(sqlite3_stmt* stmt);
    sqlite3_stmt* getStatement(int handle);
    void removeStatement(int handle);
    
    // Cleanup
    void cleanup();
    
private:
    SQLite3_ResourceManager() : nextDbId(1), nextStmtId(1) {}
    
    std::map<int, sqlite3*> databases;
    std::map<int, sqlite3_stmt*> statements;
    
    int nextDbId;
    int nextStmtId;
};

// Core database functions
int sqlite_open(const std::string& filename);
void sqlite_close(int db_handle);
int sqlite_exec(int db_handle, const std::string& sql);
std::string sqlite_errmsg(int db_handle);
int sqlite_get_last_error_code(int db_handle);
long long sqlite_last_insert_rowid(int db_handle);
int sqlite_changes(int db_handle);

// Prepared statement functions
int sqlite_prepare(int db_handle, const std::string& sql);
int sqlite_finalize(int stmt_handle);
int sqlite_reset(int stmt_handle);
int sqlite_clear_bindings(int stmt_handle);

// Binding functions
int sqlite_bind_int(int stmt_handle, int index, int value);
int sqlite_bind_int64(int stmt_handle, int index, long long value);
int sqlite_bind_double(int stmt_handle, int index, double value);
int sqlite_bind_text(int stmt_handle, int index, const std::string& value);
int sqlite_bind_null(int stmt_handle, int index);

// Step and column access
int sqlite_step(int stmt_handle);
int sqlite_column_count(int stmt_handle);
int sqlite_column_type(int stmt_handle, int index);
std::string sqlite_column_name(int stmt_handle, int index);

// Column retrieval functions
int sqlite_column_int(int stmt_handle, int index);
long long sqlite_column_int64(int stmt_handle, int index);
double sqlite_column_double(int stmt_handle, int index);
std::string sqlite_column_text(int stmt_handle, int index);

// Utility functions
std::string sqlite_version();
int sqlite_threadsafe();

// Transaction helpers
int sqlite_begin_transaction(int db_handle);
int sqlite_commit_transaction(int db_handle);
int sqlite_rollback_transaction(int db_handle);

// SQLite constants
namespace constants {
    // Result codes
    const int OK = SQLITE_OK;
    const int ERROR = SQLITE_ERROR;
    const int BUSY = SQLITE_BUSY;
    const int LOCKED = SQLITE_LOCKED;
    const int NOMEM = SQLITE_NOMEM;
    const int READONLY = SQLITE_READONLY;
    const int INTERRUPT = SQLITE_INTERRUPT;
    const int IOERR = SQLITE_IOERR;
    const int CORRUPT = SQLITE_CORRUPT;
    const int NOTFOUND = SQLITE_NOTFOUND;
    const int FULL = SQLITE_FULL;
    const int CANTOPEN = SQLITE_CANTOPEN;
    const int PROTOCOL = SQLITE_PROTOCOL;
    const int EMPTY = SQLITE_EMPTY;
    const int SCHEMA = SQLITE_SCHEMA;
    const int TOOBIG = SQLITE_TOOBIG;
    const int CONSTRAINT = SQLITE_CONSTRAINT;
    const int MISMATCH = SQLITE_MISMATCH;
    const int MISUSE = SQLITE_MISUSE;
    const int NOLFS = SQLITE_NOLFS;
    const int AUTH = SQLITE_AUTH;
    const int FORMAT = SQLITE_FORMAT;
    const int RANGE = SQLITE_RANGE;
    const int NOTADB = SQLITE_NOTADB;
    const int ROW = SQLITE_ROW;
    const int DONE = SQLITE_DONE;
    
    // Column types
    const int INTEGER = SQLITE_INTEGER;
    const int FLOAT = SQLITE_FLOAT;
    const int TEXT = SQLITE_TEXT;
    const int BLOB = SQLITE_BLOB;
    const int NULL_TYPE = SQLITE_NULL;
    
    // Open flags
    const int OPEN_READONLY = SQLITE_OPEN_READONLY;
    const int OPEN_READWRITE = SQLITE_OPEN_READWRITE;
    const int OPEN_CREATE = SQLITE_OPEN_CREATE;
}

} // namespace basic_runtime

#endif // SQLITE3_SUPPORT_ENABLED

#endif // SQLITE3_WRAPPER_H
