// Simple SQLite Database Demo using rbasic FFI
// Demonstrates basic SQLite functionality with the wrapper library

import "blib/sqlite.bas";

print("=== Simple SQLite Database Demo ===");
print("Testing basic SQLite operations");
print();

// Open database
print("1. Opening database...");
var result = sqlite_open("simple_demo.db");
if (result == 0) {
    print("ERROR: Failed to open database");
    return;
}
print("   Database opened successfully");

// Create table
print();
print("2. Creating users table...");
result = sqlite_exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, age INTEGER)");
if (result == 0) {
    print("ERROR: Failed to create table");
    sqlite_close();
    return;
}
print("   Table created successfully");

// Insert data
print();
print("3. Inserting users...");
result = sqlite_exec("INSERT INTO users (name, age) VALUES ('Alice', 28)");
if (result == 1) {
    print("   Inserted Alice, Row ID: " + sqlite_last_insert_id());
}

result = sqlite_exec("INSERT INTO users (name, age) VALUES ('Bob', 35)");
if (result == 1) {
    print("   Inserted Bob, Row ID: " + sqlite_last_insert_id());
}

result = sqlite_exec("INSERT INTO users (name, age) VALUES ('Charlie', 22)");
if (result == 1) {
    print("   Inserted Charlie, Row ID: " + sqlite_last_insert_id());
}

// Query data using prepared statement
print();
print("4. Querying all users...");
result = sqlite_prepare("SELECT id, name, age FROM users ORDER BY id");
if (result == 1) {
    print("   User List:");
    print("   ID | Name    | Age");
    print("   ---|---------|----");
    
    while (sqlite_step() == 1) {
        var id = sqlite_get_int(0);
        var name = sqlite_get_text(1);
        var age = sqlite_get_int(2);
        print("   " + id + "  | " + name + " | " + age);
    }
}

// Update data
print();
print("5. Updating Alice's age...");
result = sqlite_exec("UPDATE users SET age = 29 WHERE name = 'Alice'");
if (result == 1) {
    print("   Update successful, rows affected: " + sqlite_changes());
}

// Query specific data with prepared statement
print();
print("6. Finding users over 25...");
result = sqlite_prepare("SELECT name, age FROM users WHERE age > 25 ORDER BY age");
if (result == 1) {
    print("   Users over 25:");
    while (sqlite_step() == 1) {
        var name = sqlite_get_text(0);
        var age = sqlite_get_int(1);
        print("   - " + name + " (age " + age + ")");
    }
}

// Count users
print();
print("7. Counting total users...");
result = sqlite_prepare("SELECT COUNT(*) FROM users");
if (result == 1) {
    if (sqlite_step() == 1) {
        var count = sqlite_get_int(0);
        print("   Total users: " + count);
    }
}

// Delete some data
print();
print("8. Removing users under 25...");
result = sqlite_exec("DELETE FROM users WHERE age < 25");
if (result == 1) {
    print("   Deleted users, rows affected: " + sqlite_changes());
}

// Final count
print();
print("9. Final user count...");
result = sqlite_prepare("SELECT COUNT(*) FROM users");
if (result == 1) {
    if (sqlite_step() == 1) {
        var final_count = sqlite_get_int(0);
        print("   Final total: " + final_count + " users");
    }
}

// Close database
print();
print("10. Closing database...");
sqlite_close();

print();
print("=== SQLite Demo Complete ===");
print("Successfully demonstrated:");
print("  * Database connection (sqlite_open/sqlite_close)");
print("  * Table creation (sqlite_exec)");
print("  * Data insertion with auto-increment IDs");
print("  * Row tracking (sqlite_last_insert_id)");
print("  * Prepared statements (sqlite_prepare/sqlite_step)");
print("  * Data retrieval (sqlite_get_text/sqlite_get_int)");
print("  * Data updates with change tracking (sqlite_changes)");
print("  * Data deletion and cleanup");
print("  * Counting and aggregation");
print();
print("SQLite wrapper library is working perfectly!");