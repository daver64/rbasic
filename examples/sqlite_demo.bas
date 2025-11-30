// SQLite3 Database Demo
// Demonstrates SQLite3 integration with database creation, tables, and queries
//
// Build with: cmake -DWITH_SQLITE3=ON .. && make
// Run interpreted: ./rbasic -i examples/sqlite_demo.bas
// Run compiled: ./rbasic -c examples/sqlite_demo.bas -o sqlite_demo && ./sqlite_demo
//
// Requirements: SQLite3 library installed
// Linux: sudo apt install libsqlite3-dev
// macOS: brew install sqlite3

// SQLite3 constants (defined in rbasic when built with -DWITH_SQLITE3=ON)
var SQLITE_OK = 0;
var SQLITE_ROW = 100;
var SQLITE_DONE = 101;

print("SQLite3 Database Demo");
print("====================");
print("");
print("SQLite version: " + sqlite_version());
print("");

// Open database (creates if doesn't exist)
print("Opening database: test.db");
var db = sqlite_open("test.db");
if (db < 0) {
    print("Failed to open database");
    exit();
}
print("Database opened successfully (handle: " + str(db) + ")");
print("");

// Create table
print("Creating table 'users'...");
var sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER, email TEXT)";
var result = sqlite_exec(db, sql);

if (result != SQLITE_OK) {
    print("Create table failed: " + sqlite_errmsg(db));
    sqlite_close(db);
    exit();
}
print("Table created successfully");
print("");

// Delete existing data (for demo repeatability)
print("Clearing existing data...");
sqlite_exec(db, "DELETE FROM users");
print("");

// Insert data using prepared statements
print("Inserting users...");

var insert_sql = "INSERT INTO users (name, age, email) VALUES (?, ?, ?)";
var stmt = sqlite_prepare(db, insert_sql);

if (stmt < 0) {
    print("Prepare statement failed: " + sqlite_errmsg(db));
    sqlite_close(db);
    exit();
}

// Insert first user
sqlite_bind_text(stmt, 1, "Alice Johnson");
sqlite_bind_int(stmt, 2, 28);
sqlite_bind_text(stmt, 3, "alice@example.com");
result = sqlite_step(stmt);

if (result != SQLITE_DONE) {
    print("Insert failed: " + sqlite_errmsg(db));
} else {
    print("Inserted: Alice Johnson, age 28");
}

// Reset and insert second user
sqlite_reset(stmt);
sqlite_bind_text(stmt, 1, "Bob Smith");
sqlite_bind_int(stmt, 2, 35);
sqlite_bind_text(stmt, 3, "bob@example.com");
result = sqlite_step(stmt);

if (result != SQLITE_DONE) {
    print("Insert failed: " + sqlite_errmsg(db));
} else {
    print("Inserted: Bob Smith, age 35");
}

// Reset and insert third user
sqlite_reset(stmt);
sqlite_bind_text(stmt, 1, "Charlie Brown");
sqlite_bind_int(stmt, 2, 42);
sqlite_bind_text(stmt, 3, "charlie@example.com");
result = sqlite_step(stmt);

if (result != SQLITE_DONE) {
    print("Insert failed: " + sqlite_errmsg(db));
} else {
    print("Inserted: Charlie Brown, age 42");
}

// Finalize insert statement
sqlite_finalize(stmt);
print("");

// Query data
print("Querying all users...");
print("-----------------------------------");

var query_sql = "SELECT id, name, age, email FROM users ORDER BY age";
var query_stmt = sqlite_prepare(db, query_sql);

if (query_stmt < 0) {
    print("Prepare query failed: " + sqlite_errmsg(db));
    sqlite_close(db);
    exit();
}

var row_count = 0;
while (sqlite_step(query_stmt) == SQLITE_ROW) {
    var id = sqlite_column_int(query_stmt, 0);
    var name = sqlite_column_text(query_stmt, 1);
    var age = sqlite_column_int(query_stmt, 2);
    var email = sqlite_column_text(query_stmt, 3);
    
    print("ID: " + str(id) + " | Name: " + name + " | Age: " + str(age) + " | Email: " + email);
    row_count = row_count + 1;
}

print("-----------------------------------");
print("Total rows: " + str(row_count));
print("");

// Finalize query statement
sqlite_finalize(query_stmt);

// Query with filter
print("Querying users over 30...");
print("-----------------------------------");

var filter_sql = "SELECT name, age FROM users WHERE age > ? ORDER BY age";
var filter_stmt = sqlite_prepare(db, filter_sql);

if (filter_stmt < 0) {
    print("Prepare filter query failed: " + sqlite_errmsg(db));
    sqlite_close(db);
    exit();
}

sqlite_bind_int(filter_stmt, 1, 30);

row_count = 0;
while (sqlite_step(filter_stmt) == SQLITE_ROW) {
    var name = sqlite_column_text(filter_stmt, 0);
    var age = sqlite_column_int(filter_stmt, 1);
    
    print(name + " (age " + str(age) + ")");
    row_count = row_count + 1;
}

print("-----------------------------------");
print("Users over 30: " + str(row_count));
print("");

// Finalize filter statement
sqlite_finalize(filter_stmt);

// Update data
print("Updating Bob's age to 36...");
var update_sql = "UPDATE users SET age = ? WHERE name = ?";
var update_stmt = sqlite_prepare(db, update_sql);

if (update_stmt >= 0) {
    sqlite_bind_int(update_stmt, 1, 36);
    sqlite_bind_text(update_stmt, 2, "Bob Smith");
    result = sqlite_step(update_stmt);
    
    if (result == SQLITE_DONE) {
        var changes = sqlite_changes(db);
        print("Updated " + str(changes) + " row(s)");
    } else {
        print("Update failed: " + sqlite_errmsg(db));
    }
    
    sqlite_finalize(update_stmt);
}
print("");

// Transaction demo
print("Transaction demo: Adding multiple users...");
sqlite_exec(db, "BEGIN TRANSACTION");

insert_sql = "INSERT INTO users (name, age, email) VALUES (?, ?, ?)";
stmt = sqlite_prepare(db, insert_sql);

sqlite_bind_text(stmt, 1, "David Wilson");
sqlite_bind_int(stmt, 2, 29);
sqlite_bind_text(stmt, 3, "david@example.com");
sqlite_step(stmt);

sqlite_reset(stmt);
sqlite_bind_text(stmt, 1, "Emma Davis");
sqlite_bind_int(stmt, 2, 31);
sqlite_bind_text(stmt, 3, "emma@example.com");
sqlite_step(stmt);

sqlite_finalize(stmt);
sqlite_exec(db, "COMMIT");
print("Transaction committed");
print("");

// Final count
var count_sql = "SELECT COUNT(*) FROM users";
var count_stmt = sqlite_prepare(db, count_sql);

if (count_stmt >= 0) {
    if (sqlite_step(count_stmt) == SQLITE_ROW) {
        var total = sqlite_column_int(count_stmt, 0);
        print("Total users in database: " + str(total));
    }
    sqlite_finalize(count_stmt);
}
print("");

// Cleanup
print("Closing database...");
sqlite_close(db);
print("SQLite3 Demo completed successfully");
