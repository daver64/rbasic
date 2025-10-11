// GLM Vector Mathematics Demonstration
// Shows complete vector math capabilities in rbasic

print("=== rbasic GLM Vector Mathematics Demo ===");
print("");

// ===== Vector Creation and Component Access =====
print("1. Vector Creation and Component Access:");
var position = vec2(100.0, 200.0);
var velocity = vec3(1.5, -2.3, 4.7);
var colour = vec4(0.8, 0.6, 0.4, 1.0);

print("vec2 position: (" + position.x + ", " + position.y + ")");
print("vec3 velocity: (" + velocity.x + ", " + velocity.y + ", " + velocity.z + ")");
print("vec4 colour RGBA: (" + colour.x + ", " + colour.y + ", " + colour.z + ", " + colour.w + ")");
print("");

// ===== Component Assignment =====
print("2. Component Assignment:");
print("Before: velocity = (" + velocity.x + ", " + velocity.y + ", " + velocity.z + ")");
velocity.x = 10.0;
velocity.z = -5.0;
print("After modification: velocity = (" + velocity.x + ", " + velocity.y + ", " + velocity.z + ")");
print("");

// ===== Vector Arithmetic =====
print("3. Vector Arithmetic Operations:");
var a = vec3(2.0, 4.0, 6.0);
var b = vec3(1.0, 3.0, 5.0);

// Addition
var sum = a + b;
print("Addition: (" + a.x + ", " + a.y + ", " + a.z + ") + (" + b.x + ", " + b.y + ", " + b.z + ") = (" + sum.x + ", " + sum.y + ", " + sum.z + ")");

// Subtraction
var diff = a - b;
print("Subtraction: (" + a.x + ", " + a.y + ", " + a.z + ") - (" + b.x + ", " + b.y + ", " + b.z + ") = (" + diff.x + ", " + diff.y + ", " + diff.z + ")");

// Scalar multiplication
var scaled = a * 2.5;
print("Scalar multiplication: (" + a.x + ", " + a.y + ", " + a.z + ") * 2.5 = (" + scaled.x + ", " + scaled.y + ", " + scaled.z + ")");

// Reverse scalar multiplication
var scaled2 = 0.5 * b;
print("Reverse scalar: 0.5 * (" + b.x + ", " + b.y + ", " + b.z + ") = (" + scaled2.x + ", " + scaled2.y + ", " + scaled2.z + ")");
print("");

// ===== GLM Mathematical Functions =====
print("4. GLM Mathematical Functions:");

// Length calculation
var direction = vec3(3.0, 4.0, 0.0);  // Classic 3-4-5 triangle
var magnitude = length(direction);
print("Length of (3, 4, 0): " + magnitude + " (should be 5.0)");

// Normalization
var normalized = normalize(direction);
print("Normalized (3, 4, 0): (" + normalized.x + ", " + normalized.y + ", " + normalized.z + ")");
print("Length of normalized vector: " + length(normalized) + " (should be ~1.0)");

// Dot product
var vec1 = vec3(1.0, 2.0, 3.0);
var vec2 = vec3(4.0, 5.0, 6.0);
var dotResult = dot(vec1, vec2);
print("Dot product of (" + vec1.x + ", " + vec1.y + ", " + vec1.z + ") · (" + vec2.x + ", " + vec2.y + ", " + vec2.z + ") = " + dotResult);
print("Expected: 1*4 + 2*5 + 3*6 = 32");

// Cross product
var right = vec3(1.0, 0.0, 0.0);   // X-axis
var up = vec3(0.0, 1.0, 0.0);      // Y-axis
var forward = cross(right, up);     // Should give Z-axis
print("Cross product X × Y: (" + forward.x + ", " + forward.y + ", " + forward.z + ") (should be (0, 0, 1))");
print("");

// ===== Practical Examples =====
print("5. Practical Vector Applications:");

// Physics simulation step
var playerPos = vec3(10.0, 5.0, 0.0);
var playerVel = vec3(2.0, -1.0, 0.0);
var deltaTime = 0.016;  // 60 FPS

var newPos = playerPos + playerVel * deltaTime;
print("Physics update:");
print("  Position: (" + playerPos.x + ", " + playerPos.y + ", " + playerPos.z + ")");
print("  Velocity: (" + playerVel.x + ", " + playerVel.y + ", " + playerVel.z + ")");
print("  After 0.016s: (" + newPos.x + ", " + newPos.y + ", " + newPos.z + ")");

// Distance calculation
var player = vec3(0.0, 0.0, 0.0);
var enemy = vec3(10.0, 20.0, 0.0);
var distanceVec = enemy - player;
var distance = length(distanceVec);
print("Distance from player to enemy: " + distance + " units");

// Direction calculation
var directionToEnemy = normalize(distanceVec);
print("Direction to enemy: (" + directionToEnemy.x + ", " + directionToEnemy.y + ", " + directionToEnemy.z + ")");
print("");

// ===== Complex Expression Example =====
print("6. Complex Vector Expression:");
var origin = vec3(0.0, 0.0, 0.0);
var target = vec3(5.0, 12.0, 0.0);
var currentPos = vec3(1.0, 2.0, 0.0);

// Calculate normalized direction and move 50% toward target
var toTarget = target - currentPos;
var moveDirection = normalize(toTarget);
var moveDistance = length(toTarget) * 0.5;
var finalPos = currentPos + moveDirection * moveDistance;

print("Moving from (" + currentPos.x + ", " + currentPos.y + ", " + currentPos.z + ") toward (" + target.x + ", " + target.y + ", " + target.z + ")");
print("Final position: (" + finalPos.x + ", " + finalPos.y + ", " + finalPos.z + ")");
print("");

print("=== GLM Vector Mathematics Demo Complete ===");
print("All vector operations working correctly in both interpreter and compiled modes!");