// Math performance test - simple version
var i;

print("Starting math operations test...");

for (i = 0; i < 1000; i = i + 1) {
    var result = sqrt(i + 1);
    result = sin(result);
    result = cos(result);
    result = log(result + 1);
    result = exp(result);
}

print("Math operations completed successfully!");
print("Final result: " + result);