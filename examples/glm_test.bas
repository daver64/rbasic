// GLM vector math test - comprehensive testing of vector operations
print("=== GLM Vector Math Test ===");

// Test vec2 creation and component access
var position = vec2(10.0, 20.0);
print("vec2 position: (" + position.x + ", " + position.y + ")");

// Test vec3 with component assignment
var velocity = vec3(1.0, 2.0, 3.0);
print("Original velocity: (" + velocity.x + ", " + velocity.y + ", " + velocity.z + ")");
velocity.x = 5.0;
velocity.z = 10.0;
print("Modified velocity: (" + velocity.x + ", " + velocity.y + ", " + velocity.z + ")");

// Test vec4 creation
var colour = vec4(0.8, 0.6, 0.4, 1.0);
print("Colour RGBA: (" + colour.x + ", " + colour.y + ", " + colour.z + ", " + colour.w + ")");

// Test vector addition
var pos1 = vec3(1.0, 2.0, 3.0);
var pos2 = vec3(4.0, 5.0, 6.0);
var sum = pos1 + pos2;
print("Addition: (" + pos1.x + ", " + pos1.y + ", " + pos1.z + ") + (" + pos2.x + ", " + pos2.y + ", " + pos2.z + ") = (" + sum.x + ", " + sum.y + ", " + sum.z + ")");

// Test vector subtraction
var diff = pos2 - pos1;
print("Subtraction: (" + pos2.x + ", " + pos2.y + ", " + pos2.z + ") - (" + pos1.x + ", " + pos1.y + ", " + pos1.z + ") = (" + diff.x + ", " + diff.y + ", " + diff.z + ")");

// Test scalar multiplication
var direction = vec3(1.0, 0.0, 0.0);
var scaled = direction * 5.0;
print("Scalar multiplication: (" + direction.x + ", " + direction.y + ", " + direction.z + ") * 5.0 = (" + scaled.x + ", " + scaled.y + ", " + scaled.z + ")");

// Test vector scaling (reverse order)
var scaled2 = 2.0 * direction;
print("Reverse scalar multiplication: 2.0 * (" + direction.x + ", " + direction.y + ", " + direction.z + ") = (" + scaled2.x + ", " + scaled2.y + ", " + scaled2.z + ")");

// Test GLM functions
var v1 = vec3(3.0, 4.0, 0.0);
var len = length(v1);
print("Length of (3, 4, 0): " + len);

var normalized = normalize(v1);
print("Normalized (3, 4, 0): (" + normalized.x + ", " + normalized.y + ", " + normalized.z + ")");

// Test dot product
var v2 = vec3(1.0, 0.0, 0.0);
var dotResult = dot(v1, v2);
print("Dot product of (" + v1.x + ", " + v1.y + ", " + v1.z + ") and (" + v2.x + ", " + v2.y + ", " + v2.z + "): " + dotResult);

// Test cross product
var v3 = vec3(0.0, 1.0, 0.0);
var crossResult = cross(v2, v3);
print("Cross product of (" + v2.x + ", " + v2.y + ", " + v2.z + ") and (" + v3.x + ", " + v3.y + ", " + v3.z + "): (" + crossResult.x + ", " + crossResult.y + ", " + crossResult.z + ")");

// Test complex expression with component assignment in computation
var result = vec3(0.0, 0.0, 0.0);
result.x = pos1.x + pos2.x;
result.y = pos1.y * 2.0;
result.z = length(vec3(pos2.x, pos2.y, 0.0));
print("Complex computation result: (" + result.x + ", " + result.y + ", " + result.z + ")");

print("=== All GLM tests completed ===");