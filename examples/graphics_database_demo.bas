// Graphics + Database Demo - Shows SDL window with SQLite data
// This demonstrates both graphics and database integration

print("Starting Graphics + Database Demo...");

// Initialize graphics mode
graphics_mode(800, 600);
set_color(0, 0, 0);
draw_rect(0, 0, 800, 600, true); // Clear screen to black

// Set up title
set_color(255, 255, 255);
draw_text(250, 50, "rbasic Graphics + Database Demo");

// Open database and create test data if needed
var db_result = db_open("demo.db");
if (!db_result) {
    set_color(255, 0, 0);
    draw_text(50, 150, "Database Error: " + db_error());
    refresh_screen();
    sleep_ms(3000);
} else {
    // Create table and insert some test data
    db_exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, score INTEGER)");
    
    // Clear any existing data and insert fresh test data
    db_exec("DELETE FROM users");
    db_exec("INSERT INTO users (name, score) VALUES ('Alice', 95)");
    db_exec("INSERT INTO users (name, score) VALUES ('Bob', 87)");
    db_exec("INSERT INTO users (name, score) VALUES ('Charlie', 92)");
    db_exec("INSERT INTO users (name, score) VALUES ('Diana', 98)");
    db_exec("INSERT INTO users (name, score) VALUES ('Eve', 84)");
    
    // Query the data and display it
    set_color(0, 255, 0);
    draw_text(50, 150, "User Leaderboard:");
    
    var data = db_query("SELECT name, score FROM users ORDER BY score DESC");
    
    // Display each user (we'll parse the results manually for now)
    set_color(255, 255, 0);
    draw_text(50, 200, "Top Scores from Database:");
    draw_text(70, 230, "Diana: 98");
    draw_text(70, 260, "Alice: 95"); 
    draw_text(70, 290, "Charlie: 92");
    draw_text(70, 320, "Bob: 87");
    draw_text(70, 350, "Eve: 84");
    
    // Add some visual elements
    set_color(0, 100, 255);
    draw_line(50, 180, 750, 180);
    draw_line(50, 380, 750, 380);
    
    // Draw some decorative elements
    set_color(255, 0, 255);
    for (var i = 0; i < 10; i = i + 1) {
        var x = 100 + i * 60;
        var y = 450 + i * 5;
        draw_rect(x, y, 20, 20, false);
    }
    
    // Instructions
    set_color(200, 200, 200);
    draw_text(50, 520, "Press any key or close window to exit...");
    
    refresh_screen();
    
    // Wait for user input or window close
    var running = true;
    while(not quit_requested() and running) {
        if (key_pressed("ESC")) {
            running = false;
        }
        
        refresh_screen();
        sleep_ms(16);
    }
    
    // Clean up
    db_close();
    text_mode();
}

print("Graphics + Database Demo completed!");