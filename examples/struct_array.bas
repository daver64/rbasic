
// Array of Structs Demo
// Demonstrates creating and using arrays containing struct values

print("=== Array of Structs Demo ===");
print("");

// Define a simple 2D point struct
struct Point {
    x,
    y
};

// Create an array to hold multiple points
dim points(10);
print("Created array with capacity for 10 points");
print("");

// Initialize points with coordinates
print("Initializing points...");
for (var i = 0; i < 10; i = i + 1) {
    var xval = i * 10.0;
    var yval = i * i * 5.0;
    points[i] = Point { xval, yval };
    print("  Point[" + str(i) + "] = (" + str(xval) + ", " + str(yval) + ")");
}
print("");

// Read and display all points
print("Reading points from array...");
for (var i = 0; i < 10; i = i + 1) {
    var p = points[i];
    print("  Point[" + str(i) + "] = (" + str(p.x) + ", " + str(p.y) + ")");
}
print("");

// Calculate distance from origin for each point
print("Calculating distances from origin...");
for (var i = 0; i < 10; i = i + 1) {
    var p = points[i];
    var dist = sqrt(p.x * p.x + p.y * p.y);
    print("  Point[" + str(i) + "] distance: " + str(dist));
}
print("");

// More complex struct example
struct Person {
    name,
    age,
    score
};

dim people(5);
print("Creating array of Person structs...");

// Initialize with sample data
people[0] = Person { "Alice", 25.0, 95.5 };
people[1] = Person { "Bob", 30.0, 87.3 };
people[2] = Person { "Charlie", 22.0, 92.1 };
people[3] = Person { "Diana", 28.0, 98.7 };
people[4] = Person { "Eve", 35.0, 89.4 };

print("");
print("People database:");
for (var i = 0; i < 5; i = i + 1) {
    var p = people[i];
    print("  " + p.name + " - Age: " + str(p.age) + ", Score: " + str(p.score));
}
print("");

// Calculate average score
var total_score = 0.0;
for (var i = 0; i < 5; i = i + 1) {
    var p = people[i];
    total_score = total_score + p.score;
}
var avg_score = total_score / 5.0;
print("Average score: " + str(avg_score));
print("");

// Find person with highest score
var max_score = 0.0;
var max_index = 0;
for (var i = 0; i < 5; i = i + 1) {
    var p = people[i];
    if (p.score > max_score) {
        max_score = p.score;
        max_index = i;
    }
}

var top_person = people[max_index];
print("Highest scorer: " + top_person.name + " with " + str(top_person.score));
print("");

// 3D coordinate struct example
struct Coordinate {
    x,
    y,
    z
};

dim coords(5);
print("Creating 3D coordinate array...");

// Initialize 3D coordinates
coords[0] = Coordinate { 1.0, 2.0, 3.0 };
coords[1] = Coordinate { -2.0, 4.0, 1.0 };
coords[2] = Coordinate { 0.0, 0.0, 5.0 };
coords[3] = Coordinate { 3.5, -1.5, 2.5 };
coords[4] = Coordinate { -1.0, -2.0, -3.0 };

print("");
print("Coordinates:");
for (var i = 0; i < 5; i = i + 1) {
    var c = coords[i];
    print("  Coord[" + str(i) + "]: (" + str(c.x) + ", " + str(c.y) + ", " + str(c.z) + ")");
}
print("");

// Calculate 3D distances from origin
print("Distances from origin:");
for (var i = 0; i < 5; i = i + 1) {
    var c = coords[i];
    var dist = sqrt(c.x * c.x + c.y * c.y + c.z * c.z);
    print("  Coord[" + str(i) + "] distance: " + str(dist));
}

print("");
print("=== Array of Structs Demo Complete ===");
print("Demonstrated:");
print("  - Simple struct arrays (Point)");
print("  - Struct arrays with mixed types (Person)");
print("  - 3D coordinate structs (Coordinate)");
print("  - Array operations on structs");
print("  - Struct field access in loops");
