// Example showing function usage in rbasic with modern syntax

// Function to calculate factorial
function factorial(n)
    if n <= 1 then
        return 1;
    else
        return n * factorial(n - 1);
    end;
end;

// Function to check if a number is prime
function isPrime(n)
    if n < 2 then
        return false;
    end;
    
    for(var i = 2; i <= sqrt(n); i = i + 1)
        if n mod i == 0 then
            return false;
        end;
    end;
    
    return true;
end;

// Function to calculate greatest common divisor
function gcd(a, b)
    while(b != 0)
        var temp = b;
        b = a mod b;
        a = temp;
    wend;
    return a;
end;

// Main program
print("Math Functions Demo");
print("");

print("Testing factorial function:");
print("Factorial of 5 is", factorial(5));
print("");

print("Prime numbers from 1 to 20:");
for(var i = 1; i <= 20; i = i + 1)
    if isPrime(i) then
        print(i, "is prime");
    end;
end;
print("");

print("GCD of 48 and 18:", gcd(48, 18));