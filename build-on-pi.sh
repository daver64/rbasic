#!/bin/bash
# Native build script for Raspberry Pi
# Run this directly on the Pi for guaranteed compatibility

set -e

echo "Building rbasic natively on Raspberry Pi..."
echo "This ensures perfect GLIBC compatibility."
echo ""

# Clean build directory
rm -rf build
mkdir -p build
cd build

# Configure for native ARM build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with all cores
make -j$(nproc)

echo ""
echo "Build complete!"
echo "Binary: $(pwd)/../rbasic"
echo ""
echo "Test with: ./rbasic --help"
