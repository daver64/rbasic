#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <algorithm>  // For std::find
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdint>  // For uint8_t
#include <fstream>  // For file operations
#include <filesystem>  // For filesystem operations

// GLM includes for vector and matrix types
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Forward declarations
struct BasicStruct;
struct BasicArray;
struct BasicByteArray;
struct BasicIntArray;
struct BasicDoubleArray;
struct BasicPointer;

// GLM value wrappers for runtime
struct BasicVec2 {
    glm::vec2 data;
    BasicVec2() : data(0.0f) {}
    BasicVec2(float x, float y) : data(x, y) {}
    BasicVec2(const glm::vec2& v) : data(v) {}
};

struct BasicVec3 {
    glm::vec3 data;
    BasicVec3() : data(0.0f) {}
    BasicVec3(float x, float y, float z) : data(x, y, z) {}
    BasicVec3(const glm::vec3& v) : data(v) {}
};

struct BasicVec4 {
    glm::vec4 data;
    BasicVec4() : data(0.0f) {}
    BasicVec4(float x, float y, float z, float w) : data(x, y, z, w) {}
    BasicVec4(const glm::vec4& v) : data(v) {}
};

struct BasicMat3 {
    glm::mat3 data;
    BasicMat3() : data(1.0f) {}  // Identity matrix
    BasicMat3(const glm::mat3& m) : data(m) {}
};

struct BasicMat4 {
    glm::mat4 data;
    BasicMat4() : data(1.0f) {}  // Identity matrix
    BasicMat4(const glm::mat4& m) : data(m) {}
};

struct BasicQuat {
    glm::quat data;
    BasicQuat() : data(1.0f, 0.0f, 0.0f, 0.0f) {}  // Identity quaternion
    BasicQuat(float w, float x, float y, float z) : data(w, x, y, z) {}
    BasicQuat(const glm::quat& q) : data(q) {}
};

// Value type for compiled BASIC programs
using BasicValue = std::variant<int, double, std::string, bool, void*, BasicStruct, BasicArray, BasicByteArray, BasicIntArray, BasicDoubleArray, BasicVec2, BasicVec3, BasicVec4, BasicMat3, BasicMat4, BasicQuat>;

// Pointer wrapper for FFI
struct BasicPointer {
    void* ptr;
    std::string typeName;  // Optional type information
    
    BasicPointer() : ptr(nullptr) {}
    BasicPointer(void* p, const std::string& type = "") : ptr(p), typeName(type) {}
};

// Structure type
struct BasicStruct {
    std::string typeName;
    std::map<std::string, BasicValue> fields;
    
    BasicStruct() = default;
    BasicStruct(const std::string& type) : typeName(type) {}
};

// Array type
struct BasicArray {
    std::vector<BasicValue> elements;
    std::vector<int> dimensions;
    
    BasicArray() = default;
    BasicArray(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize);
    }
    
    BasicValue& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const BasicValue& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

// Typed arrays for better performance with homogeneous data
struct BasicByteArray {
    std::vector<uint8_t> elements;
    std::vector<int> dimensions;
    
    BasicByteArray() = default;
    BasicByteArray(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize, 0);
    }
    
    uint8_t& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const uint8_t& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

struct BasicIntArray {
    std::vector<int> elements;
    std::vector<int> dimensions;
    
    BasicIntArray() = default;
    BasicIntArray(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize, 0);
    }
    
    int& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const int& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

struct BasicDoubleArray {
    std::vector<double> elements;
    std::vector<int> dimensions;
    
    BasicDoubleArray() = default;
    BasicDoubleArray(const std::vector<int>& dims) : dimensions(dims) {
        int totalSize = 1;
        for (int dim : dims) {
            totalSize *= dim;
        }
        elements.resize(totalSize, 0.0);
    }
    
    double& at(const std::vector<int>& indices) {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const double& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = static_cast<int>(dimensions.size()) - 1; i >= 0; i--) {
            index += indices[i] * multiplier;  // 0-indexed arrays
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
};

// Library handle for FFI - Temporarily disabled for Phase 1
/*
struct BasicLibraryHandle {
    std::string name;
    void* handle;  // Platform-specific library handle
    
    BasicLibraryHandle() : handle(nullptr) {}
    BasicLibraryHandle(const std::string& lib_name, void* lib_handle) 
        : name(lib_name), handle(lib_handle) {}
    
    bool is_valid() const { return handle != nullptr; }
};
*/

// Forward declaration for IOHandler
namespace rbasic {
    class IOHandler;
}

// Runtime functions for compiled BASIC programs
namespace basic_runtime {

// I/O functions (now using IOHandler)
void init_io_handler(rbasic::IOHandler* handler);
rbasic::IOHandler* get_io_handler();
void print(const BasicValue& value);
void print_line();
void debug_print(const BasicValue& value);
BasicValue input();

// String functions  
int len(const BasicValue& str);
BasicValue mid(const BasicValue& str, int start, int length = -1);
BasicValue left(const BasicValue& str, int length);
BasicValue right(const BasicValue& str, int length);
BasicValue val(const BasicValue& str);  // Convert string to number

// Math functions
BasicValue abs_val(const BasicValue& value);
BasicValue sqrt_val(const BasicValue& value);
BasicValue sqr_val(const BasicValue& value);  // Square root (SQR function)
BasicValue sin_val(const BasicValue& value);
BasicValue cos_val(const BasicValue& value);
BasicValue tan_val(const BasicValue& value);
BasicValue asin_val(const BasicValue& value);
BasicValue acos_val(const BasicValue& value);
BasicValue atan_val(const BasicValue& value);
BasicValue atan2_val(const BasicValue& y, const BasicValue& x);
BasicValue log_val(const BasicValue& value);
BasicValue log10_val(const BasicValue& value);
BasicValue exp_val(const BasicValue& value);
BasicValue pow_val(const BasicValue& base, const BasicValue& exp);
BasicValue floor_val(const BasicValue& value);
BasicValue ceil_val(const BasicValue& value);
BasicValue round_val(const BasicValue& value);
BasicValue int_val(const BasicValue& value);
BasicValue mod_val(const BasicValue& left, const BasicValue& right);
BasicValue rnd();
BasicValue pi_val();

// Array functions
BasicArray create_array(const std::vector<int>& dimensions);
BasicValue get_array_element(const BasicArray& array, const std::vector<int>& indices);
void set_array_element(BasicArray& array, const std::vector<int>& indices, const BasicValue& value);

// Typed array functions
BasicByteArray byte_array(const std::vector<int>& dimensions);
BasicIntArray int_array(const std::vector<int>& dimensions);
BasicDoubleArray double_array(const std::vector<int>& dimensions);

// Array initialization functions with parallelization
BasicIntArray int_array_fill(const std::vector<int>& dimensions, int value);
BasicDoubleArray double_array_fill(const std::vector<int>& dimensions, double value);
BasicIntArray int_array_range(int start, int end);

// Typed array element access
uint8_t get_byte_array_element(const BasicByteArray& array, const std::vector<int>& indices);
void set_byte_array_element(BasicByteArray& array, const std::vector<int>& indices, uint8_t value);
int get_int_array_element(const BasicIntArray& array, const std::vector<int>& indices);
void set_int_array_element(BasicIntArray& array, const std::vector<int>& indices, int value);
double get_double_array_element(const BasicDoubleArray& array, const std::vector<int>& indices);
void set_double_array_element(BasicDoubleArray& array, const std::vector<int>& indices, double value);

// Wrapper functions for code generator (with func_ prefix)
BasicValue func_byte_array(int size);
BasicValue func_int_array(int size);
BasicValue func_double_array(int size);

// File I/O functions
bool file_exists(const std::string& filename);
BasicValue file_size(const std::string& filename);
bool delete_file(const std::string& filename);
bool rename_file(const std::string& oldname, const std::string& newname);

// Text file I/O
BasicValue read_text_file(const std::string& filename);
bool write_text_file(const std::string& filename, const std::string& content);
bool append_text_file(const std::string& filename, const std::string& content);

// Binary file I/O with typed arrays
bool read_binary_file(const std::string& filename, BasicByteArray& buffer);
bool write_binary_file(const std::string& filename, const BasicByteArray& buffer);
BasicValue load_binary_file(const std::string& filename);  // Returns ByteArray

// CSV/structured data I/O
bool save_int_array_csv(const std::string& filename, const BasicIntArray& array);
bool save_double_array_csv(const std::string& filename, const BasicDoubleArray& array);
BasicValue load_int_array_csv(const std::string& filename);
BasicValue load_double_array_csv(const std::string& filename);

// Wrapper functions for code generator (file I/O)
BasicValue func_file_exists(const std::string& filename);
BasicValue func_file_size(const std::string& filename);
BasicValue func_delete_file(const std::string& filename);
BasicValue func_rename_file(const std::string& oldname, const std::string& newname);
BasicValue func_read_text_file(const std::string& filename);
BasicValue func_write_text_file(const BasicValue& filenameVal, const BasicValue& contentVal);
BasicValue func_append_text_file(const BasicValue& filenameVal, const BasicValue& contentVal);
BasicValue func_load_binary_file(const std::string& filename);
BasicValue func_write_binary_file(const BasicValue& filenameVal, const BasicValue& buffer);
BasicValue func_load_int_array_csv(const std::string& filename);
BasicValue func_load_double_array_csv(const std::string& filename);
BasicValue func_save_int_array_csv(const BasicValue& filenameVal, const BasicValue& array);
BasicValue func_save_double_array_csv(const BasicValue& filenameVal, const BasicValue& array);

// Buffer allocation wrapper functions for code generator
BasicValue func_alloc_int_buffer();
BasicValue func_alloc_pointer_buffer();
BasicValue func_alloc_buffer(const BasicValue& size);
BasicValue func_deref_int(const BasicValue& ptr);
BasicValue func_deref_pointer(const BasicValue& ptr);
BasicValue func_deref_string(const BasicValue& ptr);

// SDL struct wrapper functions for code generator
BasicValue func_create_sdl_rect(const BasicValue& x, const BasicValue& y, const BasicValue& w, const BasicValue& h);
BasicValue func_create_sdl_event();
BasicValue func_get_event_type(const BasicValue& event);
BasicValue func_get_key_code(const BasicValue& event);
BasicValue func_get_rect_field(const BasicValue& rect, const BasicValue& field);
BasicValue func_free_sdl_resource(const BasicValue& ptr);
BasicValue func_sdl_cleanup_all();

// Terminal wrapper functions for code generator (with func_ prefix)
BasicValue func_terminal_init();
BasicValue func_terminal_cleanup();
BasicValue func_terminal_supports_color();
BasicValue func_terminal_clear();
BasicValue func_terminal_set_cursor(const BasicValue& row, const BasicValue& col);
BasicValue func_terminal_get_cursor_row();
BasicValue func_terminal_get_cursor_col();
BasicValue func_terminal_set_color(const BasicValue& foreground, const BasicValue& background);
BasicValue func_terminal_reset_color();
BasicValue func_terminal_print(const BasicValue& text);
BasicValue func_terminal_print(const BasicValue& text, const BasicValue& foreground);
BasicValue func_terminal_print(const BasicValue& text, const BasicValue& foreground, const BasicValue& background);
BasicValue func_terminal_println();
BasicValue func_terminal_println(const BasicValue& text);
BasicValue func_terminal_println(const BasicValue& text, const BasicValue& foreground);
BasicValue func_terminal_println(const BasicValue& text, const BasicValue& foreground, const BasicValue& background);
BasicValue func_terminal_get_rows();
BasicValue func_terminal_get_cols();
BasicValue func_terminal_kbhit();
BasicValue func_terminal_getch();
BasicValue func_terminal_getline();
BasicValue func_terminal_getline(const BasicValue& prompt);
BasicValue func_terminal_getline(const BasicValue& prompt, const BasicValue& promptColor);
BasicValue func_terminal_show_cursor(const BasicValue& visible);
BasicValue func_terminal_set_echo(const BasicValue& enabled);

// Simple 1D array access helpers
BasicValue get_array_element(BasicValue& arrayVar, BasicValue index);
void set_array_element(BasicValue& arrayVar, BasicValue index, BasicValue value);

// Multidimensional array access helpers
BasicValue get_array_element(BasicValue& arrayVar, const std::vector<BasicValue>& indices);
void set_array_element(BasicValue& arrayVar, const std::vector<BasicValue>& indices, BasicValue value);

// Structure functions
BasicStruct create_struct(const std::string& typeName);
BasicValue get_struct_field(const BasicStruct& struct_, const std::string& fieldName);
void set_struct_field(BasicStruct& struct_, const std::string& fieldName, const BasicValue& value);

// Buffer allocation and output parameter functions
BasicValue alloc_int_buffer();              // Allocates int* for output parameters
BasicValue alloc_pointer_buffer();          // Allocates void** for output parameters  
BasicValue alloc_buffer(int size);          // Allocates byte buffer of specified size
BasicValue deref_int_offset(const BasicValue& ptr, const BasicValue& offset);
BasicValue deref_int(const BasicValue& ptr);     // Dereferences int* to get int value
BasicValue deref_pointer(const BasicValue& ptr); // Dereferences void** to get void* value
BasicValue deref_string(const BasicValue& ptr);  // Dereferences char* to get string value
void set_int_buffer(const BasicValue& ptr, int value);        // Sets value in int* buffer
void set_pointer_buffer(const BasicValue& ptr, const BasicValue& value); // Sets value in void** buffer

// SDL struct helpers
BasicValue create_sdl_rect(int x, int y, int w, int h);  // Creates SDL_Rect buffer
BasicValue create_sdl_event();                           // Creates SDL_Event buffer (56 bytes)
BasicValue get_event_type(const BasicValue& event);     // Gets event.type from SDL_Event
BasicValue get_key_code(const BasicValue& event);       // Gets key code from SDL_Event
BasicValue get_rect_field(const BasicValue& rect, const std::string& field); // Gets x,y,w,h from SDL_Rect

// SDL resource management
BasicValue free_sdl_resource(const BasicValue& ptr);    // Free a specific SDL resource
void sdl_cleanup_all();                                  // Clean up all allocated SDL resources

// GLM helper functions
BasicValue create_vec2(float x, float y);
BasicValue create_vec3(float x, float y, float z);
BasicValue create_vec4(float x, float y, float z, float w);
BasicValue create_mat3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
BasicValue create_mat4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33);
BasicValue create_quat(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);

// GLM component access
BasicValue get_vec_component(const BasicValue& vec, const std::string& component);
BasicValue set_vec_component(const BasicValue& vec, const std::string& component, float value);

// Type conversion
int to_int(const BasicValue& value);
double to_double(const BasicValue& value);
std::string to_string(const BasicValue& value);
bool to_bool(const BasicValue& value);

// Terminal functions
bool terminal_init();
void terminal_cleanup();
bool terminal_supports_color();
void terminal_clear();
void terminal_set_cursor(int row, int col);
BasicValue terminal_get_cursor_row();
BasicValue terminal_get_cursor_col();
void terminal_set_color(int foreground, int background);
void terminal_reset_color();
void terminal_print(const std::string& text, int foreground, int background);
void terminal_println(const std::string& text, int foreground, int background);
BasicValue terminal_get_rows();
BasicValue terminal_get_cols();
bool terminal_kbhit();
BasicValue terminal_getch();
BasicValue terminal_getline(const std::string& prompt, int promptColor);
void terminal_show_cursor(bool visible);
void terminal_set_echo(bool enabled);

// Arithmetic operations
BasicValue add(const BasicValue& left, const BasicValue& right);
BasicValue subtract(const BasicValue& left, const BasicValue& right);
BasicValue multiply(const BasicValue& left, const BasicValue& right);
BasicValue divide(const BasicValue& left, const BasicValue& right);

// Comparison operations
bool equal(const BasicValue& left, const BasicValue& right);
bool not_equal(const BasicValue& left, const BasicValue& right);
bool less_than(const BasicValue& left, const BasicValue& right);
bool less_equal(const BasicValue& left, const BasicValue& right);
bool greater_than(const BasicValue& left, const BasicValue& right);
bool greater_equal(const BasicValue& left, const BasicValue& right);

// Initialization
// Runtime initialization
void init_runtime();
void init_runtime_sdl(); // Initialise with SDL support

// Graphics functions (using IOHandler)
void graphics_mode(int width, int height);
void text_mode();
void clear_screen();
void set_colour(int r, int g, int b);
void draw_pixel(int x, int y);
void draw_line(int x1, int y1, int x2, int y2);
void draw_rect(int x, int y, int width, int height, bool filled = false);
void draw_text(int x, int y, const std::string& text);
void refresh_screen();

// Input functions
bool key_pressed(const std::string& key);
bool quit_requested();
void sleep_ms(int ms);
int get_ticks();

// Parallel array operations
void parallel_fill_array(BasicArray& array, const BasicValue& value);
void parallel_fill_int_array(BasicIntArray& array, int value);
void parallel_fill_double_array(BasicDoubleArray& array, double value);
void parallel_array_add(BasicDoubleArray& result, const BasicDoubleArray& a, const BasicDoubleArray& b);
void parallel_array_multiply_scalar(BasicDoubleArray& array, double scalar);

// Foreign Function Interface (FFI)
BasicValue load_library(const std::string& library_name);
BasicValue unload_library(const BasicValue& library_handle);
bool is_library_loaded(const BasicValue& library_handle);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6, const BasicValue& arg7);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6, const BasicValue& arg7, const BasicValue& arg8);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6, const BasicValue& arg7, const BasicValue& arg8, const BasicValue& arg9);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6, const BasicValue& arg7, const BasicValue& arg8, const BasicValue& arg9, const BasicValue& arg10);
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6, const BasicValue& arg7, const BasicValue& arg8, const BasicValue& arg9, const BasicValue& arg10, const BasicValue& arg11);

// Constant/NULL handling system
BasicValue get_constant(const std::string& name);       // Get predefined constants (NULL, SDL_*, SQLITE_*, etc.)
BasicValue is_null(const BasicValue& value);            // Check if value is NULL/nullptr
BasicValue not_null(const BasicValue& value);           // Check if value is NOT NULL

// Constant handling wrapper functions for code generator
BasicValue func_get_constant(const BasicValue& name);
BasicValue func_is_null(const BasicValue& value);
BasicValue func_not_null(const BasicValue& value);

} // namespace basic_runtime