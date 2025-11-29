set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_SYSROOT $ENV{HOME}/pi-sysroot)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc-12)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++-12)

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Critical: Force all compilation and linking through sysroot
# Disable C23 ISO features that require newer GLIBC
set(CMAKE_C_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT} -D_GNU_SOURCE")
set(CMAKE_CXX_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT} -Wl,--rpath-link=${CMAKE_SYSROOT}/lib/aarch64-linux-gnu:${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

