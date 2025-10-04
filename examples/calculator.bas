// Simple calculator
print "Simple Calculator";
print "Enter first number:";
input a;
print "Enter second number:";
input b;

print "Choose operation:";
print "1. Addition";
print "2. Subtraction";
print "3. Multiplication";
print "4. Division";
input choice;

if choice = 1 then
    var result = a + b;
    print "Result: "; a; " + "; b; " = "; result;
else
    if choice = 2 then
        var result = a - b;
        print "Result: "; a; " - "; b; " = "; result;
    else
        if choice = 3 then
            var result = a * b;
            print "Result: "; a; " * "; b; " = "; result;
        else
            if choice = 4 then
                if b <> 0 then
                    var result = a / b;
                    print "Result: "; a; " / "; b; " = "; result;
                else
                    print "Error: Division by zero!";
                end if;
            else
                print "Invalid choice!";
            end if;
        end if;
    end if;
end if;