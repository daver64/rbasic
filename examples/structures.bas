// Example showing structure functionality in rbasic

// Define a Point structure
struct Point
    x as integer;
    y as integer;
end struct;

// Define a Person structure  
struct Person
    name as string;
    age as integer;
    height as double;
end struct;

print("=== rbasic Structure Examples ===");

// Create and use Point structure
print("\n--- Point Structure ---");
dim p1 as Point;
dim p2 as Point;

var p1.x = 0;
var p1.y = 0;
var p2.x = 3;
var p2.y = 4;

print("Point 1: (", p1.x, ", ", p1.y, ")");
print("Point 2: (", p2.x, ", ", p2.y, ")");

// Create and use Person structure
print("\n--- Person Structure ---");
dim person as Person;

var person.name = "Alice";
var person.age = 25;
var person.height = 5.6;

print("Person Details:");
print("  Name: ", person.name);
print("  Age: ", person.age, " years");
print("  Height: ", person.height, " feet");

print("\n=== Structure examples completed! ===");