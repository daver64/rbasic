// SQLite Database Demo - Full FFI Capabilities Test  
// Demonstrates advanced SQLite integration with rbasic FFI

// SQLite Core Functions
ffi "sqlite3.dll" sqlite3_open(filename as string, db as pointer) as integer;
ffi "sqlite3.dll" sqlite3_close(db as pointer) as integer;
ffi "sqlite3.dll" sqlite3_exec(db as pointer, sql as string, callback as pointer, 
                               arg as pointer, errmsg as pointer) as integer;

// SQLite Utility Functions
ffi "sqlite3.dll" sqlite3_errmsg(db as pointer) as string;
ffi "sqlite3.dll" sqlite3_last_insert_rowid(db as pointer) as integer;
ffi "sqlite3.dll" sqlite3_changes(db as pointer) as integer;

// Prepared Statement Functions (basic support)
ffi "sqlite3.dll" sqlite3_prepare_v2(db as pointer, sql as string, length as integer,
                                     stmt as pointer, tail as pointer) as integer;
ffi "sqlite3.dll" sqlite3_step(stmt as pointer) as integer;
ffi "sqlite3.dll" sqlite3_finalize(stmt as pointer) as integer;
ffi "sqlite3.dll" sqlite3_bind_int(stmt as pointer, index as integer, value as integer) as integer;
ffi "sqlite3.dll" sqlite3_bind_text(stmt as pointer, index as integer, text as string,
                                   length as integer, destructor as pointer) as integer;

// SQLite Constants
var SQLITE_OK = 0;
var SQLITE_ROW = 100;
var SQLITE_DONE = 101;

function main() {
    print("=== SQLite Database Demo ===");
    
    var database = null;
    
    // Open database
    var result = sqlite3_open("demo.db", database);
    if (result != SQLITE_OK) {
        print("Cannot open database");
        return 1;
    }
    
    print("Database opened successfully");
    
    // Create table
    var create_sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER, email TEXT);";
    result = sqlite3_exec(database, create_sql, null, null, null);
    
    if (result != SQLITE_OK) {
        print("Error creating table:", sqlite3_errmsg(database));
        sqlite3_close(database);
        return 1;
    }
    
    print("Table created successfully");
    
    // Insert sample data
    var insert_sql = "INSERT INTO users (name, age, email) VALUES ('Alice', 25, 'alice@example.com');";
    result = sqlite3_exec(database, insert_sql, null, null, null);
    
    if (result == SQLITE_OK) {
        print("Inserted user Alice, Row ID:", sqlite3_last_insert_rowid(database));
    }
    
    insert_sql = "INSERT INTO users (name, age, email) VALUES ('Bob', 30, 'bob@example.com');";
    result = sqlite3_exec(database, insert_sql, null, null, null);
    
    if (result == SQLITE_OK) {
        print("Inserted user Bob, Row ID:", sqlite3_last_insert_rowid(database));
    }
    
    insert_sql = "INSERT INTO users (name, age, email) VALUES ('Charlie', 35, 'charlie@example.com');";
    result = sqlite3_exec(database, insert_sql, null, null, null);
    
    if (result == SQLITE_OK) {
        print("Inserted user Charlie, Row ID:", sqlite3_last_insert_rowid(database));
    }
    
    print("Sample data inserted");
    
    // Update operation
    var update_sql = "UPDATE users SET age = 26 WHERE name = 'Alice';";
    result = sqlite3_exec(database, update_sql, null, null, null);
    
    if (result == SQLITE_OK) {
        print("Updated Alice's age, Rows affected:", sqlite3_changes(database));
    }
    
    // Count records
    var count_sql = "SELECT COUNT(*) FROM users;";
    result = sqlite3_exec(database, count_sql, null, null, null);
    
    if (result == SQLITE_OK) {
        print("Count query executed successfully");
    }
    
    // Demonstrate prepared statement (basic)
    var prepared_sql = "SELECT name, age FROM users WHERE age > ?;";
    var stmt = null;
    
    result = sqlite3_prepare_v2(database, prepared_sql, -1, stmt, null);
    if (result == SQLITE_OK) {
        print("Prepared statement created");
        
        // Bind parameter (age > 25)
        sqlite3_bind_int(stmt, 1, 25);
        
        // Execute and get results
        var step_result = sqlite3_step(stmt);
        var row_count = 0;
        
        while (step_result == SQLITE_ROW) {
            row_count = row_count + 1;
            step_result = sqlite3_step(stmt);
        }
        
        print("Found", row_count, "users over age 25");
        
        sqlite3_finalize(stmt);
    }
    
    // Delete operation
    var delete_sql = "DELETE FROM users WHERE age < 30;";
    result = sqlite3_exec(database, delete_sql, null, null, null);
    
    if (result == SQLITE_OK) {
        print("Deleted young users, Rows affected:", sqlite3_changes(database));
    }
    
    // Final cleanup
    sqlite3_close(database);
    
    print("=== Database Demo Complete ===");
    print("Operations performed:");
    print("  - Database creation and connection");
    print("  - Table creation");
    print("  - Data insertion with auto-increment IDs");
    print("  - Data updates and deletes");
    print("  - Prepared statements with parameters");
    print("  - Row counting and result processing");
    
    return 0;
}