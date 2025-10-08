#include "basic_runtime.h"
#include "../include/io_handler.h"
#include "../include/common.h"
#include "../include/terminal.h"
#include "../include/ffi.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdlib>

#ifdef _WIN32
// Undefine Windows macros that conflict with std::min/std::max
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

#ifdef _OPENMP
#include <omp.h>
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
    if (std::holds_alternative<std::string>(left) || std::holds_alternative<std::string>(right)) {
        return to_string(left) + to_string(right);
    } else if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) + to_double(right);
    } else {
        return to_int(left) + to_int(right);
    }
}

BasicValue subtract(const BasicValue& left, const BasicValue& right) {
    if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right)) {
        return to_double(left) - to_double(right);
    } else {
        return to_int(left) - to_int(right);
    }
}

BasicValue multiply(const BasicValue& left, const BasicValue& right) {
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

bool terminal_supports_color() {
    return rbasic::Terminal::supportsColor();
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

void terminal_set_color(int foreground, int background) {
    rbasic::Terminal::setColor(static_cast<rbasic::Color>(foreground), 
                               static_cast<rbasic::Color>(background));
}

void terminal_reset_color() {
    rbasic::Terminal::resetColor();
}

void terminal_print(const std::string& text, int foreground, int background) {
    rbasic::Terminal::print(text, static_cast<rbasic::Color>(foreground), 
                            static_cast<rbasic::Color>(background));
}

void terminal_println(const std::string& text, int foreground, int background) {
    rbasic::Terminal::println(text, static_cast<rbasic::Color>(foreground), 
                              static_cast<rbasic::Color>(background));
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

BasicValue terminal_getline(const std::string& prompt, int promptColor) {
    return rbasic::Terminal::getline(prompt, static_cast<rbasic::Color>(promptColor));
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

BasicValue func_terminal_supports_color() {
    return terminal_supports_color();
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

BasicValue func_terminal_set_color(const BasicValue& foreground, const BasicValue& background) {
    terminal_set_color(to_int(foreground), to_int(background));
    return 0;
}

BasicValue func_terminal_reset_color() {
    terminal_reset_color();
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

BasicValue func_terminal_getline(const BasicValue& prompt, const BasicValue& promptColor) {
    return terminal_getline(to_string(prompt), to_int(promptColor));
}

BasicValue func_terminal_show_cursor(const BasicValue& visible) {
    terminal_show_cursor(to_bool(visible));
    return 0;
}

BasicValue func_terminal_set_echo(const BasicValue& enabled) {
    terminal_set_echo(to_bool(enabled));
    return 0;
}

// Foreign Function Interface (FFI) implementation - Phase 1 Simplified
BasicValue load_library(const std::string& library_name) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.load_library(library_name);
        
        if (library && library->is_valid()) {
            // For Phase 1, return a simple string identifier
            return BasicValue(std::string("library_handle:" + library_name));
        } else {
            return BasicValue(std::string("error:Failed to load " + library_name));
        }
    } catch (const rbasic::ffi::FFIError& e) {
        std::cerr << "FFI Error: " << e.what() << std::endl;
        return BasicValue(std::string("error:" + std::string(e.what())));
    }
}

BasicValue unload_library(const BasicValue& library_handle) {
    if (std::holds_alternative<std::string>(library_handle)) {
        const auto& handle_str = std::get<std::string>(library_handle);
        if (handle_str.length() >= 15 && handle_str.substr(0, 15) == "library_handle:") {
            std::string lib_name = handle_str.substr(15); // Remove "library_handle:" prefix
            auto& ffi_manager = rbasic::ffi::FFIManager::instance();
            bool success = ffi_manager.unload_library(lib_name);
            return BasicValue(success ? 1.0 : 0.0); // Return double for consistency
        }
    }
    return BasicValue(0.0);
}

bool is_library_loaded(const BasicValue& library_handle) {
    if (std::holds_alternative<std::string>(library_handle)) {
        const auto& handle_str = std::get<std::string>(library_handle);
        return handle_str.length() >= 15 && handle_str.substr(0, 15) == "library_handle:";
    }
    return false;
}

// FFI function calling for compiled programs
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* funcPtr = library->get_function_address(function_name);
        if (!funcPtr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Call no-parameter function returning int
        typedef int (__cdecl *FuncType0)();
        auto func = reinterpret_cast<FuncType0>(funcPtr);
        return BasicValue(static_cast<double>(func()));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* funcPtr = library->get_function_address(function_name);
        if (!funcPtr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic parameter conversion for 1-parameter functions
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val).c_str();
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        // Try different 1-parameter patterns in order of likelihood
        
        // Pattern: (string) -> pointer (IMG_Load and similar)
        if (std::holds_alternative<std::string>(arg1) && 
            (function_name == "IMG_Load" || function_name.find("Load") != std::string::npos)) {
            typedef void* (__cdecl *FuncType)(const char*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsString(arg1));
            return BasicValue(result);
        }
        
        // Pattern: (int) -> pointer (malloc, calloc, and similar memory allocation functions)
        if ((std::holds_alternative<double>(arg1) || std::holds_alternative<int>(arg1)) && 
            (function_name == "malloc" || function_name == "calloc" || function_name.find("alloc") != std::string::npos)) {
            typedef void* (__cdecl *FuncType)(size_t);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(static_cast<size_t>(getAsInt(arg1)));
            return BasicValue(result);
        }
        
        // Pattern: (pointer) -> int (common for SQLite close functions)
        if (std::holds_alternative<void*>(arg1)) {
            typedef int (__cdecl *FuncType)(void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (int) -> int (most common case)
        if (std::holds_alternative<double>(arg1) || std::holds_alternative<int>(arg1)) {
            typedef int (__cdecl *FuncType)(int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsInt(arg1));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (string) -> int
        if (std::holds_alternative<std::string>(arg1)) {
            typedef int (__cdecl *FuncType)(const char*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsString(arg1));
            return BasicValue(static_cast<double>(result));
        }
        
        // Fallback: assume (int) -> int
        typedef int (__cdecl *FuncType)(int);
        auto func = reinterpret_cast<FuncType>(funcPtr);
        int result = func(getAsInt(arg1));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* funcPtr = library->get_function_address(function_name);
        if (!funcPtr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic 2-parameter FFI call with automatic type conversion
        
        // Helper lambdas for converting BasicValue to appropriate C types
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) return std::get<void*>(val);
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> std::string {
            if (std::holds_alternative<std::string>(val)) return std::get<std::string>(val);
            return "";
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<int>(val)) return std::get<int>(val);
            if (std::holds_alternative<double>(val)) return static_cast<int>(std::get<double>(val));
            return 0;
        };
        
        // Extract parameters with automatic type conversion
        void* p1 = getAsPointer(arg1);
        std::string s1 = getAsString(arg1);
        int i1 = getAsInt(arg1);
        
        void* p2 = getAsPointer(arg2);
        std::string s2 = getAsString(arg2);
        int i2 = getAsInt(arg2);
        
        // Try different 2-parameter patterns based on function name and parameter types
        
        // Pattern: (pointer, pointer) -> pointer (SDL_CreateTextureFromSurface)
        if (function_name == "SDL_CreateTextureFromSurface" && 
            std::holds_alternative<void*>(arg1) && std::holds_alternative<void*>(arg2)) {
            typedef void* (__cdecl *FuncType)(void*, void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsPointer(arg1), getAsPointer(arg2));
            return BasicValue(result);
        }
        
        // Pattern: (string, int) -> pointer (TTF_OpenFont and similar font loading functions)
        if (std::holds_alternative<std::string>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (function_name == "TTF_OpenFont" || function_name.find("OpenFont") != std::string::npos)) {
            typedef void* (__cdecl *FuncType)(const char*, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsString(arg1).c_str(), getAsInt(arg2));
            return BasicValue(result);
        }
        
        // Pattern: (string, string) -> pointer (fopen and similar file functions)
        if (std::holds_alternative<std::string>(arg1) && std::holds_alternative<std::string>(arg2) &&
            (function_name == "fopen" || function_name == "freopen" || function_name.find("open") != std::string::npos)) {
            typedef void* (__cdecl *FuncType)(const char*, const char*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsString(arg1).c_str(), getAsString(arg2).c_str());
            return BasicValue(result);
        }
        
        // Pattern: (pointer, pointer) -> int (SDL2 RenderDrawRect, RenderFillRect)
        if (std::holds_alternative<void*>(arg1) && std::holds_alternative<void*>(arg2)) {
            typedef int (__cdecl *FuncType)(void*, void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsPointer(arg2));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern 1: (string, pointer) -> int (SQLite open)
        if (!s1.empty() && p2 != nullptr) {
            typedef int (__cdecl *Func1)(const char*, void*);
            auto func = reinterpret_cast<Func1>(funcPtr);
            return BasicValue(static_cast<double>(func(s1.c_str(), p2)));
        }
        
        // For pointer return functions (sqlite3_column_text, sqlite3_column_name)
        if (p1 != nullptr && (function_name == "sqlite3_column_text" || function_name == "sqlite3_column_name")) {
            typedef void* (__cdecl *FuncPtr)(void*, int);
            auto func = reinterpret_cast<FuncPtr>(funcPtr);
            void* result = func(p1, i2);
            return BasicValue(result);
        }
        
        // For double return functions (sqlite3_column_double)
        if (p1 != nullptr && function_name == "sqlite3_column_double") {
            typedef double (__cdecl *FuncDouble)(void*, int);
            auto func = reinterpret_cast<FuncDouble>(funcPtr);
            double result = func(p1, i2);
            return BasicValue(result);
        }
        
        // Pattern 2: (pointer, int) -> int (most common: column_int, changes, etc.)
        if (p1 != nullptr) {
            typedef int (__cdecl *Func2)(void*, int);
            auto func = reinterpret_cast<Func2>(funcPtr);
            return BasicValue(static_cast<double>(func(p1, i2)));
        }
        
        // Pattern: (pointer, string) -> void (SDL_SetWindowTitle)
        if (std::holds_alternative<void*>(arg1) && std::holds_alternative<std::string>(arg2)) {
            typedef void (__cdecl *FuncType)(void*, const char*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            func(getAsPointer(arg1), getAsString(arg2).c_str());
            return BasicValue(0.0);
        }
        
        // Pattern 3: (int, int) -> int (all integers) - Windows API
        typedef int (__stdcall *FuncInt2)(int, int);
        auto func_int = reinterpret_cast<FuncInt2>(funcPtr);
        return BasicValue(static_cast<double>(func_int(i1, i2)));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* funcPtr = library->get_function_address(function_name);
        if (!funcPtr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic parameter conversion for 3-parameter functions
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val).c_str();
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsDouble = [](const BasicValue& val) -> double {
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            } else if (std::holds_alternative<int>(val)) {
                return static_cast<double>(std::get<int>(val));
            }
            return 0.0;
        };
        
        // Try different return type patterns in order of likelihood
        
        // Pattern: (pointer, int, int) -> pointer (SDL_CreateRenderer)
        if (std::holds_alternative<void*>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3))) {
            
            typedef void* (__cdecl *FuncType)(void*, int, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3));
            if (result != nullptr) {
                return BasicValue(result);  // Return as void* if non-null
            }
        }
        
        // Pattern: (string, int, int) -> pointer (TTF_OpenFontIndex and similar font functions)
        if (std::holds_alternative<std::string>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3)) &&
            (function_name == "TTF_OpenFontIndex" || function_name.find("OpenFont") != std::string::npos)) {
            
            typedef void* (__cdecl *FuncType)(const char*, int, long);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsString(arg1), getAsInt(arg2), static_cast<long>(getAsInt(arg3)));
            return BasicValue(result);
        }
        
        // Pattern: (pointer, int, int) -> int (common for SQLite bind functions)
        if (std::holds_alternative<void*>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3))) {
            
            typedef int (__cdecl *FuncType)(void*, int, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, string) -> int (for bind_text with 3 params)
        if (std::holds_alternative<void*>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            std::holds_alternative<std::string>(arg3)) {
            
            typedef int (__cdecl *FuncType)(void*, int, const char*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsString(arg3));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, double) -> int
        if (std::holds_alternative<void*>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            std::holds_alternative<double>(arg3)) {
            
            typedef int (__cdecl *FuncType)(void*, int, double);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsDouble(arg3));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, int) -> pointer (SDL_CreateRenderer style)
        if (std::holds_alternative<void*>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3))) {
            
            typedef void* (__cdecl *FuncType)(void*, int, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            void* result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3));
            return BasicValue(result);
        }
        
        // Fallback: try as (pointer, int, int) -> int
        typedef int (__cdecl *FuncType)(void*, int, int);
        auto func = reinterpret_cast<FuncType>(funcPtr);
        int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3, const BasicValue& arg4) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* funcPtr = library->get_function_address(function_name);
        if (!funcPtr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic parameter conversion for 4-parameter functions
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val).c_str();
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsDouble = [](const BasicValue& val) -> double {
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            } else if (std::holds_alternative<int>(val)) {
                return static_cast<double>(std::get<int>(val));
            }
            return 0.0;
        };
        
        // Try different 4-parameter patterns in order of likelihood
        
        // Pattern: (int, string, string, int) -> int (MessageBoxA style)
        // Pattern: (int, string, string, int) -> int (Windows API style)
        if ((std::holds_alternative<double>(arg1) || std::holds_alternative<int>(arg1)) &&
            std::holds_alternative<std::string>(arg2) &&
            std::holds_alternative<std::string>(arg3) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4))) {
            
            typedef int (__stdcall *FuncType)(int, const char*, const char*, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsInt(arg1), getAsString(arg2), getAsString(arg3), getAsInt(arg4));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, int, int) -> int
        if (std::holds_alternative<void*>(arg1) &&
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3)) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4))) {
            
            typedef int (__cdecl *FuncType)(void*, int, int, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), getAsInt(arg4));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, string, int) -> int
        if (std::holds_alternative<void*>(arg1) &&
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            std::holds_alternative<std::string>(arg3) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4))) {
            
            typedef int (__cdecl *FuncType)(void*, int, const char*, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsString(arg3), getAsInt(arg4));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, int, pointer) -> int 
        if (std::holds_alternative<void*>(arg1) &&
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3)) &&
            std::holds_alternative<void*>(arg4)) {
            
            typedef int (__cdecl *FuncType)(void*, int, int, void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), getAsPointer(arg4));
            return BasicValue(static_cast<double>(result));
        }        // Pattern: (pointer, pointer, pointer, pointer) -> int - SDL_RenderCopy
        if (std::holds_alternative<void*>(arg1) &&
            std::holds_alternative<void*>(arg2) &&
            std::holds_alternative<void*>(arg3) &&
            std::holds_alternative<void*>(arg4)) {
            
            typedef int (__cdecl *FuncType)(void*, void*, void*, void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsPointer(arg2), getAsPointer(arg3), getAsPointer(arg4));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, int, int) -> pointer - SDL_CreateTexture
        if (std::holds_alternative<void*>(arg1) &&
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3)) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4))) {
            
            // Try as pointer return first
            typedef void* (__cdecl *FuncTypePtrRet)(void*, int, int, int);
            auto funcPtrRet = reinterpret_cast<FuncTypePtrRet>(funcPtr);
            void* result = funcPtrRet(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), getAsInt(arg4));
            if (result != nullptr) {
                return BasicValue(result);
            }
            
            // Fallback to int return
            typedef int (__cdecl *FuncTypeIntRet)(void*, int, int, int);
            auto funcIntRet = reinterpret_cast<FuncTypeIntRet>(funcPtr);
            int intResult = funcIntRet(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), getAsInt(arg4));
            return BasicValue(static_cast<double>(intResult));
        }
        
        // Pattern: (pointer, pointer, pointer, int) -> int - SDL_UpdateTexture
        if (std::holds_alternative<void*>(arg1) &&
            std::holds_alternative<void*>(arg2) &&
            std::holds_alternative<void*>(arg3) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4))) {
            
            typedef int (__cdecl *FuncType)(void*, void*, void*, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsPointer(arg2), getAsPointer(arg3), getAsInt(arg4));
            return BasicValue(static_cast<double>(result));
        }
        
        // Fallback: assume (int, string, string, int) -> int
        typedef int (__stdcall *FuncType)(int, const char*, const char*, int);
        auto func = reinterpret_cast<FuncType>(funcPtr);
        int result = func(getAsInt(arg1), getAsString(arg2), getAsString(arg3), getAsInt(arg4));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

// 5-parameter FFI function calls
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* funcPtr = library->get_function_address(function_name);
        if (!funcPtr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic 5-parameter FFI call - COPY 3-PARAM APPROACH EXACTLY
        // Convert all parameters to their most appropriate C types
        
        // Helper lambda for converting BasicValue to appropriate C types - CONSISTENT WITH 3-PARAM
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val).c_str();
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        // No need to pre-extract parameters - use helper functions directly in pattern matching
        
        // Pattern: (pointer, int, int, int, int) -> int (SDL2 SetRenderDrawColor)
        if (std::holds_alternative<void*>(arg1) && 
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3)) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4)) &&
            (std::holds_alternative<double>(arg5) || std::holds_alternative<int>(arg5))) {
            
            typedef int (__cdecl *FuncType)(void*, int, int, int, int);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), getAsInt(arg4), getAsInt(arg5));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, string, int, pointer, pointer) -> int (SQLite prepare)
        if (std::holds_alternative<void*>(arg1) && 
            std::holds_alternative<std::string>(arg2) &&
            (std::holds_alternative<double>(arg3) || std::holds_alternative<int>(arg3)) &&
            std::holds_alternative<void*>(arg4) &&
            std::holds_alternative<void*>(arg5)) {
            
            typedef int (__cdecl *FuncType)(void*, const char*, int, void*, void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsString(arg2), getAsInt(arg3), getAsPointer(arg4), getAsPointer(arg5));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (pointer, int, string, int, pointer) -> int (SQLite bind_text)
        if (std::holds_alternative<void*>(arg1) &&
            (std::holds_alternative<double>(arg2) || std::holds_alternative<int>(arg2)) &&
            std::holds_alternative<std::string>(arg3) &&
            (std::holds_alternative<double>(arg4) || std::holds_alternative<int>(arg4)) &&
            std::holds_alternative<void*>(arg5)) {
            
            typedef int (__cdecl *FuncType)(void*, int, const char*, int, void*);
            auto func = reinterpret_cast<FuncType>(funcPtr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsString(arg3), getAsInt(arg4), getAsPointer(arg5));
            return BasicValue(static_cast<double>(result));
        }
        
        // Fallback: (int, int, int, int, int) -> int (all integers) - Windows API
        typedef int (__stdcall *FuncInt5)(int, int, int, int, int);
        auto func_int = reinterpret_cast<FuncInt5>(funcPtr);
        return BasicValue(static_cast<double>(func_int(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), getAsInt(arg4), getAsInt(arg5))));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

// 6-parameter FFI function calls
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* func_ptr = library->get_function_address(function_name);
        if (!func_ptr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic parameter conversion for 6-parameter functions
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsDouble = [](const BasicValue& val) -> double {
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            } else if (std::holds_alternative<int>(val)) {
                return static_cast<double>(std::get<int>(val));
            }
            return 0.0;
        };
        
        // Store strings to ensure pointer stability
        std::string str1, str2, str3, str4, str5, str6;
        auto getAsString = [&](const BasicValue& val, std::string& storage) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                storage = std::get<std::string>(val);
                return storage.c_str();
            }
            return nullptr;
        };
        
        // Pattern: (string, int, int, int, int, int) -> pointer (SDL_CreateWindow)
        if (std::holds_alternative<std::string>(arg1)) {
            typedef void* (__cdecl *FuncType)(const char*, int, int, int, int, int);
            auto func = reinterpret_cast<FuncType>(func_ptr);
            void* result = func(getAsString(arg1, str1), getAsInt(arg2), getAsInt(arg3), 
                               getAsInt(arg4), getAsInt(arg5), getAsInt(arg6));
            return BasicValue(result);
        }
        
        // Pattern: (pointer, int, int, int, int, int) -> int
        if (std::holds_alternative<void*>(arg1)) {
            typedef int (__cdecl *FuncType)(void*, int, int, int, int, int);
            auto func = reinterpret_cast<FuncType>(func_ptr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), 
                             getAsInt(arg4), getAsInt(arg5), getAsInt(arg6));
            return BasicValue(static_cast<double>(result));
        }
        
        // Pattern: (int, int, int, int, int, int) -> int
        typedef int (__stdcall *FuncType)(int, int, int, int, int, int);
        auto func = reinterpret_cast<FuncType>(func_ptr);
        int result = func(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), 
                         getAsInt(arg4), getAsInt(arg5), getAsInt(arg6));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

// 7-parameter FFI function calls
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6,
                           const BasicValue& arg7) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* func_ptr = library->get_function_address(function_name);
        if (!func_ptr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic parameter conversion for 7-parameter functions
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val).c_str();
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        // Pattern: (int, int, int, int, int, int, int) -> int
        typedef int (__stdcall *FuncType)(int, int, int, int, int, int, int);
        auto func = reinterpret_cast<FuncType>(func_ptr);
        int result = func(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), 
                         getAsInt(arg4), getAsInt(arg5), getAsInt(arg6), getAsInt(arg7));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

// 8-parameter FFI function calls
BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6,
                           const BasicValue& arg7, const BasicValue& arg8) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* func_ptr = library->get_function_address(function_name);
        if (!func_ptr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        // Generic parameter conversion for 8-parameter functions
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        auto getAsString = [](const BasicValue& val) -> const char* {
            if (std::holds_alternative<std::string>(val)) {
                return std::get<std::string>(val).c_str();
            }
            return nullptr;
        };
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsDouble = [](const BasicValue& val) -> double {
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val);
            } else if (std::holds_alternative<int>(val)) {
                return static_cast<double>(std::get<int>(val));
            }
            return 0.0;
        };
        
        // Check for SDL_RenderCopyEx pattern: (pointer, pointer, pointer, pointer, double, pointer, double, int) -> int
        if (std::holds_alternative<void*>(arg1) && std::holds_alternative<double>(arg5) && std::holds_alternative<double>(arg7)) {
            typedef int (__cdecl *FuncSDLRenderCopyEx)(void*, void*, void*, void*, double, void*, double, int);
            auto func = reinterpret_cast<FuncSDLRenderCopyEx>(func_ptr);
            int result = func(getAsPointer(arg1), getAsPointer(arg2), getAsPointer(arg3), 
                             getAsPointer(arg4), getAsDouble(arg5), getAsPointer(arg6), 
                             getAsDouble(arg7), getAsInt(arg8));
            return BasicValue(static_cast<double>(result));
        } else {
            // Generic pattern: (int, int, int, int, int, int, int, int) -> int - Windows API
            typedef int (__stdcall *FuncType)(int, int, int, int, int, int, int, int);
            auto func = reinterpret_cast<FuncType>(func_ptr);
            int result = func(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), 
                             getAsInt(arg4), getAsInt(arg5), getAsInt(arg6), 
                             getAsInt(arg7), getAsInt(arg8));
            return BasicValue(static_cast<double>(result));
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6,
                           const BasicValue& arg7, const BasicValue& arg8, const BasicValue& arg9) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* func_ptr = library->get_function_address(function_name);
        if (!func_ptr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        // Generic pattern: (int, int, int, int, int, int, int, int, int) -> int
        typedef int (__stdcall *FuncType)(int, int, int, int, int, int, int, int, int);
        auto func = reinterpret_cast<FuncType>(func_ptr);
        int result = func(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), 
                         getAsInt(arg4), getAsInt(arg5), getAsInt(arg6), 
                         getAsInt(arg7), getAsInt(arg8), getAsInt(arg9));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6,
                           const BasicValue& arg7, const BasicValue& arg8, const BasicValue& arg9,
                           const BasicValue& arg10) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* func_ptr = library->get_function_address(function_name);
        if (!func_ptr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        // Generic pattern: (int, int, int, int, int, int, int, int, int, int) -> int
        typedef int (__stdcall *FuncType)(int, int, int, int, int, int, int, int, int, int);
        auto func = reinterpret_cast<FuncType>(func_ptr);
        int result = func(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), 
                         getAsInt(arg4), getAsInt(arg5), getAsInt(arg6), 
                         getAsInt(arg7), getAsInt(arg8), getAsInt(arg9), 
                         getAsInt(arg10));
        return BasicValue(static_cast<double>(result));
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

BasicValue call_ffi_function(const std::string& library_name, const std::string& function_name, 
                           const BasicValue& arg1, const BasicValue& arg2, const BasicValue& arg3,
                           const BasicValue& arg4, const BasicValue& arg5, const BasicValue& arg6,
                           const BasicValue& arg7, const BasicValue& arg8, const BasicValue& arg9,
                           const BasicValue& arg10, const BasicValue& arg11) {
    try {
        auto& ffi_manager = rbasic::ffi::FFIManager::instance();
        auto library = ffi_manager.get_library(library_name);
        if (!library) {
            library = ffi_manager.load_library(library_name);
        }
        
        if (!library || !library->is_valid()) {
            throw std::runtime_error("Failed to load library: " + library_name);
        }
        
        void* func_ptr = library->get_function_address(function_name);
        if (!func_ptr) {
            throw std::runtime_error("Function not found: " + function_name);
        }
        
        auto getAsInt = [](const BasicValue& val) -> int {
            if (std::holds_alternative<double>(val)) {
                return static_cast<int>(std::get<double>(val));
            } else if (std::holds_alternative<int>(val)) {
                return std::get<int>(val);
            }
            return 0;
        };
        
        auto getAsPointer = [](const BasicValue& val) -> void* {
            if (std::holds_alternative<void*>(val)) {
                return std::get<void*>(val);
            }
            return nullptr;
        };
        
        // Pattern for filledTrigonRGBA: (pointer, int, int, int, int, int, int, int, int, int, int) -> int
        if (std::holds_alternative<void*>(arg1)) {
            typedef int (__cdecl *FuncSDL2GFX)(void*, int, int, int, int, int, int, int, int, int, int);
            auto func = reinterpret_cast<FuncSDL2GFX>(func_ptr);
            int result = func(getAsPointer(arg1), getAsInt(arg2), getAsInt(arg3), 
                             getAsInt(arg4), getAsInt(arg5), getAsInt(arg6), 
                             getAsInt(arg7), getAsInt(arg8), getAsInt(arg9), 
                             getAsInt(arg10), getAsInt(arg11));
            return BasicValue(static_cast<double>(result));
        } else {
            // Generic pattern: (int, int, int, int, int, int, int, int, int, int, int) -> int
            typedef int (__stdcall *FuncType)(int, int, int, int, int, int, int, int, int, int, int);
            auto func = reinterpret_cast<FuncType>(func_ptr);
            int result = func(getAsInt(arg1), getAsInt(arg2), getAsInt(arg3), 
                             getAsInt(arg4), getAsInt(arg5), getAsInt(arg6), 
                             getAsInt(arg7), getAsInt(arg8), getAsInt(arg9), 
                             getAsInt(arg10), getAsInt(arg11));
            return BasicValue(static_cast<double>(result));
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error("FFI call failed: " + std::string(e.what()));
    }
}

// ===== BUFFER ALLOCATION AND OUTPUT PARAMETER FUNCTIONS =====

BasicValue alloc_int_buffer() {
    // Allocate memory for a single int
    int* buffer = new int(0);  // Initialize to 0
    return BasicValue(static_cast<void*>(buffer));
}

BasicValue alloc_pointer_buffer() {
    // Allocate memory for a single void* pointer
    void** buffer = new void*(nullptr);  // Initialize to nullptr
    return BasicValue(static_cast<void*>(buffer));
}

BasicValue alloc_buffer(int size) {
    // Allocate a byte buffer of specified size
    if (size <= 0) {
        throw std::runtime_error("Buffer size must be positive");
    }
    
    uint8_t* buffer = new uint8_t[size]();  // Zero-initialize
    return BasicValue(static_cast<void*>(buffer));
}

BasicValue deref_int_offset(const BasicValue& ptr, const BasicValue& offset) {
    // Dereference int* at a specific byte offset
    if (!std::holds_alternative<void*>(ptr)) {
        throw std::runtime_error("deref_int_offset requires a pointer");
    }
    if (!std::holds_alternative<double>(offset)) {
        throw std::runtime_error("deref_int_offset requires numeric offset");
    }
    
    void* raw_ptr = std::get<void*>(ptr);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot dereference null pointer");
    }
    
    int byte_offset = static_cast<int>(std::get<double>(offset));
    uint8_t* base_ptr = static_cast<uint8_t*>(raw_ptr);
    uint32_t* target_ptr = reinterpret_cast<uint32_t*>(base_ptr + byte_offset);
    
    return BasicValue(static_cast<double>(*target_ptr));
}

BasicValue deref_int(const BasicValue& ptr) {
    // Dereference int* to get int value
    if (!std::holds_alternative<void*>(ptr)) {
        throw std::runtime_error("deref_int requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(ptr);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot dereference null pointer");
    }
    
    int* int_ptr = static_cast<int*>(raw_ptr);
    return BasicValue(static_cast<double>(*int_ptr));
}

BasicValue deref_pointer(const BasicValue& ptr) {
    // Dereference void** to get void* value
    if (!std::holds_alternative<void*>(ptr)) {
        throw std::runtime_error("deref_pointer requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(ptr);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot dereference null pointer");
    }
    
    void** ptr_ptr = static_cast<void**>(raw_ptr);
    return BasicValue(*ptr_ptr);
}

BasicValue deref_string(const BasicValue& ptr) {
    // Dereference char* to get string value
    if (!std::holds_alternative<void*>(ptr) && !std::holds_alternative<std::string>(ptr)) {
        throw std::runtime_error("deref_string requires a pointer");
    }
    
    if (std::holds_alternative<std::string>(ptr)) {
        return ptr;  // Already a string
    }
    
    void* raw_ptr = std::get<void*>(ptr);
    if (!raw_ptr) {
        return BasicValue(std::string(""));  // Return empty string for null
    }
    
    const char* str_ptr = static_cast<const char*>(raw_ptr);
    return BasicValue(std::string(str_ptr));
}

void set_int_buffer(const BasicValue& ptr, int value) {
    // Set value in int* buffer
    if (!std::holds_alternative<void*>(ptr)) {
        throw std::runtime_error("set_int_buffer requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(ptr);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot write to null pointer");
    }
    
    int* int_ptr = static_cast<int*>(raw_ptr);
    *int_ptr = value;
}

void set_pointer_buffer(const BasicValue& ptr, const BasicValue& value) {
    // Set value in void** buffer
    if (!std::holds_alternative<void*>(ptr)) {
        throw std::runtime_error("set_pointer_buffer requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(ptr);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot write to null pointer");
    }
    
    void** ptr_ptr = static_cast<void**>(raw_ptr);
    
    if (std::holds_alternative<void*>(value)) {
        *ptr_ptr = std::get<void*>(value);
    } else {
        throw std::runtime_error("set_pointer_buffer value must be a pointer");
    }
}

// ===== SDL STRUCT HELPERS =====

BasicValue create_sdl_rect(int x, int y, int w, int h) {
    // Create SDL_Rect structure (4 ints: x, y, w, h)
    struct SDLRect {
        int x, y, w, h;
    };
    
    SDLRect* rect = new SDLRect{x, y, w, h};
    return BasicValue(static_cast<void*>(rect));
}

BasicValue create_sdl_event() {
    // Create SDL_Event buffer (56 bytes on most platforms)
    constexpr size_t SDL_EVENT_SIZE = 56;
    uint8_t* event_buffer = new uint8_t[SDL_EVENT_SIZE]();  // Zero-initialize
    return BasicValue(static_cast<void*>(event_buffer));
}

BasicValue get_event_type(const BasicValue& event) {
    // Get event.type from SDL_Event (first 4 bytes as uint32)
    if (!std::holds_alternative<void*>(event)) {
        throw std::runtime_error("get_event_type requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(event);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot read from null event pointer");
    }
    
    uint32_t* type_ptr = static_cast<uint32_t*>(raw_ptr);
    return BasicValue(static_cast<double>(*type_ptr));
}

BasicValue get_key_code(const BasicValue& event) {
    // Get key code from SDL_Event
    // SDL_Event structure: type(4) + timestamp(4) + key.state(1) + repeat(1) + padding(2) + keysym.scancode(4) + keysym.sym(4)
    // keysym.sym is at offset 16 from start
    if (!std::holds_alternative<void*>(event)) {
        throw std::runtime_error("get_key_code requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(event);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot read from null event pointer");
    }
    
    uint8_t* event_bytes = static_cast<uint8_t*>(raw_ptr);
    uint32_t* key_sym = reinterpret_cast<uint32_t*>(event_bytes + 16);
    return BasicValue(static_cast<double>(*key_sym));
}

BasicValue get_rect_field(const BasicValue& rect, const std::string& field) {
    // Get x, y, w, h from SDL_Rect
    if (!std::holds_alternative<void*>(rect)) {
        throw std::runtime_error("get_rect_field requires a pointer");
    }
    
    void* raw_ptr = std::get<void*>(rect);
    if (!raw_ptr) {
        throw std::runtime_error("Cannot read from null rect pointer");
    }
    
    struct SDLRect {
        int x, y, w, h;
    };
    
    SDLRect* rect_ptr = static_cast<SDLRect*>(raw_ptr);
    
    if (field == "x") {
        return BasicValue(static_cast<double>(rect_ptr->x));
    } else if (field == "y") {
        return BasicValue(static_cast<double>(rect_ptr->y));
    } else if (field == "w") {
        return BasicValue(static_cast<double>(rect_ptr->w));
    } else if (field == "h") {
        return BasicValue(static_cast<double>(rect_ptr->h));
    } else {
        throw std::runtime_error("Invalid rect field: " + field + " (valid: x, y, w, h)");
    }
}

// ===== WRAPPER FUNCTIONS FOR CODE GENERATOR =====

BasicValue func_alloc_int_buffer() {
    return alloc_int_buffer();
}

BasicValue func_alloc_pointer_buffer() {
    return alloc_pointer_buffer();
}

BasicValue func_alloc_buffer(const BasicValue& size) {
    int sizeInt = to_int(size);
    return alloc_buffer(sizeInt);
}

BasicValue func_deref_int(const BasicValue& ptr) {
    return deref_int(ptr);
}

BasicValue func_deref_pointer(const BasicValue& ptr) {
    return deref_pointer(ptr);
}

BasicValue func_deref_string(const BasicValue& ptr) {
    return deref_string(ptr);
}

BasicValue func_create_sdl_rect(const BasicValue& x, const BasicValue& y, const BasicValue& w, const BasicValue& h) {
    int xInt = to_int(x);
    int yInt = to_int(y);
    int wInt = to_int(w);
    int hInt = to_int(h);
    return create_sdl_rect(xInt, yInt, wInt, hInt);
}

BasicValue func_create_sdl_event() {
    return create_sdl_event();
}

BasicValue func_get_event_type(const BasicValue& event) {
    return get_event_type(event);
}

BasicValue func_get_key_code(const BasicValue& event) {
    return get_key_code(event);
}

BasicValue func_get_rect_field(const BasicValue& rect, const BasicValue& field) {
    std::string fieldStr = to_string(field);
    return get_rect_field(rect, fieldStr);
}

// ===== CONSTANT/NULL HANDLING SYSTEM =====

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
    
    // SDL2 constants
    if (name == "SDL_INIT_VIDEO") return BasicValue(static_cast<double>(0x00000020));
    if (name == "SDL_INIT_AUDIO") return BasicValue(static_cast<double>(0x00000010));
    if (name == "SDL_INIT_TIMER") return BasicValue(static_cast<double>(0x00000001));
    if (name == "SDL_INIT_JOYSTICK") return BasicValue(static_cast<double>(0x00000200));
    if (name == "SDL_INIT_HAPTIC") return BasicValue(static_cast<double>(0x00001000));
    if (name == "SDL_INIT_GAMECONTROLLER") return BasicValue(static_cast<double>(0x00002000));
    if (name == "SDL_INIT_EVENTS") return BasicValue(static_cast<double>(0x00004000));
    if (name == "SDL_INIT_EVERYTHING") return BasicValue(static_cast<double>(0x0000FFFF));
    
    // SDL Window flags
    if (name == "SDL_WINDOW_FULLSCREEN") return BasicValue(static_cast<double>(0x00000001));
    if (name == "SDL_WINDOW_OPENGL") return BasicValue(static_cast<double>(0x00000002));
    if (name == "SDL_WINDOW_SHOWN") return BasicValue(static_cast<double>(0x00000004));
    if (name == "SDL_WINDOW_HIDDEN") return BasicValue(static_cast<double>(0x00000008));
    if (name == "SDL_WINDOW_BORDERLESS") return BasicValue(static_cast<double>(0x00000010));
    if (name == "SDL_WINDOW_RESIZABLE") return BasicValue(static_cast<double>(0x00000020));
    if (name == "SDL_WINDOW_MINIMIZED") return BasicValue(static_cast<double>(0x00000040));
    if (name == "SDL_WINDOW_MAXIMIZED") return BasicValue(static_cast<double>(0x00000080));
    if (name == "SDL_WINDOW_FULLSCREEN_DESKTOP") return BasicValue(static_cast<double>(0x00001001));
    if (name == "SDL_WINDOW_FOREIGN") return BasicValue(static_cast<double>(0x00000800));
    if (name == "SDL_WINDOW_ALLOW_HIGHDPI") return BasicValue(static_cast<double>(0x00002000));
    if (name == "SDL_WINDOW_MOUSE_CAPTURE") return BasicValue(static_cast<double>(0x00004000));
    if (name == "SDL_WINDOW_ALWAYS_ON_TOP") return BasicValue(static_cast<double>(0x00008000));
    if (name == "SDL_WINDOW_SKIP_TASKBAR") return BasicValue(static_cast<double>(0x00010000));
    if (name == "SDL_WINDOW_UTILITY") return BasicValue(static_cast<double>(0x00020000));
    if (name == "SDL_WINDOW_TOOLTIP") return BasicValue(static_cast<double>(0x00040000));
    if (name == "SDL_WINDOW_POPUP_MENU") return BasicValue(static_cast<double>(0x00080000));
    
    if (name == "SDL_WINDOWPOS_UNDEFINED") return BasicValue(static_cast<double>(0x1FFF0000));
    if (name == "SDL_WINDOWPOS_CENTERED") return BasicValue(static_cast<double>(0x2FFF0000));
    
    // SDL Renderer flags
    if (name == "SDL_RENDERER_SOFTWARE") return BasicValue(static_cast<double>(0x00000001));
    if (name == "SDL_RENDERER_ACCELERATED") return BasicValue(static_cast<double>(0x00000002));
    if (name == "SDL_RENDERER_PRESENTVSYNC") return BasicValue(static_cast<double>(0x00000004));
    if (name == "SDL_RENDERER_TARGETTEXTURE") return BasicValue(static_cast<double>(0x00000008));
    
    // SDL Event types
    if (name == "SDL_FIRSTEVENT") return BasicValue(static_cast<double>(0));
    if (name == "SDL_QUIT") return BasicValue(static_cast<double>(0x100));
    if (name == "SDL_APP_TERMINATING") return BasicValue(static_cast<double>(0x101));
    if (name == "SDL_APP_LOWMEMORY") return BasicValue(static_cast<double>(0x102));
    if (name == "SDL_APP_WILLENTERBACKGROUND") return BasicValue(static_cast<double>(0x103));
    if (name == "SDL_APP_DIDENTERBACKGROUND") return BasicValue(static_cast<double>(0x104));
    if (name == "SDL_APP_WILLENTERFOREGROUND") return BasicValue(static_cast<double>(0x105));
    if (name == "SDL_APP_DIDENTERFOREGROUND") return BasicValue(static_cast<double>(0x106));
    
    // Window events
    if (name == "SDL_WINDOWEVENT") return BasicValue(static_cast<double>(0x200));
    if (name == "SDL_SYSWMEVENT") return BasicValue(static_cast<double>(0x201));
    
    // Keyboard events
    if (name == "SDL_KEYDOWN") return BasicValue(static_cast<double>(0x300));
    if (name == "SDL_KEYUP") return BasicValue(static_cast<double>(0x301));
    if (name == "SDL_TEXTEDITING") return BasicValue(static_cast<double>(0x302));
    if (name == "SDL_TEXTINPUT") return BasicValue(static_cast<double>(0x303));
    
    // Mouse events
    if (name == "SDL_MOUSEBUTTONDOWN") return BasicValue(static_cast<double>(0x401));
    if (name == "SDL_MOUSEBUTTONUP") return BasicValue(static_cast<double>(0x402));
    if (name == "SDL_MOUSEMOTION") return BasicValue(static_cast<double>(0x400));
    if (name == "SDL_MOUSEWHEEL") return BasicValue(static_cast<double>(0x403));
    
    // Joystick events
    if (name == "SDL_JOYAXISMOTION") return BasicValue(static_cast<double>(0x600));
    if (name == "SDL_JOYBALLMOTION") return BasicValue(static_cast<double>(0x601));
    if (name == "SDL_JOYHATMOTION") return BasicValue(static_cast<double>(0x602));
    if (name == "SDL_JOYBUTTONDOWN") return BasicValue(static_cast<double>(0x603));
    if (name == "SDL_JOYBUTTONUP") return BasicValue(static_cast<double>(0x604));
    if (name == "SDL_JOYDEVICEADDED") return BasicValue(static_cast<double>(0x605));
    if (name == "SDL_JOYDEVICEREMOVED") return BasicValue(static_cast<double>(0x606));
    
    // Game controller events
    if (name == "SDL_CONTROLLERAXISMOTION") return BasicValue(static_cast<double>(0x650));
    if (name == "SDL_CONTROLLERBUTTONDOWN") return BasicValue(static_cast<double>(0x651));
    if (name == "SDL_CONTROLLERBUTTONUP") return BasicValue(static_cast<double>(0x652));
    if (name == "SDL_CONTROLLERDEVICEADDED") return BasicValue(static_cast<double>(0x653));
    if (name == "SDL_CONTROLLERDEVICEREMOVED") return BasicValue(static_cast<double>(0x654));
    if (name == "SDL_CONTROLLERDEVICEREMAPPED") return BasicValue(static_cast<double>(0x655));
    
    // Touch events
    if (name == "SDL_FINGERDOWN") return BasicValue(static_cast<double>(0x700));
    if (name == "SDL_FINGERUP") return BasicValue(static_cast<double>(0x701));
    if (name == "SDL_FINGERMOTION") return BasicValue(static_cast<double>(0x702));
    
    // Drop events
    if (name == "SDL_DROPFILE") return BasicValue(static_cast<double>(0x1000));
    if (name == "SDL_DROPTEXT") return BasicValue(static_cast<double>(0x1001));
    if (name == "SDL_DROPBEGIN") return BasicValue(static_cast<double>(0x1002));
    if (name == "SDL_DROPCOMPLETE") return BasicValue(static_cast<double>(0x1003));
    
    // Audio events
    if (name == "SDL_AUDIODEVICEADDED") return BasicValue(static_cast<double>(0x1100));
    if (name == "SDL_AUDIODEVICEREMOVED") return BasicValue(static_cast<double>(0x1101));
    
    // User events
    if (name == "SDL_USEREVENT") return BasicValue(static_cast<double>(0x8000));
    if (name == "SDL_LASTEVENT") return BasicValue(static_cast<double>(0xFFFF));
    
    // SDL Mouse button constants
    if (name == "SDL_BUTTON_LEFT") return BasicValue(static_cast<double>(1));
    if (name == "SDL_BUTTON_MIDDLE") return BasicValue(static_cast<double>(2));
    if (name == "SDL_BUTTON_RIGHT") return BasicValue(static_cast<double>(3));
    if (name == "SDL_BUTTON_X1") return BasicValue(static_cast<double>(4));
    if (name == "SDL_BUTTON_X2") return BasicValue(static_cast<double>(5));
    
    // SDL Mouse button masks (for SDL_GetMouseState) - SDL_BUTTON(X) = (1 << ((X)-1))
    if (name == "SDL_BUTTON_LMASK") return BasicValue(static_cast<double>(1));     // 1 << (1-1) = 1 << 0 = 1
    if (name == "SDL_BUTTON_MMASK") return BasicValue(static_cast<double>(2));     // 1 << (2-1) = 1 << 1 = 2  
    if (name == "SDL_BUTTON_RMASK") return BasicValue(static_cast<double>(4));     // 1 << (3-1) = 1 << 2 = 4
    if (name == "SDL_BUTTON_X1MASK") return BasicValue(static_cast<double>(8));    // 1 << (4-1) = 1 << 3 = 8
    if (name == "SDL_BUTTON_X2MASK") return BasicValue(static_cast<double>(16));   // 1 << (5-1) = 1 << 4 = 16
    
    // SDL Texture constants
    if (name == "SDL_PIXELFORMAT_UNKNOWN") return BasicValue(static_cast<double>(0));
    if (name == "SDL_PIXELFORMAT_INDEX1LSB") return BasicValue(static_cast<double>(286261504));
    if (name == "SDL_PIXELFORMAT_INDEX1MSB") return BasicValue(static_cast<double>(287310080));
    if (name == "SDL_PIXELFORMAT_INDEX4LSB") return BasicValue(static_cast<double>(303039488));
    if (name == "SDL_PIXELFORMAT_INDEX4MSB") return BasicValue(static_cast<double>(304088064));
    if (name == "SDL_PIXELFORMAT_INDEX8") return BasicValue(static_cast<double>(318769153));
    if (name == "SDL_PIXELFORMAT_RGB332") return BasicValue(static_cast<double>(336660481));
    if (name == "SDL_PIXELFORMAT_RGB444") return BasicValue(static_cast<double>(353504258));
    if (name == "SDL_PIXELFORMAT_RGB555") return BasicValue(static_cast<double>(353570562));
    if (name == "SDL_PIXELFORMAT_BGR555") return BasicValue(static_cast<double>(357764866));
    if (name == "SDL_PIXELFORMAT_ARGB4444") return BasicValue(static_cast<double>(355602434));
    if (name == "SDL_PIXELFORMAT_RGBA4444") return BasicValue(static_cast<double>(356651010));
    if (name == "SDL_PIXELFORMAT_ABGR4444") return BasicValue(static_cast<double>(359796738));
    if (name == "SDL_PIXELFORMAT_BGRA4444") return BasicValue(static_cast<double>(360845314));
    if (name == "SDL_PIXELFORMAT_ARGB1555") return BasicValue(static_cast<double>(355667970));
    if (name == "SDL_PIXELFORMAT_RGBA5551") return BasicValue(static_cast<double>(356782082));
    if (name == "SDL_PIXELFORMAT_ABGR1555") return BasicValue(static_cast<double>(359862274));
    if (name == "SDL_PIXELFORMAT_BGRA5551") return BasicValue(static_cast<double>(360976386));
    if (name == "SDL_PIXELFORMAT_RGB565") return BasicValue(static_cast<double>(353701890));
    if (name == "SDL_PIXELFORMAT_BGR565") return BasicValue(static_cast<double>(357896194));
    if (name == "SDL_PIXELFORMAT_RGB24") return BasicValue(static_cast<double>(386930691));
    if (name == "SDL_PIXELFORMAT_BGR24") return BasicValue(static_cast<double>(390076419));
    if (name == "SDL_PIXELFORMAT_RGB888") return BasicValue(static_cast<double>(370546692));    // 0x16161804
    if (name == "SDL_PIXELFORMAT_RGBX8888") return BasicValue(static_cast<double>(371595268));
    if (name == "SDL_PIXELFORMAT_BGR888") return BasicValue(static_cast<double>(374740996));
    if (name == "SDL_PIXELFORMAT_BGRX8888") return BasicValue(static_cast<double>(375789572));
    if (name == "SDL_PIXELFORMAT_ARGB8888") return BasicValue(static_cast<double>(372645892));  // 0x16362004
    if (name == "SDL_PIXELFORMAT_RGBA8888") return BasicValue(static_cast<double>(373694468));  // 0x16462004
    if (name == "SDL_PIXELFORMAT_ABGR8888") return BasicValue(static_cast<double>(376840196));  // 0x16762004
    if (name == "SDL_PIXELFORMAT_BGRA8888") return BasicValue(static_cast<double>(377888772));
    if (name == "SDL_PIXELFORMAT_ARGB2101010") return BasicValue(static_cast<double>(372711428));
    
    if (name == "SDL_TEXTUREACCESS_STATIC") return BasicValue(static_cast<double>(0));          // Static texture
    if (name == "SDL_TEXTUREACCESS_STREAMING") return BasicValue(static_cast<double>(1));       // Streaming texture
    if (name == "SDL_TEXTUREACCESS_TARGET") return BasicValue(static_cast<double>(2));          // Render target texture
    
    // SDL Blend modes
    if (name == "SDL_BLENDMODE_NONE") return BasicValue(static_cast<double>(0x00000000));
    if (name == "SDL_BLENDMODE_BLEND") return BasicValue(static_cast<double>(0x00000001));
    if (name == "SDL_BLENDMODE_ADD") return BasicValue(static_cast<double>(0x00000002));
    if (name == "SDL_BLENDMODE_MOD") return BasicValue(static_cast<double>(0x00000004));
    if (name == "SDL_BLENDMODE_MUL") return BasicValue(static_cast<double>(0x00000008));
    if (name == "SDL_BLENDMODE_INVALID") return BasicValue(static_cast<double>(0x7FFFFFFF));
    
    // SDL Key codes (these are the SDLK_* values - different from scan codes!)
    if (name == "SDLK_ESCAPE") return BasicValue(static_cast<double>(27));
    if (name == "SDLK_SPACE") return BasicValue(static_cast<double>(32));
    if (name == "SDLK_RETURN") return BasicValue(static_cast<double>(13));
    if (name == "SDLK_UP") return BasicValue(static_cast<double>(1073741906));
    if (name == "SDLK_DOWN") return BasicValue(static_cast<double>(1073741905));
    if (name == "SDLK_LEFT") return BasicValue(static_cast<double>(1073741904));
    if (name == "SDLK_RIGHT") return BasicValue(static_cast<double>(1073741903));
    
    // SDL Scan codes (these are the SDL_SCANCODE_* values used for keyboard input)
    if (name == "SDL_SCANCODE_ESCAPE") return BasicValue(static_cast<double>(41));
    if (name == "SDL_SCANCODE_SPACE") return BasicValue(static_cast<double>(44));
    if (name == "SDL_SCANCODE_RETURN") return BasicValue(static_cast<double>(40));
    if (name == "SDL_SCANCODE_UP") return BasicValue(static_cast<double>(82));
    if (name == "SDL_SCANCODE_DOWN") return BasicValue(static_cast<double>(81));
    if (name == "SDL_SCANCODE_LEFT") return BasicValue(static_cast<double>(80));
    if (name == "SDL_SCANCODE_RIGHT") return BasicValue(static_cast<double>(79));
    if (name == "SDL_SCANCODE_W") return BasicValue(static_cast<double>(26));
    if (name == "SDL_SCANCODE_A") return BasicValue(static_cast<double>(4));
    if (name == "SDL_SCANCODE_S") return BasicValue(static_cast<double>(22));
    if (name == "SDL_SCANCODE_D") return BasicValue(static_cast<double>(7));
    
    // SDL Scan codes (these are what get_key_code() actually returns!)
    if (name == "SDL_SCANCODE_A") return BasicValue(static_cast<double>(4));
    if (name == "SDL_SCANCODE_B") return BasicValue(static_cast<double>(5));
    if (name == "SDL_SCANCODE_C") return BasicValue(static_cast<double>(6));
    if (name == "SDL_SCANCODE_D") return BasicValue(static_cast<double>(7));
    if (name == "SDL_SCANCODE_E") return BasicValue(static_cast<double>(8));
    if (name == "SDL_SCANCODE_F") return BasicValue(static_cast<double>(9));
    if (name == "SDL_SCANCODE_G") return BasicValue(static_cast<double>(10));
    if (name == "SDL_SCANCODE_H") return BasicValue(static_cast<double>(11));
    if (name == "SDL_SCANCODE_I") return BasicValue(static_cast<double>(12));
    if (name == "SDL_SCANCODE_J") return BasicValue(static_cast<double>(13));
    if (name == "SDL_SCANCODE_K") return BasicValue(static_cast<double>(14));
    if (name == "SDL_SCANCODE_L") return BasicValue(static_cast<double>(15));
    if (name == "SDL_SCANCODE_M") return BasicValue(static_cast<double>(16));
    if (name == "SDL_SCANCODE_N") return BasicValue(static_cast<double>(17));
    if (name == "SDL_SCANCODE_O") return BasicValue(static_cast<double>(18));
    if (name == "SDL_SCANCODE_P") return BasicValue(static_cast<double>(19));
    if (name == "SDL_SCANCODE_Q") return BasicValue(static_cast<double>(20));
    if (name == "SDL_SCANCODE_R") return BasicValue(static_cast<double>(21));
    if (name == "SDL_SCANCODE_S") return BasicValue(static_cast<double>(22));
    if (name == "SDL_SCANCODE_T") return BasicValue(static_cast<double>(23));
    if (name == "SDL_SCANCODE_U") return BasicValue(static_cast<double>(24));
    if (name == "SDL_SCANCODE_V") return BasicValue(static_cast<double>(25));
    if (name == "SDL_SCANCODE_W") return BasicValue(static_cast<double>(26));
    if (name == "SDL_SCANCODE_X") return BasicValue(static_cast<double>(27));
    if (name == "SDL_SCANCODE_Y") return BasicValue(static_cast<double>(28));
    if (name == "SDL_SCANCODE_Z") return BasicValue(static_cast<double>(29));
    
    if (name == "SDL_SCANCODE_1") return BasicValue(static_cast<double>(30));
    if (name == "SDL_SCANCODE_2") return BasicValue(static_cast<double>(31));
    if (name == "SDL_SCANCODE_3") return BasicValue(static_cast<double>(32));
    if (name == "SDL_SCANCODE_4") return BasicValue(static_cast<double>(33));
    if (name == "SDL_SCANCODE_5") return BasicValue(static_cast<double>(34));
    if (name == "SDL_SCANCODE_6") return BasicValue(static_cast<double>(35));
    if (name == "SDL_SCANCODE_7") return BasicValue(static_cast<double>(36));
    if (name == "SDL_SCANCODE_8") return BasicValue(static_cast<double>(37));
    if (name == "SDL_SCANCODE_9") return BasicValue(static_cast<double>(38));
    if (name == "SDL_SCANCODE_0") return BasicValue(static_cast<double>(39));
    
    if (name == "SDL_SCANCODE_RETURN") return BasicValue(static_cast<double>(40));
    if (name == "SDL_SCANCODE_ESCAPE") return BasicValue(static_cast<double>(41));
    if (name == "SDL_SCANCODE_BACKSPACE") return BasicValue(static_cast<double>(42));
    if (name == "SDL_SCANCODE_TAB") return BasicValue(static_cast<double>(43));
    if (name == "SDL_SCANCODE_SPACE") return BasicValue(static_cast<double>(44));
    
    if (name == "SDL_SCANCODE_MINUS") return BasicValue(static_cast<double>(45));
    if (name == "SDL_SCANCODE_EQUALS") return BasicValue(static_cast<double>(46));
    if (name == "SDL_SCANCODE_LEFTBRACKET") return BasicValue(static_cast<double>(47));
    if (name == "SDL_SCANCODE_RIGHTBRACKET") return BasicValue(static_cast<double>(48));
    if (name == "SDL_SCANCODE_BACKSLASH") return BasicValue(static_cast<double>(49));
    if (name == "SDL_SCANCODE_SEMICOLON") return BasicValue(static_cast<double>(51));
    if (name == "SDL_SCANCODE_APOSTROPHE") return BasicValue(static_cast<double>(52));
    if (name == "SDL_SCANCODE_GRAVE") return BasicValue(static_cast<double>(53));
    if (name == "SDL_SCANCODE_COMMA") return BasicValue(static_cast<double>(54));
    if (name == "SDL_SCANCODE_PERIOD") return BasicValue(static_cast<double>(55));
    if (name == "SDL_SCANCODE_SLASH") return BasicValue(static_cast<double>(56));
    
    if (name == "SDL_SCANCODE_CAPSLOCK") return BasicValue(static_cast<double>(57));
    
    if (name == "SDL_SCANCODE_F1") return BasicValue(static_cast<double>(58));
    if (name == "SDL_SCANCODE_F2") return BasicValue(static_cast<double>(59));
    if (name == "SDL_SCANCODE_F3") return BasicValue(static_cast<double>(60));
    if (name == "SDL_SCANCODE_F4") return BasicValue(static_cast<double>(61));
    if (name == "SDL_SCANCODE_F5") return BasicValue(static_cast<double>(62));
    if (name == "SDL_SCANCODE_F6") return BasicValue(static_cast<double>(63));
    if (name == "SDL_SCANCODE_F7") return BasicValue(static_cast<double>(64));
    if (name == "SDL_SCANCODE_F8") return BasicValue(static_cast<double>(65));
    if (name == "SDL_SCANCODE_F9") return BasicValue(static_cast<double>(66));
    if (name == "SDL_SCANCODE_F10") return BasicValue(static_cast<double>(67));
    if (name == "SDL_SCANCODE_F11") return BasicValue(static_cast<double>(68));
    if (name == "SDL_SCANCODE_F12") return BasicValue(static_cast<double>(69));
    
    if (name == "SDL_SCANCODE_PRINTSCREEN") return BasicValue(static_cast<double>(70));
    if (name == "SDL_SCANCODE_SCROLLLOCK") return BasicValue(static_cast<double>(71));
    if (name == "SDL_SCANCODE_PAUSE") return BasicValue(static_cast<double>(72));
    if (name == "SDL_SCANCODE_INSERT") return BasicValue(static_cast<double>(73));
    if (name == "SDL_SCANCODE_HOME") return BasicValue(static_cast<double>(74));
    if (name == "SDL_SCANCODE_PAGEUP") return BasicValue(static_cast<double>(75));
    if (name == "SDL_SCANCODE_DELETE") return BasicValue(static_cast<double>(76));
    if (name == "SDL_SCANCODE_END") return BasicValue(static_cast<double>(77));
    if (name == "SDL_SCANCODE_PAGEDOWN") return BasicValue(static_cast<double>(78));
    if (name == "SDL_SCANCODE_RIGHT") return BasicValue(static_cast<double>(79));
    if (name == "SDL_SCANCODE_LEFT") return BasicValue(static_cast<double>(80));
    if (name == "SDL_SCANCODE_DOWN") return BasicValue(static_cast<double>(81));
    if (name == "SDL_SCANCODE_UP") return BasicValue(static_cast<double>(82));
    
    if (name == "SDL_SCANCODE_LCTRL") return BasicValue(static_cast<double>(224));
    if (name == "SDL_SCANCODE_LSHIFT") return BasicValue(static_cast<double>(225));
    if (name == "SDL_SCANCODE_LALT") return BasicValue(static_cast<double>(226));
    if (name == "SDL_SCANCODE_LGUI") return BasicValue(static_cast<double>(227));
    if (name == "SDL_SCANCODE_RCTRL") return BasicValue(static_cast<double>(228));
    if (name == "SDL_SCANCODE_RSHIFT") return BasicValue(static_cast<double>(229));
    if (name == "SDL_SCANCODE_RALT") return BasicValue(static_cast<double>(230));
    if (name == "SDL_SCANCODE_RGUI") return BasicValue(static_cast<double>(231));
    
    // SQLite result codes
    if (name == "SQLITE_OK") return BasicValue(static_cast<double>(0));
    if (name == "SQLITE_ERROR") return BasicValue(static_cast<double>(1));
    if (name == "SQLITE_INTERNAL") return BasicValue(static_cast<double>(2));
    if (name == "SQLITE_PERM") return BasicValue(static_cast<double>(3));
    if (name == "SQLITE_ABORT") return BasicValue(static_cast<double>(4));
    if (name == "SQLITE_BUSY") return BasicValue(static_cast<double>(5));
    if (name == "SQLITE_LOCKED") return BasicValue(static_cast<double>(6));
    if (name == "SQLITE_NOMEM") return BasicValue(static_cast<double>(7));
    if (name == "SQLITE_READONLY") return BasicValue(static_cast<double>(8));
    if (name == "SQLITE_INTERRUPT") return BasicValue(static_cast<double>(9));
    if (name == "SQLITE_IOERR") return BasicValue(static_cast<double>(10));
    if (name == "SQLITE_CORRUPT") return BasicValue(static_cast<double>(11));
    if (name == "SQLITE_NOTFOUND") return BasicValue(static_cast<double>(12));
    if (name == "SQLITE_FULL") return BasicValue(static_cast<double>(13));
    if (name == "SQLITE_CANTOPEN") return BasicValue(static_cast<double>(14));
    if (name == "SQLITE_PROTOCOL") return BasicValue(static_cast<double>(15));
    if (name == "SQLITE_EMPTY") return BasicValue(static_cast<double>(16));
    if (name == "SQLITE_SCHEMA") return BasicValue(static_cast<double>(17));
    if (name == "SQLITE_TOOBIG") return BasicValue(static_cast<double>(18));
    if (name == "SQLITE_CONSTRAINT") return BasicValue(static_cast<double>(19));
    if (name == "SQLITE_MISMATCH") return BasicValue(static_cast<double>(20));
    if (name == "SQLITE_MISUSE") return BasicValue(static_cast<double>(21));
    if (name == "SQLITE_NOLFS") return BasicValue(static_cast<double>(22));
    if (name == "SQLITE_AUTH") return BasicValue(static_cast<double>(23));
    if (name == "SQLITE_FORMAT") return BasicValue(static_cast<double>(24));
    if (name == "SQLITE_RANGE") return BasicValue(static_cast<double>(25));
    if (name == "SQLITE_NOTADB") return BasicValue(static_cast<double>(26));
    if (name == "SQLITE_NOTICE") return BasicValue(static_cast<double>(27));
    if (name == "SQLITE_WARNING") return BasicValue(static_cast<double>(28));
    if (name == "SQLITE_ROW") return BasicValue(static_cast<double>(100));
    if (name == "SQLITE_DONE") return BasicValue(static_cast<double>(101));
    
    // SQLite open flags
    if (name == "SQLITE_OPEN_READONLY") return BasicValue(static_cast<double>(0x00000001));
    if (name == "SQLITE_OPEN_READWRITE") return BasicValue(static_cast<double>(0x00000002));
    if (name == "SQLITE_OPEN_CREATE") return BasicValue(static_cast<double>(0x00000004));
    if (name == "SQLITE_OPEN_DELETEONCLOSE") return BasicValue(static_cast<double>(0x00000008));
    if (name == "SQLITE_OPEN_EXCLUSIVE") return BasicValue(static_cast<double>(0x00000010));
    if (name == "SQLITE_OPEN_AUTOPROXY") return BasicValue(static_cast<double>(0x00000020));
    if (name == "SQLITE_OPEN_URI") return BasicValue(static_cast<double>(0x00000040));
    if (name == "SQLITE_OPEN_MEMORY") return BasicValue(static_cast<double>(0x00000080));
    if (name == "SQLITE_OPEN_MAIN_DB") return BasicValue(static_cast<double>(0x00000100));
    if (name == "SQLITE_OPEN_TEMP_DB") return BasicValue(static_cast<double>(0x00000200));
    if (name == "SQLITE_OPEN_TRANSIENT_DB") return BasicValue(static_cast<double>(0x00000400));
    if (name == "SQLITE_OPEN_MAIN_JOURNAL") return BasicValue(static_cast<double>(0x00000800));
    if (name == "SQLITE_OPEN_TEMP_JOURNAL") return BasicValue(static_cast<double>(0x00001000));
    if (name == "SQLITE_OPEN_SUBJOURNAL") return BasicValue(static_cast<double>(0x00002000));
    if (name == "SQLITE_OPEN_MASTER_JOURNAL") return BasicValue(static_cast<double>(0x00004000));
    if (name == "SQLITE_OPEN_NOMUTEX") return BasicValue(static_cast<double>(0x00008000));
    if (name == "SQLITE_OPEN_FULLMUTEX") return BasicValue(static_cast<double>(0x00010000));
    if (name == "SQLITE_OPEN_SHAREDCACHE") return BasicValue(static_cast<double>(0x00020000));
    if (name == "SQLITE_OPEN_PRIVATECACHE") return BasicValue(static_cast<double>(0x00040000));
    if (name == "SQLITE_OPEN_WAL") return BasicValue(static_cast<double>(0x00080000));
    
    // SQLite data types
    if (name == "SQLITE_INTEGER") return BasicValue(static_cast<double>(1));
    if (name == "SQLITE_FLOAT") return BasicValue(static_cast<double>(2));
    if (name == "SQLITE_BLOB") return BasicValue(static_cast<double>(4));
    if (name == "SQLITE_NULL") return BasicValue(static_cast<double>(5));
    if (name == "SQLITE_TEXT") return BasicValue(static_cast<double>(3));
    if (name == "SQLITE3_TEXT") return BasicValue(static_cast<double>(3));
    
    // SQLite transaction types
    if (name == "SQLITE_TXN_NONE") return BasicValue(static_cast<double>(0));
    if (name == "SQLITE_TXN_READ") return BasicValue(static_cast<double>(1));
    if (name == "SQLITE_TXN_WRITE") return BasicValue(static_cast<double>(2));
    
    // SQLite synchronous modes  
    if (name == "SQLITE_SYNC_OFF") return BasicValue(static_cast<double>(0));
    if (name == "SQLITE_SYNC_NORMAL") return BasicValue(static_cast<double>(1));
    if (name == "SQLITE_SYNC_FULL") return BasicValue(static_cast<double>(2));
    if (name == "SQLITE_SYNC_EXTRA") return BasicValue(static_cast<double>(3));
    
    // Windows API constants (useful for MessageBox etc.)
    if (name == "MB_OK") return BasicValue(static_cast<double>(0x00000000));
    if (name == "MB_OKCANCEL") return BasicValue(static_cast<double>(0x00000001));
    if (name == "MB_ABORTRETRYIGNORE") return BasicValue(static_cast<double>(0x00000002));
    if (name == "MB_YESNOCANCEL") return BasicValue(static_cast<double>(0x00000003));
    if (name == "MB_YESNO") return BasicValue(static_cast<double>(0x00000004));
    if (name == "MB_RETRYCANCEL") return BasicValue(static_cast<double>(0x00000005));
    if (name == "MB_CANCELTRYCONTINUE") return BasicValue(static_cast<double>(0x00000006));
    
    if (name == "MB_ICONHAND") return BasicValue(static_cast<double>(0x00000010));
    if (name == "MB_ICONERROR") return BasicValue(static_cast<double>(0x00000010));
    if (name == "MB_ICONSTOP") return BasicValue(static_cast<double>(0x00000010));
    if (name == "MB_ICONQUESTION") return BasicValue(static_cast<double>(0x00000020));
    if (name == "MB_ICONEXCLAMATION") return BasicValue(static_cast<double>(0x00000030));
    if (name == "MB_ICONWARNING") return BasicValue(static_cast<double>(0x00000030));
    if (name == "MB_ICONASTERISK") return BasicValue(static_cast<double>(0x00000040));
    if (name == "MB_ICONINFORMATION") return BasicValue(static_cast<double>(0x00000040));
    
    if (name == "MB_DEFBUTTON1") return BasicValue(static_cast<double>(0x00000000));
    if (name == "MB_DEFBUTTON2") return BasicValue(static_cast<double>(0x00000100));
    if (name == "MB_DEFBUTTON3") return BasicValue(static_cast<double>(0x00000200));
    if (name == "MB_DEFBUTTON4") return BasicValue(static_cast<double>(0x00000300));
    
    if (name == "MB_APPLMODAL") return BasicValue(static_cast<double>(0x00000000));
    if (name == "MB_SYSTEMMODAL") return BasicValue(static_cast<double>(0x00001000));
    if (name == "MB_TASKMODAL") return BasicValue(static_cast<double>(0x00002000));
    
    // MessageBox return values
    if (name == "IDOK") return BasicValue(static_cast<double>(1));
    if (name == "IDCANCEL") return BasicValue(static_cast<double>(2));
    if (name == "IDABORT") return BasicValue(static_cast<double>(3));
    if (name == "IDRETRY") return BasicValue(static_cast<double>(4));
    if (name == "IDIGNORE") return BasicValue(static_cast<double>(5));
    if (name == "IDYES") return BasicValue(static_cast<double>(6));
    if (name == "IDNO") return BasicValue(static_cast<double>(7));
    if (name == "IDCLOSE") return BasicValue(static_cast<double>(8));
    if (name == "IDHELP") return BasicValue(static_cast<double>(9));
    if (name == "IDTRYAGAIN") return BasicValue(static_cast<double>(10));
    if (name == "IDCONTINUE") return BasicValue(static_cast<double>(11));
    
    // File attributes
    if (name == "FILE_ATTRIBUTE_READONLY") return BasicValue(static_cast<double>(0x00000001));
    if (name == "FILE_ATTRIBUTE_HIDDEN") return BasicValue(static_cast<double>(0x00000002));
    if (name == "FILE_ATTRIBUTE_SYSTEM") return BasicValue(static_cast<double>(0x00000004));
    if (name == "FILE_ATTRIBUTE_DIRECTORY") return BasicValue(static_cast<double>(0x00000010));
    if (name == "FILE_ATTRIBUTE_ARCHIVE") return BasicValue(static_cast<double>(0x00000020));
    if (name == "FILE_ATTRIBUTE_DEVICE") return BasicValue(static_cast<double>(0x00000040));
    if (name == "FILE_ATTRIBUTE_NORMAL") return BasicValue(static_cast<double>(0x00000080));
    if (name == "FILE_ATTRIBUTE_TEMPORARY") return BasicValue(static_cast<double>(0x00000100));
    if (name == "FILE_ATTRIBUTE_SPARSE_FILE") return BasicValue(static_cast<double>(0x00000200));
    if (name == "FILE_ATTRIBUTE_COMPRESSED") return BasicValue(static_cast<double>(0x00000800));
    
    // Generic access rights
    if (name == "GENERIC_READ") return BasicValue(static_cast<double>(0x80000000));
    if (name == "GENERIC_WRITE") return BasicValue(static_cast<double>(0x40000000));
    if (name == "GENERIC_EXECUTE") return BasicValue(static_cast<double>(0x20000000));
    if (name == "GENERIC_ALL") return BasicValue(static_cast<double>(0x10000000));
    
    // Standard I/O handles
    if (name == "STD_INPUT_HANDLE") return BasicValue(static_cast<double>(-10));
    if (name == "STD_OUTPUT_HANDLE") return BasicValue(static_cast<double>(-11));
    if (name == "STD_ERROR_HANDLE") return BasicValue(static_cast<double>(-12));
    
    // Common color constants for convenience
    if (name == "COLOR_BLACK") return BasicValue(static_cast<double>(0x000000));
    if (name == "COLOR_WHITE") return BasicValue(static_cast<double>(0xFFFFFF));
    if (name == "COLOR_RED") return BasicValue(static_cast<double>(0xFF0000));
    if (name == "COLOR_GREEN") return BasicValue(static_cast<double>(0x00FF00));
    if (name == "COLOR_BLUE") return BasicValue(static_cast<double>(0x0000FF));
    if (name == "COLOR_YELLOW") return BasicValue(static_cast<double>(0xFFFF00));
    if (name == "COLOR_MAGENTA") return BasicValue(static_cast<double>(0xFF00FF));
    if (name == "COLOR_CYAN") return BasicValue(static_cast<double>(0x00FFFF));
    if (name == "COLOR_GRAY") return BasicValue(static_cast<double>(0x808080));
    if (name == "COLOR_DARKGRAY") return BasicValue(static_cast<double>(0x404040));
    if (name == "COLOR_LIGHTGRAY") return BasicValue(static_cast<double>(0xC0C0C0));
    
    // Math constants
    if (name == "PI") return BasicValue(3.14159265358979323846);
    if (name == "E") return BasicValue(2.71828182845904523536);
    if (name == "SQRT2") return BasicValue(1.41421356237309504880);
    if (name == "SQRT3") return BasicValue(1.73205080756887729352);
    if (name == "LOG2E") return BasicValue(1.44269504088896340736);
    if (name == "LOG10E") return BasicValue(0.43429448190325182765);
    if (name == "LN2") return BasicValue(0.69314718055994530942);
    if (name == "LN10") return BasicValue(2.30258509299404568402);
    
    // Size constants
    if (name == "KILOBYTE") return BasicValue(static_cast<double>(1024));
    if (name == "MEGABYTE") return BasicValue(static_cast<double>(1024 * 1024));
    if (name == "GIGABYTE") return BasicValue(static_cast<double>(1024 * 1024 * 1024));
    
    // Return 0 for unknown constants (could also throw error)
    return BasicValue(static_cast<double>(0));
}

BasicValue is_null(const BasicValue& value) {
    // Check if a value is NULL/nullptr
    if (std::holds_alternative<void*>(value)) {
        return BasicValue(std::get<void*>(value) == nullptr);
    }
    return BasicValue(false);
}

BasicValue not_null(const BasicValue& value) {
    // Check if a value is NOT NULL
    if (std::holds_alternative<void*>(value)) {
        return BasicValue(std::get<void*>(value) != nullptr);
    }
    return BasicValue(true); // Non-pointer values are considered "not null"
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

} // namespace basic_runtime