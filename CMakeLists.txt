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
    src/ffi.cpp
    src/console_io_handler.cpp
    src/command_builder.cpp
    src/type_utils.cpp
    src/terminal.cpp
    src/repl.cpp
    src/math_utils.cpp
    src/ffi_utils.cpp
    src/memory_manager.cpp
    src/unified_value.cpp
    src/safe_ffi.cpp
)

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
    include/ffi.h
    include/math_utils.h
    include/ffi_utils.h
    include/memory_manager.h
    include/unified_value.h
    include/safe_ffi.h
)

# Create main executable
add_executable(rbasic ${SOURCES} ${HEADERS})

# Create runtime library for compiled programs
add_library(rbasic_runtime STATIC
    runtime/basic_runtime.cpp
    runtime/basic_runtime.h
    src/terminal.cpp
    include/terminal.h
    src/ffi.cpp
    include/ffi.h
    src/ffi_utils.cpp
    include/ffi_utils.h
    src/memory_manager.cpp
    include/memory_manager.h
    src/unified_value.cpp
    include/unified_value.h
    src/safe_ffi.cpp
    include/safe_ffi.h
)

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
    # Link with dl for dynamic library loading
    target_link_libraries(rbasic dl)
    target_link_libraries(rbasic_runtime dl)
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
    src/ffi_utils.cpp
    src/memory_manager.cpp
    src/unified_value.cpp
    src/safe_ffi.cpp
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

# Add OpenMP compile definitions
if(OpenMP_CXX_FOUND)
    target_compile_definitions(rbasic PRIVATE _OPENMP)
    target_compile_definitions(rbasic_runtime PRIVATE _OPENMP)
    target_compile_definitions(rbasic_tests PRIVATE _OPENMP)
endif()

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