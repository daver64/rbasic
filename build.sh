#!/bin/bash

# rbasic Linux Build Script
# Usage: ./build.sh [options]
# Options:
#   -d, --debug     Build in Debug mode instead of Release
#   -c, --clean     Clean build directory before building
#   -j, --jobs N    Use N parallel jobs for building (default: auto-detect)
# Future: FFI (Foreign Function Interface) support will be added
#   -h, --help      Show this help message

BUILD_TYPE="Release"
CLEAN_BUILD=false
PARALLEL_JOBS=""
# No optional features currently enabled

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -h|--help)
            echo "rbasic Linux Build Script"
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  -d, --debug     Build in Debug mode instead of Release"
            echo "  -c, --clean     Clean build directory before building"
            echo "  -j, --jobs N    Use N parallel jobs for building (default: auto-detect)"
            echo "  Future: FFI (Foreign Function Interface) support"
            echo "  -h, --help      Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                    # Standard release build"
            echo "  $0 -d                 # Debug build"
            echo "  $0 -c -j4             # Clean build with 4 parallel jobs"
            echo "  $0 debug --jobs 8     # Debug build with parallel compilation"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

echo "Building rbasic ($BUILD_TYPE)..."

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Change to build directory
cd build

# Prepare CMake options
CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

# Configure with CMake
echo "Configuring with CMake..."
cmake .. $CMAKE_OPTIONS

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Determine parallel jobs
if [ -z "$PARALLEL_JOBS" ]; then
    # Auto-detect CPU cores
    if command -v nproc &> /dev/null; then
        PARALLEL_JOBS=$(nproc)
    else
        PARALLEL_JOBS=4  # Fallback
    fi
fi

# Build the project
echo "Building with $PARALLEL_JOBS parallel jobs..."
cmake --build . --config $BUILD_TYPE -j $PARALLEL_JOBS

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Go back to project root
cd ..

echo ""
echo "✅ Build complete! The rbasic executable should be in the project root."
echo ""
echo "To test:"
echo "  ./rbasic -i examples/hello.bas"
echo "  ./rbasic -c examples/hello.bas"
echo ""
echo "To run tests:"
echo "  ./rbasic_tests"