// Example showing structure functionality in rbasic with modern C-style syntax

// Define a Point structure
struct Point {
    x,
    y
};

// Define a Person structure  
struct Person {
    name,
    age,
    height
};

print("=== rbasic Structure Examples ===");

// Create and use Point structure
print("--- Point Structure ---");
var p1 = Point { 0, 0 };
var p2 = Point { 3, 4 };

print("Point 1: (", p1.x, ", ", p1.y, ")");
print("Point 2: (", p2.x, ", ", p2.y, ")");

// Create and use Person structure
print("--- Person Structure ---");
var person = Person { "Alice", 25, 5.6 };

print("Person Details:");
print("  Name: ", person.name);
print("  Age: ", person.age, " years");
print("  Height: ", person.height, " feet");

print("=== Structure examples completed! ===");