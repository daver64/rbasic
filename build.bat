@echo off
echo Building rbasic...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Change to build directory
cd build

REM Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build the project
cmake --build . --config Release

REM Go back to project root
cd ..

echo Build complete! The rbasic executable should be in the project root.
echo.
echo To test:
echo   rbasic -i examples\hello.bas
echo   rbasic -c examples\hello.bas