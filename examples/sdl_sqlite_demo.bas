// Example demonstrating SDL2 graphics with SQLite database
// This example creates a simple data visualization app

import "blib/sdl2.bas";
import "blib/sqlite.bas";

// Create sample database and table
function setup_database() {
    if (!sqlite_open("demo.db")) {
        print("Failed to open database");
        return 0;
    }
    
    // Create a table for storing points
    sqlite_create_table("points", "id INTEGER PRIMARY KEY, x INTEGER, y INTEGER, color_r INTEGER, color_g INTEGER, color_b INTEGER");
    
    // Insert some sample data
    sqlite_exec("DELETE FROM points"); // Clear existing data
    
    if (sqlite_insert_begin("points", "x, y, color_r, color_g, color_b")) {
        // Add some random points
        for (var i = 0; i < 50; i = i + 1) {
            var x = int_val(rnd() * 780) + 10;
            var y = int_val(rnd() * 580) + 10;
            var r = int_val(rnd() * 255);
            var g = int_val(rnd() * 255);
            var b = int_val(rnd() * 255);
            
            sqlite_bind_int(1, x);
            sqlite_bind_int(2, y);
            sqlite_bind_int(3, r);
            sqlite_bind_int(4, g);
            sqlite_bind_int(5, b);
            
            sqlite_step();
            sqlite_reset();
        }
    }
    
    print("Database setup complete with " + val(sqlite_changes()) + " points");
    return 1;
}

// Render all points from database
function render_points() {
    if (sqlite_select_begin("points", "x, y, color_r, color_g, color_b", "")) {
        while (sqlite_step() == 1) {
            var x = sqlite_get_int(0);
            var y = sqlite_get_int(1);
            var r = sqlite_get_int(2);
            var g = sqlite_get_int(3);
            var b = sqlite_get_int(4);
            
            sdl_set_color(r, g, b);
            sdl_fill_rect(x - 3, y - 3, 6, 6); // Draw small squares
        }
    }
}

// Add a new point where user clicks
function add_point_at_mouse(x as integer, y as integer) {
    if (sqlite_insert_begin("points", "x, y, color_r, color_g, color_b")) {
        var r = int_val(rnd() * 255);
        var g = int_val(rnd() * 255);
        var b = int_val(rnd() * 255);
        
        sqlite_bind_int(1, x);
        sqlite_bind_int(2, y);
        sqlite_bind_int(3, r);
        sqlite_bind_int(4, g);
        sqlite_bind_int(5, b);
        
        sqlite_step();
        print("Added point at (" + val(x) + ", " + val(y) + ")");
    }
}

// Main program
function main() {
    print("SDL2 + SQLite Demo");
    print("==================");
    
    // Setup database
    if (!setup_database()) {
        return;
    }
    
    // Initialize SDL
    if (!sdl_init("SDL2 + SQLite Demo", 800, 600)) {
        sqlite_close();
        return;
    }
    
    print("Use arrow keys to navigate, SPACE to add random points, ESC to exit");
    
    var offset_x = 0;
    var offset_y = 0;
    
    // Main loop
    while (sdl_is_running()) {
        var frame_start = sdl_frame_start();
        
        // Handle input
        var key = sdl_get_key();
        if (key == key_escape()) {
            break;
        } else if (key == key_space()) {
            // Add a random point
            var x = int_val(rnd() * 780) + 10;
            var y = int_val(rnd() * 580) + 10;
            add_point_at_mouse(x, y);
        } else if (key == key_up()) {
            offset_y = offset_y - 10;
        } else if (key == key_down()) {
            offset_y = offset_y + 10;
        } else if (key == key_left()) {
            offset_x = offset_x - 10;
        } else if (key == key_right()) {
            offset_x = offset_x + 10;
        }
        
        // Render
        sdl_color_black();
        sdl_clear();
        
        render_points();
        
        // Draw UI text area (simulate)
        sdl_color_white();
        sdl_draw_rect(10, 10, 200, 60);
        
        sdl_present();
        sdl_frame_end(frame_start, 60); // 60 FPS
    }
    
    // Cleanup
    sqlite_close();
    sdl_cleanup();
    
    print("Demo completed successfully!");
}

main();