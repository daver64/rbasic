// duplicate_test.bas - Test duplicate import handling

import "simple_lib.bas";
print("First import done\n");

import "simple_lib.bas";  // This should be skipped
print("Second import done\n");

print("Duplicate import test completed!");