// Debug FFI call
print("Step 1: Declaring FFI function");
ffi int GetCurrentProcessId() from "kernel32";
print("Step 2: FFI function declared");

print("Step 3: Attempting to call GetCurrentProcessId()");
try {
    var pid = GetCurrentProcessId();
    print("Step 4: Success! PID = " + string(pid));
} catch (error) {
    print("Step 4: Error calling function: " + error);
}