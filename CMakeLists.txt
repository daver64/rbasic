cmake_minimum_required(VERSION 3.16)
project(rbasic VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    src/console_io_handler.cpp
    src/command_builder.cpp
    src/type_utils.cpp
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
)

# Create main executable
add_executable(rbasic ${SOURCES} ${HEADERS})

# Create runtime library for compiled programs
add_library(rbasic_runtime STATIC
    runtime/basic_runtime.cpp
    runtime/basic_runtime.h
)

# Set runtime library output to runtime directory
set_target_properties(rbasic_runtime PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime
)

# Compiler-specific options
if(MSVC)
    target_compile_options(rbasic PRIVATE /W4)
    target_compile_options(rbasic_runtime PRIVATE /W4)
else()
    target_compile_options(rbasic PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(rbasic_runtime PRIVATE -Wall -Wextra -pedantic)
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
)

target_include_directories(rbasic_tests PRIVATE include)

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