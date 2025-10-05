// Even simpler test - just call the function without assignment
print("About to call GetCurrentProcessId directly");
ffi int GetCurrentProcessId() from "kernel32";
GetCurrentProcessId();