// simple_import_test.bas - Test the import functionality with simple library

import "simple_lib.bas";

var result = hello_world();
print("Function returned: ");
print(result);
print("\n");

print("Library constant: ");
print(library_constant);
print("\n");

print("Simple import test completed!");