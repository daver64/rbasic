// Modern BASIC syntax test
var name = "World";
var number = 42;

print("Hello,", name);
print("The answer is", number);

// Math functions test
var x = 16;
print("Square root of", x, "is", sqrt(x));
print("Sine of 1 is", sin(1));
print("Pi is", pi());

// Control flow with parentheses
for(var i = 1; i <= 3; i = i + 1)
    print("Loop iteration:", i);
end;

var j = 1;
while(j <= 2)
    print("While loop:", j);
    j = j + 1;
wend;

print("Enter a number:");
var userInput = input();
print("You entered:", userInput);