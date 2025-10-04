cmake_minimum_required(VERSION 3.16)
project(rbasic VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

# Include directories
include_directories(include)

# Optional SDL2 support
option(ENABLE_SDL_SUPPORT "Enable SDL2 graphics support" OFF)

if(ENABLE_SDL_SUPPORT)
    # Try to find SDL2 using environment variables
    if(DEFINED ENV{SDL2_ROOT})
        set(SDL2_DIR "$ENV{SDL2_ROOT}")
        find_path(SDL2_INCLUDE_DIRS SDL.h PATHS ${SDL2_DIR}/include)
        find_library(SDL2_LIBRARIES SDL2 PATHS ${SDL2_DIR}/lib/x64)
        find_library(SDL2_MAIN_LIBRARIES SDL2main PATHS ${SDL2_DIR}/lib/x64)
        find_library(SDL2_TTF_LIBRARIES SDL2_ttf PATHS ${SDL2_DIR}/lib/x64)
        
        if(SDL2_INCLUDE_DIRS AND SDL2_LIBRARIES AND SDL2_TTF_LIBRARIES)
            set(SDL2_FOUND TRUE)
            message(STATUS "SDL2 found: ${SDL2_DIR}")
        else()
            message(WARNING "SDL2_ROOT is set but SDL2 or SDL2_ttf not found")
            set(SDL2_FOUND FALSE)
        endif()
    else()
        # Try standard CMake FindSDL2
        find_package(SDL2 QUIET)
    endif()
    
    if(SDL2_FOUND)
        add_compile_definitions(RBASIC_SDL_SUPPORT)
        message(STATUS "SDL2 graphics support enabled")
    else()
        message(WARNING "SDL2 not found, building without graphics support")
    endif()
endif()

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
)

# Add SDL2 sources if enabled
if(ENABLE_SDL_SUPPORT AND SDL2_FOUND)
    list(APPEND SOURCES src/sdl_io_handler.cpp)
    list(APPEND HEADERS include/sdl_io_handler.h)
endif()

# Create main executable
add_executable(rbasic ${SOURCES} ${HEADERS})

# Link SDL2 if available
if(ENABLE_SDL_SUPPORT AND SDL2_FOUND)
    target_include_directories(rbasic PRIVATE ${SDL2_INCLUDE_DIRS})
    if(SDL2_MAIN_LIBRARIES AND SDL2_TTF_LIBRARIES)
        target_link_libraries(rbasic ${SDL2_LIBRARIES} ${SDL2_MAIN_LIBRARIES} ${SDL2_TTF_LIBRARIES})
    else()
        target_link_libraries(rbasic ${SDL2_LIBRARIES})
    endif()
    
    if(WIN32)
        # Copy SDL2 DLLs to output directory on Windows
        if(EXISTS "${SDL2_DIR}/lib/x64/SDL2.dll")
            add_custom_command(TARGET rbasic POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SDL2_DIR}/lib/x64/SDL2.dll"
                $<TARGET_FILE_DIR:rbasic>)
        endif()
        if(EXISTS "${SDL2_DIR}/lib/x64/SDL2_ttf.dll")
            add_custom_command(TARGET rbasic POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SDL2_DIR}/lib/x64/SDL2_ttf.dll"
                $<TARGET_FILE_DIR:rbasic>)
        endif()
    endif()
endif()

# Create runtime library for compiled programs
add_library(rbasic_runtime STATIC
    runtime/basic_runtime.cpp
    runtime/basic_runtime.h
)

# Add SDL2 support to runtime library if enabled
if(ENABLE_SDL_SUPPORT AND SDL2_FOUND)
    target_include_directories(rbasic_runtime PRIVATE ${SDL2_INCLUDE_DIRS})
    target_compile_definitions(rbasic_runtime PRIVATE RBASIC_SDL_SUPPORT)
    # Note: SDL2 libraries will be linked by the final executable, not the static library
endif()

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
)

target_include_directories(rbasic_tests PRIVATE include)

# Add SDL2 support to tests if enabled
if(ENABLE_SDL_SUPPORT AND SDL2_FOUND)
    target_include_directories(rbasic_tests PRIVATE ${SDL2_INCLUDE_DIRS})
    target_compile_definitions(rbasic_tests PRIVATE RBASIC_SDL_SUPPORT)
    list(APPEND TEST_SOURCES src/sdl_io_handler.cpp)
    target_sources(rbasic_tests PRIVATE ${TEST_SOURCES})
    if(SDL2_MAIN_LIBRARIES AND SDL2_TTF_LIBRARIES)
        target_link_libraries(rbasic_tests ${SDL2_LIBRARIES} ${SDL2_MAIN_LIBRARIES} ${SDL2_TTF_LIBRARIES})
    else()
        target_link_libraries(rbasic_tests ${SDL2_LIBRARIES})
    endif()
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