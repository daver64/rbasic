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

// Forward declarations
struct BasicStruct;
struct BasicArray;

// Value type for compiled BASIC programs
using BasicValue = std::variant<int, double, std::string, bool, BasicStruct, BasicArray>;

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
        
        for (int i = dimensions.size() - 1; i >= 0; i--) {
            index += (indices[i] - 1) * multiplier;  // BASIC arrays are 1-indexed
            multiplier *= dimensions[i];
        }
        
        return elements[index];
    }
    
    const BasicValue& at(const std::vector<int>& indices) const {
        int index = 0;
        int multiplier = 1;
        
        for (int i = dimensions.size() - 1; i >= 0; i--) {
            index += (indices[i] - 1) * multiplier;  // BASIC arrays are 1-indexed
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
BasicValue input();

// String functions  
int len(const BasicValue& str);
BasicValue mid(const BasicValue& str, int start, int length = -1);
BasicValue left(const BasicValue& str, int length);
BasicValue right(const BasicValue& str, int length);

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
void init_runtime_sdl(); // Initialize with SDL support

// Graphics functions (using IOHandler)
void graphics_mode(int width, int height);
void text_mode();
void clear_screen();
void set_color(int r, int g, int b);
void draw_pixel(int x, int y);
void draw_line(int x1, int y1, int x2, int y2);
void draw_rect(int x, int y, int width, int height, bool filled = false);
void refresh_screen();

// Input functions
bool key_pressed(const std::string& key);
bool quit_requested();
void sleep_ms(int ms);
int get_ticks();

} // namespace basic_runtime