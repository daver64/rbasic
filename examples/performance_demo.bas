// Performance demonstration showing automatic OpenMP parallelization
// Arrays with 1000+ elements automatically use multi-core execution

print("=== Performance Demo: Automatic Parallelization ===");
print();

// Small array - uses serial execution (fast startup)
print("Creating small array (500 elements) - serial execution...");
dim small_array(500);
for (var i = 0; i < 500; i = i + 1) {
    small_array[i] = i * i;
}
print("Small array completed");
print();

// Large array - automatically uses parallel execution  
print("Creating large array (5000 elements) - parallel execution...");
dim large_array(5000);
for (var i = 0; i < 5000; i = i + 1) {
    large_array[i] = i * i;  // This runs in parallel automatically!
}
print("Large array completed");
print();

// Typed arrays also benefit from parallelization
print("Testing typed arrays with parallelization...");
var fast_doubles = double_array(3000);
for (var i = 0; i < 3000; i = i + 1) {
    fast_doubles[i] = sin(i * 0.001) * cos(i * 0.002);  // Parallel math!
}
print("Typed array with mathematical operations completed");
print();

// Verify some results
print("Verification:");
print("small_array[10] =", small_array[10]);
print("large_array[100] =", large_array[100]);
print("fast_doubles[500] =", fast_doubles[500]);
print();

print("=== Parallelization Features ===");
print("* Automatic - no code changes needed");
print("* Smart thresholding - only large arrays (>=1000 elements)");
print("* Cross-platform OpenMP support");
print("* Works in both interpreter and compiled modes");
print();
print("For maximum performance: ./rbasic -c performance_demo.bas -o fast_demo");
print("Parallelization is completely invisible to the programmer!");