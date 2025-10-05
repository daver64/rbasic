// Graphics + Database Demo - Shows SDL window with SQLite data
// This demonstrates both graphics and database integration with proper parsing

debug_print("Starting Graphics + Database Demo...");

// Declare all variables globally to avoid scoping issues
var user_count = 0;
var current_name = "";
var current_score = "";
var parsing_name = true;
var pos = 1;
var char = "";
var display_text = "";
var y_pos = 280;
var i = 1;

// Initialise graphics mode
graphics_mode(800, 600);
set_colour(0, 0, 0);
draw_rect(0, 0, 800, 600, true); // Clear screen to black

// Set up title
set_colour(255, 255, 255);
draw_text(250, 50, "rbasic Graphics + Database Demo");

// Open database and create test data if needed
var db_result = db_open("graphics_demo.db");
debug_print("DB Open Result: " + str(db_result));

if (db_result == 0) {
    debug_print("Database Error: " + db_error());
    set_colour(255, 0, 0);
    draw_text(50, 100, "Database Connection Failed");
    refresh_screen();
    sleep_ms(5000);
} else {
    set_colour(0, 255, 0);
    draw_text(50, 100, "Database Connected Successfully");
    
    // Create table and insert some test data
    debug_print("Creating table...");
    var create_result = db_exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, score INTEGER)");
    debug_print("Create result: " + str(create_result));
    debug_print("DB Error after create: " + db_error());
    
    // Clear any existing data and insert fresh test data
    debug_print("Inserting data...");
    var delete_result = db_exec("DELETE FROM users");
    debug_print("Delete result: " + str(delete_result));
    
    var insert1 = db_exec("INSERT INTO users (name, score) VALUES ('Alice', 95)");
    var insert2 = db_exec("INSERT INTO users (name, score) VALUES ('Bob', 87)");
    var insert3 = db_exec("INSERT INTO users (name, score) VALUES ('Charlie', 92)");
    
    debug_print("Insert results: " + str(insert1) + " " + str(insert2) + " " + str(insert3));
    debug_print("Final DB error after inserts: " + db_error());
    
    // Check how many rows we actually have
    var count_data = db_query("SELECT COUNT(*) FROM users");
    debug_print("Row count query result: '" + count_data + "'");
    
    // Query the data and parse it into an array
    debug_print("Querying user data...");
    set_colour(255, 255, 0);
    draw_text(50, 150, "High Score Leaderboard:");
    
    var data = db_query("SELECT name, score FROM users ORDER BY score DESC");
    debug_print("Data query result: '" + data + "'");
    debug_print("Data length: " + str(len(data)));
    
    // Create arrays to hold parsed data
    dim user_names(5) as string;
    dim user_scores(5) as integer;
    
    if (len(data) > 0) {
        debug_print("Starting to parse data...");
        // Reset variables
        user_count = 0;
        current_name = "";
        current_score = "";
        parsing_name = true;
        
        // Parse the tab-separated data manually
        for (pos = 1; pos <= len(data) and user_count < 5; pos = pos + 1) {
            char = mid(data, pos, 1);
            
            if (char == "	") {  // Tab character - switch to parsing score
                parsing_name = false;
            } else if (char == "\n" or pos == len(data)) {
                // End of row - store the data
                if (len(current_name) > 0) {
                    user_count = user_count + 1;
                    user_names[user_count] = current_name;
                    if (len(current_score) > 0) {
                        user_scores[user_count] = val(current_score);
                    } else {
                        user_scores[user_count] = 0;
                    }
                }
                // Reset for next row
                current_name = "";
                current_score = "";
                parsing_name = true;
            } else if (char != "\r") {
                // Regular character (skip carriage returns)
                if (parsing_name) {
                    current_name = current_name + char;
                } else {
                    current_score = current_score + char;
                }
            }
        }
        
        debug_print("Parsed " + str(user_count) + " users total");
        for (i = 1; i <= user_count; i = i + 1) {
            debug_print("User " + str(i) + ": '" + user_names[i] + "' = " + str(user_scores[i]));
        }
        
        // Display the parsed data nicely
        set_colour(255, 255, 255);
        y_pos = 200;
        for (i = 1; i <= user_count; i = i + 1) {
            display_text = str(i) + ". " + user_names[i] + " - " + str(user_scores[i]) + " points";
            draw_text(100, y_pos, display_text);
            y_pos = y_pos + 35;
        }
        
        // Show success message
        set_colour(0, 255, 0);
        draw_text(50, 450, "Successfully loaded " + str(user_count) + " users from database");
        
    } else {
        debug_print("No data returned from query!");
        set_colour(255, 100, 100);
        draw_text(100, 200, "No data found in database");
    }    // Add some visual decorations
    set_colour(0, 100, 255);
    draw_line(50, 130, 750, 130);
    draw_line(50, 420, 750, 420);
    
    // Draw some decorative elements
    set_colour(100, 100, 255);
    for (var j = 0; j < 8; j = j + 1) {
        var x = 100 + j * 80;
        var y = 500 + j * 3;
        draw_rect(x, y, 15, 15, false);
    }
    
    // Instructions
    set_colour(200, 200, 200);
    draw_text(50, 520, "Press ESC to exit or close window...");
    
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

debug_print("Graphics + Database Demo completed!");