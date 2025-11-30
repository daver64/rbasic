#include "basic_runtime.h"
#include "../include/io_handler.h"
#include "../include/common.h"
#include "../include/terminal.h"

// Raspberry Pi hardware support (conditional)
#ifdef RPI_SUPPORT_ENABLED
#include "../include/rpi_gpio.h"
#include "../include/rpi_spi.h"
#include "../include/rpi_i2c.h"
#include "../include/rpi_pwm.h"
#include "../include/rpi_serial.h"
// Expose C functions to runtime
using namespace rbasic::rpi;
#endif

#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <thread>

#ifdef _WIN32
// Undefine Windows macros that conflict with std::min/std::max
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
// Windows calling convention
#define FFI_CALL_CONV __cdecl
#else
// Unix/Linux calling convention (default)
#define FFI_CALL_CONV
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef SDL2_SUPPORT_ENABLED
#include "sdl2_wrapper.h"
#endif

#ifdef SQLITE3_SUPPORT_ENABLED
#include "sqlite3_wrapper.h"
#endif

namespace basic_runtime {

// Array parallelization helpers
void parallel_fill_array(BasicArray& array, const BasicValue& value) {
    int size = static_cast<int>(array.elements.size());
#ifdef _OPENMP
    if (size > 1000) { // Only parallelize large arrays
        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            array.elements[i] = value;
        }
    } else {
#endif
        // Use serial loop for small arrays or when OpenMP not available
        for (int i = 0; i < size; ++i) {
            array.elements[i] = value;
        }
#ifdef _OPENMP
    }
#endif
}

void parallel_fill_int_array(BasicIntArray& array, int value) {
    int size = static_cast<int>(array.elements.size());
#ifdef _OPENMP
    if (size > 1000) { // Only parallelize large arrays
        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            array.elements[i] = value;
        }
    } else {
#endif
        // Use serial loop for small arrays or when OpenMP not available
        for (int i = 0; i < size; ++i) {
            array.elements[i] = value;
        }
#ifdef _OPENMP
    }
#endif
}

void parallel_fill_double_array(BasicDoubleArray& array, double value) {
    int size = static_cast<int>(array.elements.size());
#ifdef _OPENMP
    if (size > 1000) { // Only parallelize large arrays
        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            array.elements[i] = value;
        }
    } else {
#endif
        // Use serial loop for small arrays or when OpenMP not available
        for (int i = 0; i < size; ++i) {
            array.elements[i] = value;
        }
#ifdef _OPENMP
    }
#endif
}

void parallel_array_add(BasicDoubleArray& result, const BasicDoubleArray& a, const BasicDoubleArray& b) {
    auto min_size = std::min(a.elements.size(), b.elements.size());
    int size = static_cast<int>(min_size);
    result.elements.resize(size);
    
#ifdef _OPENMP
    if (size > 1000) { // Only parallelize large arrays
        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            result.elements[i] = a.elements[i] + b.elements[i];
        }
    } else {
#endif
        for (int i = 0; i < size; ++i) {
            result.elements[i] = a.elements[i] + b.elements[i];
        }
#ifdef _OPENMP
    }
#endif
}

void parallel_array_multiply_scalar(BasicDoubleArray& array, double scalar) {
    int size = static_cast<int>(array.elements.size());
#ifdef _OPENMP
    if (size > 1000) { // Only parallelize large arrays
        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            array.elements[i] = array.elements[i] * scalar;
        }
    } else {
#endif
        for (int i = 0; i < size; ++i) {
            array.elements[i] = array.elements[i] * scalar;
        }
#ifdef _OPENMP
    }
#endif
}

// Global IOHandler for compiled programs
static rbasic::IOHandler* g_io_handler = nullptr;

void init_io_handler(rbasic::IOHandler* handler) {
    g_io_handler = handler;
}

rbasic::IOHandler* get_io_handler() {
    return g_io_handler;
}

void init_runtime() {
    // Sync C++ iostream with C stdio for better compatibility (especially with MinGW64 + PowerShell)
    std::ios::sync_with_stdio(true);
    std::cin.tie(&std::cout);
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    // Initialize terminal support for compiled programs
    rbasic::Terminal::initialize();
    
    // Register cleanup function to be called on exit
    std::atexit([]() {
        rbasic::Terminal::cleanup();
    });
}

void init_runtime_sdl() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    

}

void print(const BasicValue& value) {
    if (g_io_handler) {
        g_io_handler->print(to_string(value));
    } else {
        std::cout << to_string(value);
        std::cout.flush(); // Ensure output is flushed immediately
    }
}

void print_line() {
    if (g_io_handler) {
        g_io_handler->newline();
    } else {
        std::cout << std::endl;
        std::cout.flush(); // Ensure output is flushed immediately
    }
}

// Debug function that always prints to console, bypassing SDL
void debug_print(const BasicValue& value) {
    std::cout << to_string(value) << std::endl;
    std::cout.flush();
}

BasicValue input() {
    std::string line;
    if (g_io_handler) {
        line = g_io_handler->input();
    } else {
        std::getline(std::cin, line);
    }
    
    // Try to parse as number
    try {
        if (rbasic::hasDecimalPoint(line)) {
            return std::stod(line);
        } else {
            return std::stoi(line);
        }
    } catch (const std::exception&) {
        return line; // Return as string if not a number
    }
}

int len(const BasicValue& str) {
    return static_cast<int>(to_string(str).length());
}

BasicValue mid(const BasicValue& str, int start, int length) {
    std::string s = to_string(str);
    int adjusted_start = std::max(0, start - 1); // BASIC is 1-indexed
    start = adjusted_start;
    
    if (length == -1) {
        return s.substr(start);
    } else {
        return s.substr(start, length);
    }
}

BasicValue left(const BasicValue& str, int length) {
    std::string s = to_string(str);
    return s.substr(0, length);
}

BasicValue right(const BasicValue& str, int length) {
    std::string s = to_string(str);
    int string_len = static_cast<int>(s.length());
    int start = std::max(0, string_len - length);
    return s.substr(start);
}

BasicValue val(const BasicValue& str) {
    std::string s = to_string(str);
    try {
        if (rbasic::hasDecimalPoint(s)) {
            return std::stod(s);
        } else {
            return std::stoi(s);
        }
    } catch (const std::exception&) {
        return 0; // Default to 0 if conversion fails
    }
}

BasicValue abs_val(const BasicValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::abs(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::abs(std::get<double>(value));
    }
    return 0;
}

BasicValue sqrt_val(const BasicValue& value) {
    return std::sqrt(to_double(value));
}

BasicValue sqr_val(const BasicValue& value) {
    return std::sqrt(to_double(value));
}

BasicValue sin_val(const BasicValue& value) {
    return std::sin(to_double(value));
}

BasicValue cos_val(const BasicValue& value) {
    return std::cos(to_double(value));
}

BasicValue tan_val(const BasicValue& value) {
    return std::tan(to_double(value));
}

BasicValue asin_val(const BasicValue& value) {
    return std::asin(to_double(value));
}

BasicValue acos_val(const BasicValue& value) {
    return std::acos(to_double(value));
}

BasicValue atan_val(const BasicValue& value) {
    return std::atan(to_double(value));
}

BasicValue atan2_val(const BasicValue& y, const BasicValue& x) {
    return std::atan2(to_double(y), to_double(x));
}

BasicValue log_val(const BasicValue& value) {
    double val = to_double(value);
    if (val <= 0) {
        throw std::runtime_error("LOG requires a positive argument");
    }
    return std::log(val);
}

BasicValue ln_val(const BasicValue& value) {
    double val = to_double(value);
    if (val <= 0) {
        throw std::runtime_error("LN requires a positive argument");
    }
    return std::log(val);  // ln is natural logarithm (same as log)
}

BasicValue log10_val(const BasicValue& value) {
    double val = to_double(value);
    if (val <= 0) {
        throw std::runtime_error("LOG10 requires a positive argument");
    }
    return std::log10(val);
}

BasicValue exp_val(const BasicValue& value) {
    return std::exp(to_double(value));
}

BasicValue pow_val(const BasicValue& base, const BasicValue& exp) {
    return std::pow(to_double(base), to_double(exp));
}

BasicValue floor_val(const BasicValue& value) {
    return std::floor(to_double(value));
}

BasicValue ceil_val(const BasicValue& value) {
    return std::ceil(to_double(value));
}

BasicValue round_val(const BasicValue& value) {
    return std::round(to_double(value));
}

BasicValue pi_val() {
    return 3.141592653589793;
}

BasicValue mod_val(const BasicValue& left, const BasicValue& right) {
    int leftInt = to_int(left);
    int rightInt = to_int(right);
    if (rightInt == 0) {
        throw std::runtime_error("Modulo by zero");
    }
    return leftInt % rightInt;
}

BasicValue int_val(const BasicValue& value) {
    return to_int(value);
}

BasicValue rnd() {
    return static_cast<double>(std::rand()) / RAND_MAX;
}

BasicArray create_array(const std::vector<int>& dimensions) {
    return BasicArray(dimensions);
}

BasicValue get_array_element(const BasicArray& array, const std::vector<int>& indices) {
    return array.at(indices);
}

void set_array_element(BasicArray& array, const std::vector<int>& indices, const BasicValue& value) {
    array.at(indices) = value;
}

// Typed array functions
BasicByteArray byte_array(const std::vector<int>& dimensions) {
    return BasicByteArray(dimensions);
}

BasicIntArray int_array(const std::vector<int>& dimensions) {
    return BasicIntArray(dimensions);
}

BasicDoubleArray double_array(const std::vector<int>& dimensions) {
    return BasicDoubleArray(dimensions);
}

// Array initialization functions with parallelization
BasicIntArray int_array_fill(const std::vector<int>& dimensions, int value) {
    BasicIntArray arr(dimensions);
    parallel_fill_int_array(arr, value);
    return arr;
}

BasicDoubleArray double_array_fill(const std::vector<int>& dimensions, double value) {
    BasicDoubleArray arr(dimensions);
    parallel_fill_double_array(arr, value);
    return arr;
}

BasicIntArray int_array_range(int start, int end) {
    std::vector<int> dims = {end - start};
    BasicIntArray arr(dims);
    
    int size = static_cast<int>(arr.elements.size());
#ifdef _OPENMP
    if (size > 1000) { // Only parallelize large arrays
        #pragma omp parallel for
        for (int i = 0; i < size; ++i) {
            arr.elements[i] = start + i;
        }
    } else {
#endif
        for (int i = 0; i < size; ++i) {
            arr.elements[i] = start + i;
        }
#ifdef _OPENMP
    }
#endif
    
    return arr;
}

// Typed array element access
uint8_t get_byte_array_element(const BasicByteArray& array, const std::vector<int>& indices) {
    return array.at(indices);
}

void set_byte_array_element(BasicByteArray& array, const std::vector<int>& indices, uint8_t value) {
    array.at(indices) = value;
}

int get_int_array_element(const BasicIntArray& array, const std::vector<int>& indices) {
    return array.at(indices);
}

void set_int_array_element(BasicIntArray& array, const std::vector<int>& indices, int value) {
    array.at(indices) = value;
}

double get_double_array_element(const BasicDoubleArray& array, const std::vector<int>& indices) {
    return array.at(indices);
}

void set_double_array_element(BasicDoubleArray& array, const std::vector<int>& indices, double value) {
    array.at(indices) = value;
}

// Wrapper functions for code generator (with func_ prefix)
BasicValue func_byte_array(int size) {
    return BasicByteArray({size});
}

BasicValue func_int_array(int size) {
    return BasicIntArray({size});
}

BasicValue func_double_array(int size) {
    return BasicDoubleArray({size});
}

BasicStruct create_struct(const std::string& typeName) {
    return BasicStruct(typeName);
}

BasicValue get_struct_field(const BasicStruct& struct_, const std::string& fieldName) {
    auto it = struct_.fields.find(fieldName);
    if (it != struct_.fields.end()) {
        return it->second;
    }
    return 0; // Default value if field not found
}

// Overload that accepts BasicValue and extracts the struct
BasicValue get_struct_field(const BasicValue& value, const std::string& fieldName) {
    if (std::holds_alternative<BasicStruct>(value)) {
        return get_struct_field(std::get<BasicStruct>(value), fieldName);
    }
    return 0; // Return 0 if not a struct
}

void set_struct_field(BasicStruct& struct_, const std::string& fieldName, const BasicValue& value) {
    struct_.fields[fieldName] = value;
}

int to_int(const BasicValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return static_cast<int>(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1 : 0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stoi(std::get<std::string>(value));
        } catch (const std::exception&) {
            return 0;
        }
    }
    return 0;
}

double to_double(const BasicValue& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value));
        } catch (const std::exception&) {
            return 0.0;
        }
    }
    return 0.0;
}

std::string to_string(const BasicValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<BasicStruct>(value)) {
        return "[struct " + std::get<BasicStruct>(value).typeName + "]";
    } else if (std::holds_alternative<BasicArray>(value)) {
        return "[array]";
    }
    return "";
}

bool to_bool(const BasicValue& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) != 0;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        return !std::get<std::string>(value).empty();
    }
    return false;
}

BasicValue add(const BasicValue& left, const BasicValue& right) {
    // GLM vector addition
    if (std::holds_alternative<BasicVec2>(left) && std::holds_alternative<BasicVec2>(right)) {
        BasicVec2 leftVec = std::get<BasicVec2>(left);
        BasicVec2 rightVec = std::get<BasicVec2>(right);
        return BasicValue(BasicVec2(leftVec.data + rightVec.data));
    }
    if (std::holds_alternative<BasicVec3>(left) && std::holds_alternative<BasicVec3>(right)) {
        BasicVec3 leftVec = std::get<BasicVec3>(left);
        BasicVec3 rightVec = std::get<BasicVec3>(right);
        return BasicValue(BasicVec3(leftVec.data + rightVec.data));
    }
    if (std::holds_alternative<BasicVec4>(left) && std::holds_alternative<BasicVec4>(right)) {
        BasicVec4 leftVec = std::get<BasicVec4>(left);
        BasicVec4 rightVec = std::get<BasicVec4>(right);
        return BasicValue(BasicVec4(leftVec.data + rightVec.data));
    }
    
    // Original string and numeric addition
    if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
        return to_string(left) + to_string(right);
    } else if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) + to_double(right);
    } else {
        return to_int(left) + to_int(right);
    }
}

BasicValue subtract(const BasicValue& left, const BasicValue& right) {
    // GLM vector subtraction
    if (std::holds_alternative<BasicVec2>(left) && std::holds_alternative<BasicVec2>(right)) {
        BasicVec2 leftVec = std::get<BasicVec2>(left);
        BasicVec2 rightVec = std::get<BasicVec2>(right);
        return BasicValue(BasicVec2(leftVec.data - rightVec.data));
    }
    if (std::holds_alternative<BasicVec3>(left) && std::holds_alternative<BasicVec3>(right)) {
        BasicVec3 leftVec = std::get<BasicVec3>(left);
        BasicVec3 rightVec = std::get<BasicVec3>(right);
        return BasicValue(BasicVec3(leftVec.data - rightVec.data));
    }
    if (std::holds_alternative<BasicVec4>(left) && std::holds_alternative<BasicVec4>(right)) {
        BasicVec4 leftVec = std::get<BasicVec4>(left);
        BasicVec4 rightVec = std::get<BasicVec4>(right);
        return BasicValue(BasicVec4(leftVec.data - rightVec.data));
    }
    
    // Original numeric subtraction
    if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) - to_double(right);
    } else {
        return to_int(left) - to_int(right);
    }
}

BasicValue multiply(const BasicValue& left, const BasicValue& right) {
    // GLM vector-scalar multiplication
    if (std::holds_alternative<BasicVec2>(left) && (std::holds_alternative<double>(right) || std::holds_alternative<int>(right))) {
        BasicVec2 vec = std::get<BasicVec2>(left);
        float scalar = static_cast<float>(to_double(right));
        return BasicValue(BasicVec2(vec.data * scalar));
    }
    if ((std::holds_alternative<double>(left) || std::holds_alternative<int>(left)) && std::holds_alternative<BasicVec2>(right)) {
        float scalar = static_cast<float>(to_double(left));
        BasicVec2 vec = std::get<BasicVec2>(right);
        return BasicValue(BasicVec2(scalar * vec.data));
    }
    if (std::holds_alternative<BasicVec3>(left) && (std::holds_alternative<double>(right) || std::holds_alternative<int>(right))) {
        BasicVec3 vec = std::get<BasicVec3>(left);
        float scalar = static_cast<float>(to_double(right));
        return BasicValue(BasicVec3(vec.data * scalar));
    }
    if ((std::holds_alternative<double>(left) || std::holds_alternative<int>(left)) && std::holds_alternative<BasicVec3>(right)) {
        float scalar = static_cast<float>(to_double(left));
        BasicVec3 vec = std::get<BasicVec3>(right);
        return BasicValue(BasicVec3(scalar * vec.data));
    }
    if (std::holds_alternative<BasicVec4>(left) && (std::holds_alternative<double>(right) || std::holds_alternative<int>(right))) {
        BasicVec4 vec = std::get<BasicVec4>(left);
        float scalar = static_cast<float>(to_double(right));
        return BasicValue(BasicVec4(vec.data * scalar));
    }
    if ((std::holds_alternative<double>(left) || std::holds_alternative<int>(left)) && std::holds_alternative<BasicVec4>(right)) {
        float scalar = static_cast<float>(to_double(left));
        BasicVec4 vec = std::get<BasicVec4>(right);
        return BasicValue(BasicVec4(scalar * vec.data));
    }
    
    // GLM vector-vector multiplication (component-wise)
    if (std::holds_alternative<BasicVec2>(left) && std::holds_alternative<BasicVec2>(right)) {
        BasicVec2 leftVec = std::get<BasicVec2>(left);
        BasicVec2 rightVec = std::get<BasicVec2>(right);
        return BasicValue(BasicVec2(leftVec.data * rightVec.data));
    }
    if (std::holds_alternative<BasicVec3>(left) && std::holds_alternative<BasicVec3>(right)) {
        BasicVec3 leftVec = std::get<BasicVec3>(left);
        BasicVec3 rightVec = std::get<BasicVec3>(right);
        return BasicValue(BasicVec3(leftVec.data * rightVec.data));
    }
    if (std::holds_alternative<BasicVec4>(left) && std::holds_alternative<BasicVec4>(right)) {
        BasicVec4 leftVec = std::get<BasicVec4>(left);
        BasicVec4 rightVec = std::get<BasicVec4>(right);
        return BasicValue(BasicVec4(leftVec.data * rightVec.data));
    }
    
    // Original numeric multiplication
    if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) * to_double(right);
    } else {
        return to_int(left) * to_int(right);
    }
}

BasicValue divide(const BasicValue& left, const BasicValue& right) {
    double rightVal = to_double(right);
    if (rightVal == 0.0) {
        throw std::runtime_error("Division by zero");
    }
    return to_double(left) / rightVal;
}

bool equal(const BasicValue& left, const BasicValue& right) {
    // Handle same types
    if (left.index() == right.index()) {
        if (std::holds_alternative<int>(left)) {
            return std::get<int>(left) == std::get<int>(right);
        } else if (std::holds_alternative<double>(left)) {
            return std::get<double>(left) == std::get<double>(right);
        } else if (std::holds_alternative<std::string>(left)) {
            return std::get<std::string>(left) == std::get<std::string>(right);
        } else if (std::holds_alternative<bool>(left)) {
            return std::get<bool>(left) == std::get<bool>(right);
        } else {
            // For struct and array types, compare as strings for now
            return to_string(left) == to_string(right);
        }
    }
    
    // Different types, try to compare as numbers or strings
    if ((std::holds_alternative<int>(left) || std::holds_alternative<double>(left)) &&
        (std::holds_alternative<int>(right) || std::holds_alternative<double>(right))) {
        return to_double(left) == to_double(right);
    }
    return to_string(left) == to_string(right);
}

bool not_equal(const BasicValue& left, const BasicValue& right) {
    return !equal(left, right);
}

bool less_than(const BasicValue& left, const BasicValue& right) {
    if ((std::holds_alternative<int>(left) || std::holds_alternative<double>(left)) &&
        (std::holds_alternative<int>(right) || std::holds_alternative<double>(right))) {
        return to_double(left) < to_double(right);
    }
    return to_string(left) < to_string(right);
}

bool less_equal(const BasicValue& left, const BasicValue& right) {
    return less_than(left, right) || equal(left, right);
}

bool greater_than(const BasicValue& left, const BasicValue& right) {
    return !less_equal(left, right);
}

bool greater_equal(const BasicValue& left, const BasicValue& right) {
    return !less_than(left, right);
}

// Graphics functions
void graphics_mode(int width, int height) {
    if (g_io_handler) {
        g_io_handler->graphics_mode(width, height);
    } 
}

void text_mode() {
    if (g_io_handler) {
        g_io_handler->text_mode();
    }
}

void clear_screen() {
    if (g_io_handler) {
        g_io_handler->clear_screen();
    } 
}

void set_colour(int r, int g, int b) {
    if (g_io_handler) {
        g_io_handler->set_colour(r, g, b);
    }
}

void draw_pixel(int x, int y) {
    if (g_io_handler) {
        g_io_handler->draw_pixel(x, y);
    } 
}

void draw_line(int x1, int y1, int x2, int y2) {
    if (g_io_handler) {
        g_io_handler->draw_line(x1, y1, x2, y2);
    } 
}

void draw_rect(int x, int y, int width, int height, bool filled) {
    if (g_io_handler) {
        g_io_handler->draw_rect(x, y, width, height, filled);
    } 
}

void draw_text(int x, int y, const std::string& text) {
    if (g_io_handler) {
        g_io_handler->draw_text(x, y, text);
    } 
}

void refresh_screen() {
    if (g_io_handler) {
        g_io_handler->refresh_screen();
    } 
}

bool key_pressed(const std::string& key) {
    if (g_io_handler) {
        return g_io_handler->key_pressed(key);
    } 
    return false;
}

bool quit_requested() {
    if (g_io_handler) {
        return g_io_handler->quit_requested();
    }
    return false;
}

void sleep_ms(int ms) {
    if (g_io_handler) {
        g_io_handler->sleep_ms(ms);
    } 
}

int get_ticks() {
    if (g_io_handler) {
        return g_io_handler->get_ticks();
    }
    return 0;
}

// Simple 1D array access helpers
BasicValue get_array_element(BasicValue& arrayVar, BasicValue index) {
    if (std::holds_alternative<BasicArray>(arrayVar)) {
        BasicArray& array = std::get<BasicArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            return array.elements[idx]; // Use 0-based indexing directly
        }
    } else if (std::holds_alternative<BasicByteArray>(arrayVar)) {
        BasicByteArray& array = std::get<BasicByteArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            return BasicValue(static_cast<int>(array.elements[idx]));
        }
    } else if (std::holds_alternative<BasicIntArray>(arrayVar)) {
        BasicIntArray& array = std::get<BasicIntArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            return BasicValue(array.elements[idx]);
        }
    } else if (std::holds_alternative<BasicDoubleArray>(arrayVar)) {
        BasicDoubleArray& array = std::get<BasicDoubleArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            return BasicValue(array.elements[idx]);
        }
    }
    return BasicValue(0); // Default value
}

void set_array_element(BasicValue& arrayVar, BasicValue index, BasicValue value) {
    if (std::holds_alternative<BasicArray>(arrayVar)) {
        BasicArray& array = std::get<BasicArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            array.elements[idx] = value; // Use 0-based indexing directly
        }
    } else if (std::holds_alternative<BasicByteArray>(arrayVar)) {
        BasicByteArray& array = std::get<BasicByteArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            array.elements[idx] = static_cast<uint8_t>(to_int(value));
        }
    } else if (std::holds_alternative<BasicIntArray>(arrayVar)) {
        BasicIntArray& array = std::get<BasicIntArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            array.elements[idx] = to_int(value);
        }
    } else if (std::holds_alternative<BasicDoubleArray>(arrayVar)) {
        BasicDoubleArray& array = std::get<BasicDoubleArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 0 && idx < static_cast<int>(array.elements.size())) {
            array.elements[idx] = to_double(value);
        }
    }
}

// Multidimensional array access helpers
BasicValue get_array_element(BasicValue& arrayVar, const std::vector<BasicValue>& indices) {
    // Convert BasicValue indices to int indices
    std::vector<int> intIndices;
    for (const auto& index : indices) {
        intIndices.push_back(to_int(index));
    }
    
    if (std::holds_alternative<BasicArray>(arrayVar)) {
        BasicArray& array = std::get<BasicArray>(arrayVar);
        return get_array_element(array, intIndices);
    } else if (std::holds_alternative<BasicByteArray>(arrayVar)) {
        BasicByteArray& array = std::get<BasicByteArray>(arrayVar);
        return static_cast<int>(get_byte_array_element(array, intIndices));
    } else if (std::holds_alternative<BasicIntArray>(arrayVar)) {
        BasicIntArray& array = std::get<BasicIntArray>(arrayVar);
        return get_int_array_element(array, intIndices);
    } else if (std::holds_alternative<BasicDoubleArray>(arrayVar)) {
        BasicDoubleArray& array = std::get<BasicDoubleArray>(arrayVar);
        return get_double_array_element(array, intIndices);
    }
    
    return 0; // Default value if not an array
}

void set_array_element(BasicValue& arrayVar, const std::vector<BasicValue>& indices, BasicValue value) {
    // Convert BasicValue indices to int indices
    std::vector<int> intIndices;
    for (const auto& index : indices) {
        intIndices.push_back(to_int(index));
    }
    
    if (std::holds_alternative<BasicArray>(arrayVar)) {
        BasicArray& array = std::get<BasicArray>(arrayVar);
        set_array_element(array, intIndices, value);
    } else if (std::holds_alternative<BasicByteArray>(arrayVar)) {
        BasicByteArray& array = std::get<BasicByteArray>(arrayVar);
        set_byte_array_element(array, intIndices, static_cast<uint8_t>(to_int(value)));
    } else if (std::holds_alternative<BasicIntArray>(arrayVar)) {
        BasicIntArray& array = std::get<BasicIntArray>(arrayVar);
        set_int_array_element(array, intIndices, to_int(value));
    } else if (std::holds_alternative<BasicDoubleArray>(arrayVar)) {
        BasicDoubleArray& array = std::get<BasicDoubleArray>(arrayVar);
        set_double_array_element(array, intIndices, to_double(value));
    }
}


// File I/O functions
bool file_exists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

BasicValue file_size(const std::string& filename) {
    try {
        return static_cast<int>(std::filesystem::file_size(filename));
    } catch (...) {
        return -1; // Error
    }
}

bool delete_file(const std::string& filename) {
    try {
        return std::filesystem::remove(filename);
    } catch (...) {
        return false;
    }
}

bool rename_file(const std::string& oldname, const std::string& newname) {
    try {
        std::filesystem::rename(oldname, newname);
        return true;
    } catch (...) {
        return false;
    }
}

// Text file I/O
BasicValue read_text_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return std::string(""); // Return empty string on error
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    return content;
}

bool write_text_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return !file.fail();
}

bool append_text_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return !file.fail();
}

// Binary file I/O with typed arrays
bool read_binary_file(const std::string& filename, BasicByteArray& buffer) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Resize buffer if needed
    if (buffer.elements.size() < fileSize) {
        buffer.elements.resize(fileSize);
        buffer.dimensions = {static_cast<int>(fileSize)};
    }
    
    // Read data
    file.read(reinterpret_cast<char*>(buffer.elements.data()), fileSize);
    file.close();
    
    return !file.fail();
}

bool write_binary_file(const std::string& filename, const BasicByteArray& buffer) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(buffer.elements.data()), buffer.elements.size());
    file.close();
    
    return !file.fail();
}

BasicValue load_binary_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return BasicByteArray(); // Return empty array on error
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Create buffer
    BasicByteArray buffer({static_cast<int>(fileSize)});
    
    // Read data
    file.read(reinterpret_cast<char*>(buffer.elements.data()), fileSize);
    file.close();
    
    return buffer;
}

// CSV/structured data I/O
bool save_int_array_csv(const std::string& filename, const BasicIntArray& array) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (size_t i = 0; i < array.elements.size(); ++i) {
        if (i > 0) file << ",";
        file << array.elements[i];
    }
    file << "\n";
    file.close();
    
    return !file.fail();
}

bool save_double_array_csv(const std::string& filename, const BasicDoubleArray& array) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (size_t i = 0; i < array.elements.size(); ++i) {
        if (i > 0) file << ",";
        file << array.elements[i];
    }
    file << "\n";
    file.close();
    
    return !file.fail();
}

BasicValue load_int_array_csv(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return BasicIntArray(); // Return empty array on error
    }
    
    std::string line;
    std::getline(file, line);
    file.close();
    
    // Parse CSV line
    std::vector<int> values;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        try {
            values.push_back(std::stoi(token));
        } catch (...) {
            values.push_back(0); // Default value on parse error
        }
    }
    
    // Create array
    BasicIntArray result({static_cast<int>(values.size())});
    std::copy(values.begin(), values.end(), result.elements.begin());
    
    return result;
}

BasicValue load_double_array_csv(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return BasicDoubleArray(); // Return empty array on error
    }
    
    std::string line;
    std::getline(file, line);
    file.close();
    
    // Parse CSV line
    std::vector<double> values;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        try {
            values.push_back(std::stod(token));
        } catch (...) {
            values.push_back(0.0); // Default value on parse error
        }
    }
    
    // Create array
    BasicDoubleArray result({static_cast<int>(values.size())});
    std::copy(values.begin(), values.end(), result.elements.begin());
    
    return result;
}

// Wrapper functions for code generator (file I/O)
BasicValue func_file_exists(const std::string& filename) {
    return file_exists(filename);
}

BasicValue func_file_size(const std::string& filename) {
    return file_size(filename);
}

BasicValue func_delete_file(const std::string& filename) {
    return delete_file(filename);
}

BasicValue func_rename_file(const std::string& oldname, const std::string& newname) {
    return rename_file(oldname, newname);
}

BasicValue func_read_text_file(const std::string& filename) {
    return read_text_file(filename);
}

BasicValue func_write_text_file(const BasicValue& filenameVal, const BasicValue& contentVal) {
    if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<std::string>(contentVal)) {
        return write_text_file(std::get<std::string>(filenameVal), std::get<std::string>(contentVal));
    }
    return false;
}

BasicValue func_append_text_file(const BasicValue& filenameVal, const BasicValue& contentVal) {
    if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<std::string>(contentVal)) {
        return append_text_file(std::get<std::string>(filenameVal), std::get<std::string>(contentVal));
    }
    return false;
}

BasicValue func_load_binary_file(const std::string& filename) {
    return load_binary_file(filename);
}

BasicValue func_write_binary_file(const BasicValue& filenameVal, const BasicValue& buffer) {
    if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<BasicByteArray>(buffer)) {
        return write_binary_file(std::get<std::string>(filenameVal), std::get<BasicByteArray>(buffer));
    }
    return false;
}

BasicValue func_load_int_array_csv(const std::string& filename) {
    return load_int_array_csv(filename);
}

BasicValue func_load_double_array_csv(const std::string& filename) {
    return load_double_array_csv(filename);
}

BasicValue func_sleep(const BasicValue& milliseconds) {
    int ms = std::holds_alternative<int>(milliseconds) ? std::get<int>(milliseconds) :
             static_cast<int>(std::get<double>(milliseconds));
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    return 0;
}

BasicValue func_save_int_array_csv(const BasicValue& filenameVal, const BasicValue& array) {
    if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<BasicIntArray>(array)) {
        return save_int_array_csv(std::get<std::string>(filenameVal), std::get<BasicIntArray>(array));
    }
    return false;
}

BasicValue func_save_double_array_csv(const BasicValue& filenameVal, const BasicValue& array) {
    if (std::holds_alternative<std::string>(filenameVal) && std::holds_alternative<BasicDoubleArray>(array)) {
        return save_double_array_csv(std::get<std::string>(filenameVal), std::get<BasicDoubleArray>(array));
    }
    return false;
}

// Terminal functions
bool terminal_init() {
    return rbasic::Terminal::initialize();
}

void terminal_cleanup() {
    rbasic::Terminal::cleanup();
}

bool terminal_supports_colour() {
    return rbasic::Terminal::supportsColour();
}

void terminal_clear() {
    rbasic::Terminal::clear();
}

void terminal_set_cursor(int row, int col) {
    rbasic::Terminal::setCursor(row, col);
}

BasicValue terminal_get_cursor_row() {
    int row, col;
    rbasic::Terminal::getCursor(row, col);
    return row;
}

BasicValue terminal_get_cursor_col() {
    int row, col;
    rbasic::Terminal::getCursor(row, col);
    return col;
}

void terminal_save_cursor() {
    rbasic::Terminal::saveCursor();
}

void terminal_restore_cursor() {
    rbasic::Terminal::restoreCursor();
}

void terminal_set_colour(int foreground, int background) {
    rbasic::Terminal::setColour(static_cast<rbasic::Colour>(foreground), 
                               static_cast<rbasic::Colour>(background));
}

void terminal_reset_colour() {
    rbasic::Terminal::resetColour();
}

void terminal_print(const std::string& text, int foreground, int background) {
    rbasic::Terminal::print(text, static_cast<rbasic::Colour>(foreground), 
                            static_cast<rbasic::Colour>(background));
}

void terminal_println(const std::string& text, int foreground, int background) {
    rbasic::Terminal::println(text, static_cast<rbasic::Colour>(foreground), 
                              static_cast<rbasic::Colour>(background));
}

BasicValue terminal_get_rows() {
    int rows, cols;
    rbasic::Terminal::getSize(rows, cols);
    return rows;
}

BasicValue terminal_get_cols() {
    int rows, cols;
    rbasic::Terminal::getSize(rows, cols);
    return cols;
}

bool terminal_kbhit() {
    return rbasic::Terminal::kbhit();
}

BasicValue terminal_getch() {
    return rbasic::Terminal::getch();
}

BasicValue terminal_getline(const std::string& prompt, int promptColour) {
    return rbasic::Terminal::getline(prompt, static_cast<rbasic::Colour>(promptColour));
}

void terminal_show_cursor(bool visible) {
    rbasic::Terminal::showCursor(visible);
}

void terminal_set_echo(bool enabled) {
    rbasic::Terminal::setEcho(enabled);
}

// Terminal wrapper functions for code generator (with func_ prefix)
BasicValue func_terminal_init() {
    return terminal_init();
}

BasicValue func_terminal_cleanup() {
    terminal_cleanup();
    return 0;
}

BasicValue func_terminal_supports_colour() {
    return terminal_supports_colour();
}

BasicValue func_terminal_clear() {
    terminal_clear();
    return 0;
}

BasicValue func_terminal_set_cursor(const BasicValue& row, const BasicValue& col) {
    terminal_set_cursor(to_int(row), to_int(col));
    return 0;
}

BasicValue func_terminal_get_cursor_row() {
    return terminal_get_cursor_row();
}

BasicValue func_terminal_get_cursor_col() {
    return terminal_get_cursor_col();
}

BasicValue func_terminal_save_cursor() {
    terminal_save_cursor();
    return 0;
}

BasicValue func_terminal_restore_cursor() {
    terminal_restore_cursor();
    return 0;
}

BasicValue func_terminal_set_colour(const BasicValue& foreground, const BasicValue& background) {
    terminal_set_colour(to_int(foreground), to_int(background));
    return 0;
}

BasicValue func_terminal_reset_colour() {
    terminal_reset_colour();
    return 0;
}

BasicValue func_terminal_print(const BasicValue& text) {
    terminal_print(to_string(text), -1, -1);
    return 0;
}

BasicValue func_terminal_print(const BasicValue& text, const BasicValue& foreground) {
    terminal_print(to_string(text), to_int(foreground), -1);
    return 0;
}

BasicValue func_terminal_print(const BasicValue& text, const BasicValue& foreground, const BasicValue& background) {
    terminal_print(to_string(text), to_int(foreground), to_int(background));
    return 0;
}

BasicValue func_terminal_println() {
    terminal_println("", -1, -1);
    return 0;
}

BasicValue func_terminal_println(const BasicValue& text) {
    terminal_println(to_string(text), -1, -1);
    return 0;
}

BasicValue func_terminal_println(const BasicValue& text, const BasicValue& foreground) {
    terminal_println(to_string(text), to_int(foreground), -1);
    return 0;
}

BasicValue func_terminal_println(const BasicValue& text, const BasicValue& foreground, const BasicValue& background) {
    terminal_println(to_string(text), to_int(foreground), to_int(background));
    return 0;
}

BasicValue func_terminal_get_rows() {
    return terminal_get_rows();
}

BasicValue func_terminal_get_cols() {
    return terminal_get_cols();
}

BasicValue func_terminal_kbhit() {
    return terminal_kbhit();
}

BasicValue func_terminal_getch() {
    return terminal_getch();
}

BasicValue func_terminal_getline() {
    return terminal_getline("", -1);
}

BasicValue func_terminal_getline(const BasicValue& prompt) {
    return terminal_getline(to_string(prompt), -1);
}

BasicValue func_terminal_getline(const BasicValue& prompt, const BasicValue& promptColour) {
    return terminal_getline(to_string(prompt), to_int(promptColour));
}

BasicValue func_terminal_show_cursor(const BasicValue& visible) {
    terminal_show_cursor(to_bool(visible));
    return 0;
}

BasicValue func_terminal_set_echo(const BasicValue& enabled) {
    terminal_set_echo(to_bool(enabled));
    return 0;
}

// Core constants (NULL, TRUE, FALSE)
BasicValue get_constant(const std::string& name) {
    // Core language constants
    if (name == "NULL" || name == "null") {
        return BasicValue(static_cast<void*>(nullptr));
    }
    if (name == "TRUE" || name == "true") {
        return BasicValue(true);
    }
    if (name == "FALSE" || name == "false") {
        return BasicValue(false);
    }
    
#ifdef SDL2_SUPPORT_ENABLED
    // SDL2 constants
    if (name.substr(0, 4) == "SDL_") {
        // Init flags
        if (name == "SDL_INIT_VIDEO") return BasicValue(constants::INIT_VIDEO);
        if (name == "SDL_INIT_AUDIO") return BasicValue(constants::INIT_AUDIO);
        if (name == "SDL_INIT_TIMER") return BasicValue(constants::INIT_TIMER);
        if (name == "SDL_INIT_JOYSTICK") return BasicValue(constants::INIT_JOYSTICK);
        if (name == "SDL_INIT_HAPTIC") return BasicValue(constants::INIT_HAPTIC);
        if (name == "SDL_INIT_GAMECONTROLLER") return BasicValue(constants::INIT_GAMECONTROLLER);
        if (name == "SDL_INIT_EVENTS") return BasicValue(constants::INIT_EVENTS);
        if (name == "SDL_INIT_EVERYTHING") return BasicValue(constants::INIT_EVERYTHING);
        
        // Window flags
        if (name == "SDL_WINDOW_FULLSCREEN") return BasicValue(constants::WINDOW_FULLSCREEN);
        if (name == "SDL_WINDOW_OPENGL") return BasicValue(constants::WINDOW_OPENGL);
        if (name == "SDL_WINDOW_SHOWN") return BasicValue(constants::WINDOW_SHOWN);
        if (name == "SDL_WINDOW_HIDDEN") return BasicValue(constants::WINDOW_HIDDEN);
        if (name == "SDL_WINDOW_BORDERLESS") return BasicValue(constants::WINDOW_BORDERLESS);
        if (name == "SDL_WINDOW_RESIZABLE") return BasicValue(constants::WINDOW_RESIZABLE);
        if (name == "SDL_WINDOW_MINIMIZED") return BasicValue(constants::WINDOW_MINIMIZED);
        if (name == "SDL_WINDOW_MAXIMIZED") return BasicValue(constants::WINDOW_MAXIMIZED);
        
        // Renderer flags
        if (name == "SDL_RENDERER_SOFTWARE") return BasicValue(constants::RENDERER_SOFTWARE);
        if (name == "SDL_RENDERER_ACCELERATED") return BasicValue(constants::RENDERER_ACCELERATED);
        if (name == "SDL_RENDERER_PRESENTVSYNC") return BasicValue(constants::RENDERER_PRESENTVSYNC);
        if (name == "SDL_RENDERER_TARGETTEXTURE") return BasicValue(constants::RENDERER_TARGETTEXTURE);
        
        // Event types
        if (name == "SDL_QUIT") return BasicValue(constants::EVENT_QUIT);
        if (name == "SDL_KEYDOWN") return BasicValue(constants::EVENT_KEYDOWN);
        if (name == "SDL_KEYUP") return BasicValue(constants::EVENT_KEYUP);
        if (name == "SDL_MOUSEMOTION") return BasicValue(constants::EVENT_MOUSEMOTION);
        if (name == "SDL_MOUSEBUTTONDOWN") return BasicValue(constants::EVENT_MOUSEBUTTONDOWN);
        if (name == "SDL_MOUSEBUTTONUP") return BasicValue(constants::EVENT_MOUSEBUTTONUP);
        if (name == "SDL_MOUSEWHEEL") return BasicValue(constants::EVENT_MOUSEWHEEL);
        if (name == "SDL_WINDOWEVENT") return BasicValue(constants::EVENT_WINDOWEVENT);
        
        // Window position
        if (name == "SDL_WINDOWPOS_UNDEFINED") return BasicValue(constants::WINDOWPOS_UNDEFINED);
        if (name == "SDL_WINDOWPOS_CENTERED") return BasicValue(constants::WINDOWPOS_CENTERED);
        
        // Scan codes (dynamically extract from name)
        if (name.substr(0, 13) == "SDL_SCANCODE_") {
            std::string scancode_name = name.substr(4); // Remove "SDL_"
            return BasicValue(SDL_GetScancodeFromName(scancode_name.c_str()));
        }
    }
#endif
    
#ifdef SQLITE3_SUPPORT_ENABLED
    // SQLite3 constants
    if (name.substr(0, 7) == "SQLITE_") {
        // Result codes
        if (name == "SQLITE_OK") return BasicValue(constants::OK);
        if (name == "SQLITE_ERROR") return BasicValue(constants::ERROR);
        if (name == "SQLITE_BUSY") return BasicValue(constants::BUSY);
        if (name == "SQLITE_LOCKED") return BasicValue(constants::LOCKED);
        if (name == "SQLITE_NOMEM") return BasicValue(constants::NOMEM);
        if (name == "SQLITE_READONLY") return BasicValue(constants::READONLY);
        if (name == "SQLITE_INTERRUPT") return BasicValue(constants::INTERRUPT);
        if (name == "SQLITE_IOERR") return BasicValue(constants::IOERR);
        if (name == "SQLITE_CORRUPT") return BasicValue(constants::CORRUPT);
        if (name == "SQLITE_NOTFOUND") return BasicValue(constants::NOTFOUND);
        if (name == "SQLITE_FULL") return BasicValue(constants::FULL);
        if (name == "SQLITE_CANTOPEN") return BasicValue(constants::CANTOPEN);
        if (name == "SQLITE_PROTOCOL") return BasicValue(constants::PROTOCOL);
        if (name == "SQLITE_EMPTY") return BasicValue(constants::EMPTY);
        if (name == "SQLITE_SCHEMA") return BasicValue(constants::SCHEMA);
        if (name == "SQLITE_TOOBIG") return BasicValue(constants::TOOBIG);
        if (name == "SQLITE_CONSTRAINT") return BasicValue(constants::CONSTRAINT);
        if (name == "SQLITE_MISMATCH") return BasicValue(constants::MISMATCH);
        if (name == "SQLITE_MISUSE") return BasicValue(constants::MISUSE);
        if (name == "SQLITE_NOLFS") return BasicValue(constants::NOLFS);
        if (name == "SQLITE_AUTH") return BasicValue(constants::AUTH);
        if (name == "SQLITE_FORMAT") return BasicValue(constants::FORMAT);
        if (name == "SQLITE_RANGE") return BasicValue(constants::RANGE);
        if (name == "SQLITE_NOTADB") return BasicValue(constants::NOTADB);
        if (name == "SQLITE_ROW") return BasicValue(constants::ROW);
        if (name == "SQLITE_DONE") return BasicValue(constants::DONE);
        
        // Column types
        if (name == "SQLITE_INTEGER") return BasicValue(constants::INTEGER);
        if (name == "SQLITE_FLOAT") return BasicValue(constants::FLOAT);
        if (name == "SQLITE_TEXT") return BasicValue(constants::TEXT);
        if (name == "SQLITE_BLOB") return BasicValue(constants::BLOB);
        if (name == "SQLITE_NULL") return BasicValue(constants::NULL_TYPE);
        
        // Open flags
        if (name == "SQLITE_OPEN_READONLY") return BasicValue(constants::OPEN_READONLY);
        if (name == "SQLITE_OPEN_READWRITE") return BasicValue(constants::OPEN_READWRITE);
        if (name == "SQLITE_OPEN_CREATE") return BasicValue(constants::OPEN_CREATE);
    }
#endif
    
    // Unknown constant
    throw std::runtime_error("Unknown constant: " + name);
}

BasicValue is_null(const BasicValue& value) {
    if (std::holds_alternative<void*>(value)) {
        return BasicValue(std::get<void*>(value) == nullptr);
    }
    return BasicValue(false);
}

BasicValue not_null(const BasicValue& value) {
    if (std::holds_alternative<void*>(value)) {
        return BasicValue(std::get<void*>(value) != nullptr);
    }
    return BasicValue(true);
}

// ===== WRAPPER FUNCTIONS FOR CODE GENERATOR =====

BasicValue func_get_constant(const BasicValue& name) {
    std::string nameStr = to_string(name);
    return get_constant(nameStr);
}

BasicValue func_is_null(const BasicValue& value) {
    return is_null(value);
}

BasicValue func_not_null(const BasicValue& value) {
    return not_null(value);
}

// ===================================================================
// SDL2 Graphics Support Implementation
// ===================================================================
#ifdef SDL2_SUPPORT_ENABLED

// Core SDL functions
BasicValue func_sdl_init(const BasicValue& flags) {
    int result = sdl_init(to_int(flags));
    return BasicValue(result);
}

BasicValue func_sdl_quit() {
    sdl_quit();
    return BasicValue(0);
}

BasicValue func_sdl_get_error() {
    return BasicValue(sdl_get_error());
}

// Window functions
BasicValue func_sdl_create_window(const BasicValue& title, const BasicValue& x, const BasicValue& y,
                                  const BasicValue& w, const BasicValue& h, const BasicValue& flags) {
    int handle = sdl_create_window(to_string(title), to_int(x), to_int(y),
                                                  to_int(w), to_int(h), to_int(flags));
    return BasicValue(handle);
}

BasicValue func_sdl_destroy_window(const BasicValue& window_handle) {
    sdl_destroy_window(to_int(window_handle));
    return BasicValue(0);
}

BasicValue func_sdl_set_window_title(const BasicValue& window_handle, const BasicValue& title) {
    sdl_set_window_title(to_int(window_handle), to_string(title));
    return BasicValue(0);
}

BasicValue func_sdl_set_window_size(const BasicValue& window_handle, const BasicValue& w, const BasicValue& h) {
    sdl_set_window_size(to_int(window_handle), to_int(w), to_int(h));
    return BasicValue(0);
}

BasicValue func_sdl_set_window_position(const BasicValue& window_handle, const BasicValue& x, const BasicValue& y) {
    sdl_set_window_position(to_int(window_handle), to_int(x), to_int(y));
    return BasicValue(0);
}

BasicValue func_sdl_show_window(const BasicValue& window_handle) {
    sdl_show_window(to_int(window_handle));
    return BasicValue(0);
}

BasicValue func_sdl_hide_window(const BasicValue& window_handle) {
    sdl_hide_window(to_int(window_handle));
    return BasicValue(0);
}

// Renderer functions
BasicValue func_sdl_create_renderer(const BasicValue& window_handle, const BasicValue& index, const BasicValue& flags) {
    int handle = sdl_create_renderer(to_int(window_handle), to_int(index), to_int(flags));
    return BasicValue(handle);
}

BasicValue func_sdl_destroy_renderer(const BasicValue& renderer_handle) {
    sdl_destroy_renderer(to_int(renderer_handle));
    return BasicValue(0);
}

BasicValue func_sdl_render_clear(const BasicValue& renderer_handle) {
    sdl_render_clear(to_int(renderer_handle));
    return BasicValue(0);
}

BasicValue func_sdl_render_present(const BasicValue& renderer_handle) {
    sdl_render_present(to_int(renderer_handle));
    return BasicValue(0);
}

BasicValue func_sdl_set_render_draw_color(const BasicValue& renderer_handle, const BasicValue& r,
                                          const BasicValue& g, const BasicValue& b, const BasicValue& a) {
    sdl_set_render_draw_color(to_int(renderer_handle), to_int(r), to_int(g), to_int(b), to_int(a));
    return BasicValue(0);
}

// Drawing primitives
BasicValue func_sdl_render_draw_point(const BasicValue& renderer_handle, const BasicValue& x, const BasicValue& y) {
    sdl_render_draw_point(to_int(renderer_handle), to_int(x), to_int(y));
    return BasicValue(0);
}

BasicValue func_sdl_render_draw_line(const BasicValue& renderer_handle, const BasicValue& x1, const BasicValue& y1,
                                     const BasicValue& x2, const BasicValue& y2) {
    sdl_render_draw_line(to_int(renderer_handle), to_int(x1), to_int(y1), to_int(x2), to_int(y2));
    return BasicValue(0);
}

BasicValue func_sdl_render_draw_rect(const BasicValue& renderer_handle, const BasicValue& x, const BasicValue& y,
                                     const BasicValue& w, const BasicValue& h) {
    sdl_render_draw_rect(to_int(renderer_handle), to_int(x), to_int(y), to_int(w), to_int(h));
    return BasicValue(0);
}

BasicValue func_sdl_render_fill_rect(const BasicValue& renderer_handle, const BasicValue& x, const BasicValue& y,
                                     const BasicValue& w, const BasicValue& h) {
    sdl_render_fill_rect(to_int(renderer_handle), to_int(x), to_int(y), to_int(w), to_int(h));
    return BasicValue(0);
}

// Advanced drawing (SDL2_gfx)
#ifdef SDL2_GFX_AVAILABLE
BasicValue func_sdl_render_draw_circle(const BasicValue& renderer_handle, const BasicValue& x,
                                       const BasicValue& y, const BasicValue& radius) {
    sdl_render_draw_circle(to_int(renderer_handle), to_int(x), to_int(y), to_int(radius));
    return BasicValue(0);
}

BasicValue func_sdl_render_fill_circle(const BasicValue& renderer_handle, const BasicValue& x,
                                       const BasicValue& y, const BasicValue& radius) {
    sdl_render_fill_circle(to_int(renderer_handle), to_int(x), to_int(y), to_int(radius));
    return BasicValue(0);
}

BasicValue func_sdl_render_draw_ellipse(const BasicValue& renderer_handle, const BasicValue& x, const BasicValue& y,
                                        const BasicValue& rx, const BasicValue& ry) {
    sdl_render_draw_ellipse(to_int(renderer_handle), to_int(x), to_int(y), to_int(rx), to_int(ry));
    return BasicValue(0);
}

BasicValue func_sdl_render_fill_ellipse(const BasicValue& renderer_handle, const BasicValue& x, const BasicValue& y,
                                        const BasicValue& rx, const BasicValue& ry) {
    sdl_render_fill_ellipse(to_int(renderer_handle), to_int(x), to_int(y), to_int(rx), to_int(ry));
    return BasicValue(0);
}

BasicValue func_sdl_render_draw_triangle(const BasicValue& renderer_handle,
                                         const BasicValue& x1, const BasicValue& y1,
                                         const BasicValue& x2, const BasicValue& y2,
                                         const BasicValue& x3, const BasicValue& y3) {
    sdl_render_draw_triangle(to_int(renderer_handle), 
                                           to_int(x1), to_int(y1),
                                           to_int(x2), to_int(y2),
                                           to_int(x3), to_int(y3));
    return BasicValue(0);
}

BasicValue func_sdl_render_fill_triangle(const BasicValue& renderer_handle,
                                         const BasicValue& x1, const BasicValue& y1,
                                         const BasicValue& x2, const BasicValue& y2,
                                         const BasicValue& x3, const BasicValue& y3) {
    sdl_render_fill_triangle(to_int(renderer_handle),
                                           to_int(x1), to_int(y1),
                                           to_int(x2), to_int(y2),
                                           to_int(x3), to_int(y3));
    return BasicValue(0);
}
#endif

// Texture functions
BasicValue func_sdl_load_texture(const BasicValue& renderer_handle, const BasicValue& filename) {
    int handle = sdl_load_texture(to_int(renderer_handle), to_string(filename));
    return BasicValue(handle);
}

BasicValue func_sdl_destroy_texture(const BasicValue& texture_handle) {
    sdl_destroy_texture(to_int(texture_handle));
    return BasicValue(0);
}

BasicValue func_sdl_render_copy(const BasicValue& renderer_handle, const BasicValue& texture_handle,
                                const BasicValue& src_x, const BasicValue& src_y,
                                const BasicValue& src_w, const BasicValue& src_h,
                                const BasicValue& dst_x, const BasicValue& dst_y,
                                const BasicValue& dst_w, const BasicValue& dst_h) {
    sdl_render_copy(to_int(renderer_handle), to_int(texture_handle),
                                  to_int(src_x), to_int(src_y), to_int(src_w), to_int(src_h),
                                  to_int(dst_x), to_int(dst_y), to_int(dst_w), to_int(dst_h));
    return BasicValue(0);
}

BasicValue func_sdl_query_texture(const BasicValue& texture_handle) {
    int format, access, w, h;
    sdl_query_texture(to_int(texture_handle), &format, &access, &w, &h);
    
    // Return as a struct with width and height
    BasicStruct result("SDL_TextureInfo");
    result.fields["width"] = BasicValue(w);
    result.fields["height"] = BasicValue(h);
    result.fields["format"] = BasicValue(format);
    result.fields["access"] = BasicValue(access);
    return BasicValue(result);
}

// Event functions
BasicValue func_sdl_poll_event() {
    int result = sdl_poll_event();
    return BasicValue(result);
}

BasicValue func_sdl_get_event_type() {
    int type = sdl_get_event_type();
    return BasicValue(type);
}

BasicValue func_sdl_get_key_scancode() {
    int scancode = sdl_get_key_scancode();
    return BasicValue(scancode);
}

BasicValue func_sdl_get_key_keycode() {
    int keycode = sdl_get_key_keycode();
    return BasicValue(keycode);
}

BasicValue func_sdl_get_mouse_x() {
    int x = sdl_get_mouse_x();
    return BasicValue(x);
}

BasicValue func_sdl_get_mouse_y() {
    int y = sdl_get_mouse_y();
    return BasicValue(y);
}

BasicValue func_sdl_get_mouse_button() {
    int button = sdl_get_mouse_button();
    return BasicValue(button);
}

// Delay function
BasicValue func_sdl_delay(const BasicValue& ms) {
    sdl_delay(to_int(ms));
    return BasicValue(0);
}

#endif // SDL2_SUPPORT_ENABLED

// ===================================================================
// SQLite3 Database Support Implementation
// ===================================================================
#ifdef SQLITE3_SUPPORT_ENABLED



// Core database functions
BasicValue func_sqlite_open(const BasicValue& filename) {
    int handle = sqlite_open(to_string(filename));
    return BasicValue(handle);
}

BasicValue func_sqlite_close(const BasicValue& db_handle) {
    sqlite_close(to_int(db_handle));
    return BasicValue(0);
}

BasicValue func_sqlite_exec(const BasicValue& db_handle, const BasicValue& sql) {
    int result = sqlite_exec(to_int(db_handle), to_string(sql));
    return BasicValue(result);
}

BasicValue func_sqlite_errmsg(const BasicValue& db_handle) {
    return BasicValue(sqlite_errmsg(to_int(db_handle)));
}

BasicValue func_sqlite_get_last_error_code(const BasicValue& db_handle) {
    int code = sqlite_get_last_error_code(to_int(db_handle));
    return BasicValue(code);
}

BasicValue func_sqlite_last_insert_rowid(const BasicValue& db_handle) {
    long long rowid = sqlite_last_insert_rowid(to_int(db_handle));
    return BasicValue(static_cast<int>(rowid));
}

BasicValue func_sqlite_changes(const BasicValue& db_handle) {
    int changes = sqlite_changes(to_int(db_handle));
    return BasicValue(changes);
}

// Prepared statement functions
BasicValue func_sqlite_prepare(const BasicValue& db_handle, const BasicValue& sql) {
    int handle = sqlite_prepare(to_int(db_handle), to_string(sql));
    return BasicValue(handle);
}

BasicValue func_sqlite_finalize(const BasicValue& stmt_handle) {
    int result = sqlite_finalize(to_int(stmt_handle));
    return BasicValue(result);
}

BasicValue func_sqlite_reset(const BasicValue& stmt_handle) {
    int result = sqlite_reset(to_int(stmt_handle));
    return BasicValue(result);
}

BasicValue func_sqlite_clear_bindings(const BasicValue& stmt_handle) {
    int result = sqlite_clear_bindings(to_int(stmt_handle));
    return BasicValue(result);
}

// Binding functions
BasicValue func_sqlite_bind_int(const BasicValue& stmt_handle, const BasicValue& index, const BasicValue& value) {
    int result = sqlite_bind_int(to_int(stmt_handle), to_int(index), to_int(value));
    return BasicValue(result);
}

BasicValue func_sqlite_bind_int64(const BasicValue& stmt_handle, const BasicValue& index, const BasicValue& value) {
    long long val = static_cast<long long>(to_int(value));
    int result = sqlite_bind_int64(to_int(stmt_handle), to_int(index), val);
    return BasicValue(result);
}

BasicValue func_sqlite_bind_double(const BasicValue& stmt_handle, const BasicValue& index, const BasicValue& value) {
    int result = sqlite_bind_double(to_int(stmt_handle), to_int(index), to_double(value));
    return BasicValue(result);
}

BasicValue func_sqlite_bind_text(const BasicValue& stmt_handle, const BasicValue& index, const BasicValue& value) {
    int result = sqlite_bind_text(to_int(stmt_handle), to_int(index), to_string(value));
    return BasicValue(result);
}

BasicValue func_sqlite_bind_null(const BasicValue& stmt_handle, const BasicValue& index) {
    int result = sqlite_bind_null(to_int(stmt_handle), to_int(index));
    return BasicValue(result);
}

// Step and column access
BasicValue func_sqlite_step(const BasicValue& stmt_handle) {
    int result = sqlite_step(to_int(stmt_handle));
    return BasicValue(result);
}

BasicValue func_sqlite_column_count(const BasicValue& stmt_handle) {
    int count = sqlite_column_count(to_int(stmt_handle));
    return BasicValue(count);
}

BasicValue func_sqlite_column_type(const BasicValue& stmt_handle, const BasicValue& index) {
    int type = sqlite_column_type(to_int(stmt_handle), to_int(index));
    return BasicValue(type);
}

BasicValue func_sqlite_column_name(const BasicValue& stmt_handle, const BasicValue& index) {
    return BasicValue(sqlite_column_name(to_int(stmt_handle), to_int(index)));
}

// Column retrieval functions
BasicValue func_sqlite_column_int(const BasicValue& stmt_handle, const BasicValue& index) {
    int value = sqlite_column_int(to_int(stmt_handle), to_int(index));
    return BasicValue(value);
}

BasicValue func_sqlite_column_int64(const BasicValue& stmt_handle, const BasicValue& index) {
    long long value = sqlite_column_int64(to_int(stmt_handle), to_int(index));
    return BasicValue(static_cast<int>(value));
}

BasicValue func_sqlite_column_double(const BasicValue& stmt_handle, const BasicValue& index) {
    double value = sqlite_column_double(to_int(stmt_handle), to_int(index));
    return BasicValue(value);
}

BasicValue func_sqlite_column_text(const BasicValue& stmt_handle, const BasicValue& index) {
    return BasicValue(sqlite_column_text(to_int(stmt_handle), to_int(index)));
}

// Utility functions
BasicValue func_sqlite_version() {
    return BasicValue(sqlite_version());
}

BasicValue func_sqlite_threadsafe() {
    int result = sqlite_threadsafe();
    return BasicValue(result);
}

// Transaction helpers
BasicValue func_sqlite_begin_transaction(const BasicValue& db_handle) {
    int result = sqlite_begin_transaction(to_int(db_handle));
    return BasicValue(result);
}

BasicValue func_sqlite_commit_transaction(const BasicValue& db_handle) {
    int result = sqlite_commit_transaction(to_int(db_handle));
    return BasicValue(result);
}

BasicValue func_sqlite_rollback_transaction(const BasicValue& db_handle) {
    int result = sqlite_rollback_transaction(to_int(db_handle));
    return BasicValue(result);
}

#endif // SQLITE3_SUPPORT_ENABLED

} // namespace basic_runtime
