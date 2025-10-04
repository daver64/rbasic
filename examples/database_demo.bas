// Simple SQLite database example for rbasic
// This demonstrates basic database operations

// Create and open a database
var result = db_open("test.db");
if (!result) {
    print("Failed to open database: " + db_error());
} else {
    print("Database opened successfully");
    
    // Create a table
    result = db_exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    if (!result) {
        print("Failed to create table: " + db_error());
    } else {
        print("Table created successfully");
        
        // Insert some data
        result = db_exec("INSERT INTO users (name, age) VALUES ('Alice', 25)");
        if (result) {
            print("Inserted Alice");
        }
        
        result = db_exec("INSERT INTO users (name, age) VALUES ('Bob', 30)");
        if (result) {
            print("Inserted Bob");
        }
        
        // Query the data
        print("Querying users:");
        var data = db_query("SELECT id, name, age FROM users");
        print(data);
        
        // Clean up
        db_close();
        print("Database closed");
    }
}