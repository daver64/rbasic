// Test single parameter FFI function - properly structured
ffi "kernel32.dll" Sleep(milliseconds as integer) as integer;

function main() {
    print("Testing single parameter FFI function...");
    print("Sleeping for 1 second...");
    
    var result = Sleep(1000);
    
    print("Sleep completed!");
    print("Sleep result: ");
    print(result);
    
    return 0;
}

main();