#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <cmath>
#include <cstdlib>
#include <ctime>

#ifdef RBASIC_SDL_SUPPORT
#include <SDL.h>
#endif

#ifdef RBASIC_SQLITE_SUPPORT
#include <sqlite3.h>
#endif

// Forward declarations
struct BasicStruct;
struct BasicArray;
struct BasicByteArray;
struct BasicIntArray;
struct BasicDoubleArray;

// Value type for compiled BASIC programs
using BasicValue = std::variant<int, double, std::string, bool, BasicStruct, BasicArray, BasicByteArray, BasicIntArray, BasicDoubleArray>;

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

// Simple 1D array access helpers
BasicValue get_array_element(BasicValue& arrayVar, BasicValue index);
void set_array_element(BasicValue& arrayVar, BasicValue index, BasicValue value);

// Structure functions
BasicStruct create_struct(const std::string& typeName);
BasicValue get_struct_field(const BasicStruct& struct_, const std::string& fieldName);
void set_struct_field(BasicStruct& struct_, const std::string& fieldName, const BasicValue& value);

// Type conversion
int to_int(const BasicValue& value);
double to_double(const BasicValue& value);
std::string to_string(const BasicValue& value);
bool to_bool(const BasicValue& value);

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

// Database functions (SQLite)
#ifdef RBASIC_SQLITE_SUPPORT
BasicValue db_open(const std::string& database_path);
BasicValue db_close();
BasicValue db_exec(const std::string& sql);
BasicValue db_query(const std::string& sql);
BasicValue db_error();
BasicValue db_escape(const std::string& str);
#endif

} // namespace basic_runtime