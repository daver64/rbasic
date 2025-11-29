cmake_minimum_required(VERSION 3.16)
project(rbasic VERSION 1.0.0 LANGUAGES CXX C)

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found - enabling parallel array operations")
else()
    message(STATUS "OpenMP not found - using serial array operations")
endif()

# Require C++17 as a minimum
if(CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "C++17 or higher is required")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

# Set C standard to C11 to avoid C23 GLIBC dependencies
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Ensure C++17 is used on all platforms
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

# Include directories
include_directories(include)

# Detect ARM architecture (Raspberry Pi)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm.*|ARM.*|aarch64.*|AARCH64.*")
    set(BUILD_RPI_SUPPORT ON)
    message(STATUS "ARM architecture detected - enabling Raspberry Pi GPIO support")
else()
    set(BUILD_RPI_SUPPORT OFF)
    message(STATUS "Non-ARM architecture (${CMAKE_SYSTEM_PROCESSOR}) - Raspberry Pi GPIO support disabled")
endif()

# Optional: Allow manual override
option(FORCE_RPI_SUPPORT "Force enable Raspberry Pi support even on non-ARM platforms" OFF)
if(FORCE_RPI_SUPPORT)
    set(BUILD_RPI_SUPPORT ON)
    message(STATUS "Raspberry Pi support force-enabled")
endif()

option(DISABLE_RPI_SUPPORT "Force disable Raspberry Pi support even on ARM platforms" OFF)
if(DISABLE_RPI_SUPPORT)
    set(BUILD_RPI_SUPPORT OFF)
    message(STATUS "Raspberry Pi support force-disabled")
endif()

# Add preprocessor definition
if(BUILD_RPI_SUPPORT)
    add_compile_definitions(RPI_SUPPORT_ENABLED)
    message(STATUS "RPI_SUPPORT_ENABLED defined")
endif()

# Core language build - no external dependencies

# Source files
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/interpreter.cpp
    src/codegen.cpp
    src/runtime.cpp
    src/common.cpp
    src/io_handler.cpp
    src/console_io_handler.cpp
    src/command_builder.cpp
    src/type_utils.cpp
    src/terminal.cpp
    src/repl.cpp
    src/math_utils.cpp
    src/memory_manager.cpp
    src/unified_value.cpp
    src/glibc_compat.c
)

# Raspberry Pi support sources (conditional)
if(BUILD_RPI_SUPPORT)
    list(APPEND SOURCES
        src/rpi_gpio.cpp
        src/rpi_spi.cpp
        src/rpi_i2c.cpp
        src/rpi_pwm.cpp
        src/rpi_serial.cpp
    )
endif()

# Header files
set(HEADERS
    include/lexer.h
    include/parser.h
    include/ast.h
    include/interpreter.h
    include/codegen.h
    include/runtime.h
    include/common.h
    include/io_handler.h
    include/console_io_handler.h
    include/command_builder.h
    include/type_utils.h
    include/terminal.h
    include/repl.h
    include/math_utils.h
    include/memory_manager.h
    include/unified_value.h
)

# Raspberry Pi support headers (conditional)
if(BUILD_RPI_SUPPORT)
    list(APPEND HEADERS
        include/rpi_gpio.h
        include/rpi_spi.h
        include/rpi_i2c.h
        include/rpi_pwm.h
        include/rpi_serial.h
    )
endif()

# Create main executable
add_executable(rbasic ${SOURCES} ${HEADERS})

# Create runtime library for compiled programs
add_library(rbasic_runtime STATIC
    runtime/basic_runtime.cpp
    runtime/basic_runtime.h
    src/terminal.cpp
    include/terminal.h
    src/memory_manager.cpp
    include/memory_manager.h
    src/unified_value.cpp
    include/unified_value.h
)

# Add Raspberry Pi support to runtime (conditional)
if(BUILD_RPI_SUPPORT)
    target_sources(rbasic_runtime PRIVATE
        src/rpi_gpio.cpp
        include/rpi_gpio.h
        src/rpi_spi.cpp
        include/rpi_spi.h
        src/rpi_i2c.cpp
        include/rpi_i2c.h
        src/rpi_pwm.cpp
        include/rpi_pwm.h
        src/rpi_serial.cpp
        include/rpi_serial.h
    )
endif()

# Include directories for runtime library
target_include_directories(rbasic_runtime PRIVATE include)

# Set runtime library output to runtime directory
set_target_properties(rbasic_runtime PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime
)

# Link with required libraries
target_link_libraries(rbasic rbasic_runtime)
if(OpenMP_CXX_FOUND)
    target_link_libraries(rbasic OpenMP::OpenMP_CXX)
    target_link_libraries(rbasic_runtime OpenMP::OpenMP_CXX)
endif()

if(NOT WIN32)
    # On Linux/Unix, link with stdc++fs for filesystem support (GCC < 9)
    target_link_libraries(rbasic stdc++fs)
    target_link_libraries(rbasic_runtime stdc++fs)
endif()

# Enable testing
enable_testing()

# Test executable
add_executable(rbasic_tests
    tests/test_main.cpp
    tests/lexer_tests.cpp
    tests/parser_tests.cpp
    tests/integration_tests.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/interpreter.cpp
    src/runtime.cpp
    src/common.cpp
    src/io_handler.cpp
    src/console_io_handler.cpp
    src/command_builder.cpp
    src/type_utils.cpp
    src/terminal.cpp
    src/repl.cpp
    src/math_utils.cpp
    src/memory_manager.cpp
    src/unified_value.cpp
)

target_include_directories(rbasic_tests PRIVATE include)

# Link rbasic_tests with required libraries
target_link_libraries(rbasic_tests rbasic_runtime)
if(OpenMP_CXX_FOUND)
    target_link_libraries(rbasic_tests OpenMP::OpenMP_CXX)
endif()

if(NOT WIN32)
    target_link_libraries(rbasic_tests stdc++fs)
endif()

# OpenMP definitions are automatically provided by OpenMP::OpenMP_CXX
# No need to manually define _OPENMP as it's automatically defined by the compiler

# Compiler-specific options (set after all targets are defined)
if(MSVC)
    target_compile_options(rbasic PRIVATE /W4)
    target_compile_options(rbasic_runtime PRIVATE /W4)
    target_compile_options(rbasic_tests PRIVATE /W4)
else()
    # Ensure C++17 is explicitly set for GCC/Clang
    target_compile_options(rbasic PRIVATE -std=c++17 -Wall -Wextra -pedantic)
    target_compile_options(rbasic_runtime PRIVATE -std=c++17 -Wall -Wextra -pedantic)
    target_compile_options(rbasic_tests PRIVATE -std=c++17 -Wall -Wextra -pedantic)
    
    # Add additional useful warnings for better code quality
    target_compile_options(rbasic PRIVATE -Wunused -Wuninitialized -Wshadow)
    target_compile_options(rbasic_runtime PRIVATE -Wunused -Wuninitialized -Wshadow)
    target_compile_options(rbasic_tests PRIVATE -Wunused -Wuninitialized -Wshadow)
endif()

# Set MSVC runtime library for all targets after they're defined
if(MSVC)
    # Use static runtime library for consistency
    set_property(TARGET rbasic PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET rbasic_runtime PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET rbasic_tests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_test(NAME unit_tests COMMAND rbasic_tests)

# Install targets
install(TARGETS rbasic DESTINATION bin)
install(TARGETS rbasic_runtime DESTINATION lib)