// Example showing functions and structures in rbasic

// Define a Point structure
struct Point
    x as integer;
    y as integer;
end struct;

// Define a function to calculate distance
function distance(p1 as Point, p2 as Point) as double
    var dx = p1.x - p2.x;
    var dy = p1.y - p2.y;
    return sqr(dx * dx + dy * dy);
end function;

// Define a function to create a point
function makePoint(x as integer, y as integer) as Point
    dim result as Point;
    var result.x = x;
    var result.y = y;
    return result;
end function;

// Main program
print "Point and Distance Calculator";

dim point1 as Point;
dim point2 as Point;

var point1 = makePoint(0, 0);
var point2 = makePoint(3, 4);

print "Point 1: ("; point1.x; ", "; point1.y; ")";
print "Point 2: ("; point2.x; ", "; point2.y; ")";

var dist = distance(point1, point2);
print "Distance: "; dist;