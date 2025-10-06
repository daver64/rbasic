// SQLite Database Library fo// Execute a SQL statement without returning resultsh-level wrapper around SQLite3 using rbasic's FFI system
// Provides easy-to-use database operations and SQL execution

// SQLite3 Library Loading
ffi integer sqlite3_open(filename as string, ppDb as pointer) from "sqlite3.dll";
ffi integer sqlite3_close(db as pointer) from "sqlite3.dll";
ffi integer sqlite3_prepare_v2(db as pointer, sql as string, nByte as integer, ppStmt as pointer, pzTail as pointer) from "sqlite3.dll";
ffi integer sqlite3_step(pStmt as pointer) from "sqlite3.dll";
ffi integer sqlite3_finalize(pStmt as pointer) from "sqlite3.dll";
ffi integer sqlite3_column_count(pStmt as pointer) from "sqlite3.dll";
ffi pointer sqlite3_column_text(pStmt as pointer, iCol as integer) from "sqlite3.dll";
ffi integer sqlite3_column_int(pStmt as pointer, iCol as integer) from "sqlite3.dll";
ffi double sqlite3_column_double(pStmt as pointer, iCol as integer) from "sqlite3.dll";
ffi pointer sqlite3_column_name(pStmt as pointer, iCol as integer) from "sqlite3.dll";
ffi integer sqlite3_bind_text(pStmt as pointer, index as integer, value as string, n as integer, destructor as pointer) from "sqlite3.dll";
ffi integer sqlite3_bind_int(pStmt as pointer, index as integer, value as integer) from "sqlite3.dll";
ffi integer sqlite3_bind_double(pStmt as pointer, index as integer, value as double) from "sqlite3.dll";
ffi integer sqlite3_reset(pStmt as pointer) from "sqlite3.dll";
ffi integer sqlite3_clear_bindings(pStmt as pointer) from "sqlite3.dll";
ffi pointer sqlite3_errmsg(db as pointer) from "sqlite3.dll";
ffi integer sqlite3_changes(db as pointer) from "sqlite3.dll";
ffi integer sqlite3_last_insert_rowid(db as pointer) from "sqlite3.dll";

// Global SQLite state
var sqlite_db = NULL;
var sqlite_stmt = NULL;
var sqlite_is_open = false;

// Close the database connection
function sqlite_close() {
    if (not_null(sqlite_stmt)) {
        sqlite3_finalize(sqlite_stmt);
        sqlite_stmt = NULL;
    }
    
    if (not_null(sqlite_db)) {
        sqlite3_close(sqlite_db);
        sqlite_db = NULL;
        sqlite_is_open = false;
        print("Database closed");
    }
}

// Open a database connection
function sqlite_open(filename as string) as integer {
    if (sqlite_is_open) {
        print("Database already open, closing previous connection");
        sqlite_close();
    }
    
    var db_ptr = alloc_pointer_buffer();
    var result = sqlite3_open(filename, db_ptr);
    
    if (result == get_constant("SQLITE_OK")) {
        sqlite_db = deref_pointer(db_ptr);
        sqlite_is_open = true;
        print("Database opened: " + filename);
        return 1;
    } else {
        print("Failed to open database: " + filename);
        return 0;
    }
}

// Check if database is open
function sqlite_is_connected() as integer {
    return sqlite_is_open;
}

// Get last error message
function sqlite_error() as string {
    if (not_null(sqlite_db)) {
        var error_ptr = sqlite3_errmsg(sqlite_db);
        return deref_string(error_ptr);
    }
    return "No database connection";
}

// Execute a simple SQL statement (no results expected)
function sqlite_exec(sql as string) as integer {
    if (is_null(sqlite_db)) {
        print("No database connection");
        return 0;
    }
    
    var stmt_ptr = alloc_pointer_buffer();
    var result = sqlite3_prepare_v2(sqlite_db, sql, -1, stmt_ptr, null);
    
    if (result != get_constant("SQLITE_OK")) {
        print("SQL prepare failed: " + sqlite_error());
        return 0;
    }
    
    var stmt = deref_pointer(stmt_ptr);
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result == get_constant("SQLITE_DONE")) {
        return 1;
    } else {
        print("SQL execution failed: " + sqlite_error());
        return 0;
    }
}

// Prepare a SQL statement for execution (with parameters)
function sqlite_prepare(sql as string) as integer {
    if (is_null(sqlite_db)) {
        print("No database connection");
        return 0;
    }
    
    // Clean up previous statement
    if (not_null(sqlite_stmt)) {
        sqlite3_finalize(sqlite_stmt);
        sqlite_stmt = NULL;
    }
    
    var stmt_ptr = alloc_pointer_buffer();
    var result = sqlite3_prepare_v2(sqlite_db, sql, -1, stmt_ptr, null);
    
    if (result == get_constant("SQLITE_OK")) {
        sqlite_stmt = deref_pointer(stmt_ptr);
        return 1;
    } else {
        print("SQL prepare failed: " + sqlite_error());
        return 0;
    }
}

// Bind parameters to prepared statement
function sqlite_bind_text(index as integer, value as string) as integer {
    if (is_null(sqlite_stmt)) {
        print("No prepared statement");
        return 0;
    }
    
    var result = sqlite3_bind_text(sqlite_stmt, index, value, -1, NULL);
    return (result == get_constant("SQLITE_OK"));
}

function sqlite_bind_int(index as integer, value as integer) as integer {
    if (is_null(sqlite_stmt)) {
        print("No prepared statement");
        return 0;
    }
    
    var result = sqlite3_bind_int(sqlite_stmt, index, value);
    return (result == get_constant("SQLITE_OK"));
}

function sqlite_bind_double(index as integer, value as double) as integer {
    if (is_null(sqlite_stmt)) {
        print("No prepared statement");
        return 0;
    }
    
    var result = sqlite3_bind_double(sqlite_stmt, index, value);
    return (result == get_constant("SQLITE_OK"));
}

// Execute prepared statement and get next row
function sqlite_step() as integer {
    if (is_null(sqlite_stmt)) {
        print("No prepared statement");
        return 0;
    }
    
    var result = sqlite3_step(sqlite_stmt);
    
    if (result == get_constant("SQLITE_ROW")) {
        return 1; // Row available
    } else if (result == get_constant("SQLITE_DONE")) {
        return 0; // No more rows
    } else {
        print("SQL step failed: " + sqlite_error());
        return -1; // Error
    }
}

// Reset prepared statement for re-execution
function sqlite_reset() as integer {
    if (is_null(sqlite_stmt)) {
        print("No prepared statement");
        return 0;
    }
    
    var result = sqlite3_reset(sqlite_stmt);
    sqlite3_clear_bindings(sqlite_stmt);
    return (result == get_constant("SQLITE_OK"));
}

// Get column count from current row
function sqlite_column_count() as integer {
    if (is_null(sqlite_stmt)) {
        return 0;
    }
    
    return sqlite3_column_count(sqlite_stmt);
}

// Get column data from current row
function sqlite_get_text(column as integer) as string {
    if (is_null(sqlite_stmt)) {
        return "";
    }
    
    var text_ptr = sqlite3_column_text(sqlite_stmt, column);
    return deref_string(text_ptr);
}

function sqlite_get_int(column as integer) as integer {
    if (is_null(sqlite_stmt)) {
        return 0;
    }
    
    return sqlite3_column_int(sqlite_stmt, column);
}

function sqlite_get_double(column as integer) as double {
    if (is_null(sqlite_stmt)) {
        return 0.0;
    }
    
    return sqlite3_column_double(sqlite_stmt, column);
}

function sqlite_get_column_name(column as integer) as string {
    if (is_null(sqlite_stmt)) {
        return "";
    }
    
    var name_ptr = sqlite3_column_name(sqlite_stmt, column);
    return deref_string(name_ptr);
}

// Get database statistics
function sqlite_changes() as integer {
    if (is_null(sqlite_db)) {
        return 0;
    }
    
    return sqlite3_changes(sqlite_db);
}

function sqlite_last_insert_id() as integer {
    if (is_null(sqlite_db)) {
        return 0;
    }
    
    return sqlite3_last_insert_rowid(sqlite_db);
}

// High-level convenience functions

// Create a simple table
function sqlite_create_table(table_name as string, columns as string) as integer {
    var sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (" + columns + ")";
    return sqlite_exec(sql);
}

// Insert a row with named columns
function sqlite_insert_begin(table_name as string, columns as string) as integer {
    var placeholders = "";
    var column_count = 1;
    
    // Count commas to determine number of columns
    for (var i = 0; i < len(columns); i = i + 1) {
        if (mid(columns, i, 1) == ",") {
            column_count = column_count + 1;
        }
    }
    
    // Generate placeholders ?, ?, ?
    for (var i = 1; i <= column_count; i = i + 1) {
        if (i > 1) {
            placeholders = placeholders + ", ";
        }
        placeholders = placeholders + "?";
    }
    
    var sql = "INSERT INTO " + table_name + " (" + columns + ") VALUES (" + placeholders + ")";
    return sqlite_prepare(sql);
}

// Simple select with optional WHERE clause
function sqlite_select_begin(table_name as string, columns as string, where_clause as string) as integer {
    var sql = "SELECT " + columns + " FROM " + table_name;
    
    if (len(where_clause) > 0) {
        sql = sql + " WHERE " + where_clause;
    }
    
    return sqlite_prepare(sql);
}

// Update with WHERE clause
function sqlite_update_begin(table_name as string, set_clause as string, where_clause as string) as integer {
    var sql = "UPDATE " + table_name + " SET " + set_clause;
    
    if (len(where_clause) > 0) {
        sql = sql + " WHERE " + where_clause;
    }
    
    return sqlite_prepare(sql);
}

// Delete with WHERE clause
function sqlite_delete_begin(table_name as string, where_clause as string) as integer {
    var sql = "DELETE FROM " + table_name;
    
    if (len(where_clause) > 0) {
        sql = sql + " WHERE " + where_clause;
    }
    
    return sqlite_prepare(sql);
}

// Print all rows from a query result
function sqlite_print_results() {
    var columns = sqlite_column_count();
    
    // Print column headers
    for (var i = 0; i < columns; i = i + 1) {
        if (i > 0) {
            print(" | ");
        }
        print(sqlite_get_column_name(i));
    }
    print();
    
    // Print separator
    for (var i = 0; i < columns; i = i + 1) {
        if (i > 0) {
            print("---");
        }
        print("----------");
    }
    print();
    
    // Print data rows
    while (sqlite_step() == 1) {
        for (var i = 0; i < columns; i = i + 1) {
            if (i > 0) {
                print(" | ");
            }
            print(sqlite_get_text(i));
        }
        print();
    }
}

// Transaction support
function sqlite_begin_transaction() as integer {
    return sqlite_exec("BEGIN TRANSACTION");
}

function sqlite_commit() as integer {
    return sqlite_exec("COMMIT");
}

function sqlite_rollback() as integer {
    return sqlite_exec("ROLLBACK");
}

// Database utility functions
function sqlite_table_exists(table_name as string) as integer {
    if (sqlite_prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?")) {
        sqlite_bind_text(1, table_name);
        var exists = sqlite_step();
        sqlite_reset();
        return (exists == 1);
    }
    return 0;
}

function sqlite_drop_table(table_name as string) as integer {
    var sql = "DROP TABLE IF EXISTS " + table_name;
    return sqlite_exec(sql);
}

function sqlite_get_table_info(table_name as string) {
    var sql = "PRAGMA table_info(" + table_name + ")";
    if (sqlite_prepare(sql)) {
        print("Table: " + table_name);
        sqlite_print_results();
    }
}

// Database backup (simple export to SQL)
function sqlite_dump_table(table_name as string) {
    print("-- Table: " + table_name);
    
    // Get CREATE statement
    if (sqlite_prepare("SELECT sql FROM sqlite_master WHERE type='table' AND name=?")) {
        sqlite_bind_text(1, table_name);
        if (sqlite_step() == 1) {
            print(sqlite_get_text(0) + ";");
        }
        sqlite_reset();
    }
    
    // Get all data
    var sql = "SELECT * FROM " + table_name;
    if (sqlite_prepare(sql)) {
        while (sqlite_step() == 1) {
            var columns = sqlite_column_count();
            var insert_sql = "INSERT INTO " + table_name + " VALUES (";
            
            for (var i = 0; i < columns; i = i + 1) {
                if (i > 0) {
                    insert_sql = insert_sql + ", ";
                }
                insert_sql = insert_sql + "'" + sqlite_get_text(i) + "'";
            }
            
            insert_sql = insert_sql + ");";
            print(insert_sql);
        }
    }
}

print("SQLite library loaded successfully");