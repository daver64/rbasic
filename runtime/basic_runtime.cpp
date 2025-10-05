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

#ifdef RBASIC_SDL_SUPPORT
// Simple SDL state for compiled programs
static SDL_Window* g_sdl_window = nullptr;
static SDL_Renderer* g_sdl_renderer = nullptr;
static bool g_sdl_initialized = false;
static int g_sdl_r = 255, g_sdl_g = 255, g_sdl_b = 255;
static bool g_quit_requested = false;

void init_sdl_if_needed() {
    if (!g_sdl_initialized) {
        if (SDL_Init(SDL_INIT_VIDEO) == 0) {
            g_sdl_window = SDL_CreateWindow("RBASIC Graphics", 
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                800, 600, SDL_WINDOW_SHOWN);
            if (g_sdl_window) {
                g_sdl_renderer = SDL_CreateRenderer(g_sdl_window, -1, SDL_RENDERER_ACCELERATED);
                if (g_sdl_renderer) {
                    SDL_SetRenderDrawColor(g_sdl_renderer, 0, 0, 0, 255);
                    SDL_RenderClear(g_sdl_renderer);
                    SDL_RenderPresent(g_sdl_renderer);
                }
            }
        }
        g_sdl_initialized = true;
    }
}

void process_sdl_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            g_quit_requested = true;
        }
    }
}
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
    
#ifdef RBASIC_SDL_SUPPORT
    init_sdl_if_needed();
#endif
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
    } else {
#ifdef RBASIC_SDL_SUPPORT
        init_sdl_if_needed();
        if (g_sdl_window) {
            SDL_SetWindowSize(g_sdl_window, width, height);
            SDL_SetWindowPosition(g_sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
#endif
    }
}

void text_mode() {
    if (g_io_handler) {
        g_io_handler->text_mode();
    } else {
#ifdef RBASIC_SDL_SUPPORT
        // Clean up SDL resources
        if (g_sdl_renderer) {
            SDL_DestroyRenderer(g_sdl_renderer);
            g_sdl_renderer = nullptr;
        }
        if (g_sdl_window) {
            SDL_DestroyWindow(g_sdl_window);
            g_sdl_window = nullptr;
        }
        if (g_sdl_initialized) {
            SDL_Quit();
            g_sdl_initialized = false;
        }
#endif
    }
}

void clear_screen() {
    if (g_io_handler) {
        g_io_handler->clear_screen();
    } else {
#ifdef RBASIC_SDL_SUPPORT
        if (g_sdl_renderer) {
            SDL_SetRenderDrawColor(g_sdl_renderer, 0, 0, 0, 255);
            SDL_RenderClear(g_sdl_renderer);
        }
#endif
    }
}

void set_colour(int r, int g, int b) {
    if (g_io_handler) {
        g_io_handler->set_colour(r, g, b);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        g_sdl_r = r;
        g_sdl_g = g;
        g_sdl_b = b;
#endif
    }
}

void draw_pixel(int x, int y) {
    if (g_io_handler) {
        g_io_handler->draw_pixel(x, y);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        if (g_sdl_renderer) {
            SDL_SetRenderDrawColor(g_sdl_renderer, g_sdl_r, g_sdl_g, g_sdl_b, 255);
            SDL_RenderDrawPoint(g_sdl_renderer, x, y);
        }
#endif
    }
}

void draw_line(int x1, int y1, int x2, int y2) {
    if (g_io_handler) {
        g_io_handler->draw_line(x1, y1, x2, y2);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        if (g_sdl_renderer) {
            SDL_SetRenderDrawColor(g_sdl_renderer, g_sdl_r, g_sdl_g, g_sdl_b, 255);
            SDL_RenderDrawLine(g_sdl_renderer, x1, y1, x2, y2);
        }
#endif
    }
}

void draw_rect(int x, int y, int width, int height, bool filled) {
    if (g_io_handler) {
        g_io_handler->draw_rect(x, y, width, height, filled);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        if (g_sdl_renderer) {
            SDL_SetRenderDrawColor(g_sdl_renderer, g_sdl_r, g_sdl_g, g_sdl_b, 255);
            if (filled) {
                SDL_Rect rect = {x, y, width, height};
                SDL_RenderFillRect(g_sdl_renderer, &rect);
            } else {
                SDL_Rect rect = {x, y, width, height};
                SDL_RenderDrawRect(g_sdl_renderer, &rect);
            }
        }
#endif
    }
}

void draw_text(int x, int y, const std::string& text) {
    if (g_io_handler) {
        g_io_handler->draw_text(x, y, text);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        // For compiled programs without IOHandler, we'd need to implement direct SDL text rendering
        // For now, this is a placeholder - most graphics programs will use the IOHandler path
        std::cout << "[Text at (" << x << "," << y << "): " << text << "]" << std::endl;
#endif
    }
}

void refresh_screen() {
    if (g_io_handler) {
        g_io_handler->refresh_screen();
    } else {
#ifdef RBASIC_SDL_SUPPORT
        if (g_sdl_renderer) {
            SDL_RenderPresent(g_sdl_renderer);
        }
#endif
    }
}

bool key_pressed(const std::string& key) {
    if (g_io_handler) {
        return g_io_handler->key_pressed(key);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        process_sdl_events();  // Process events first
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (key == "ESC" && e.key.keysym.sym == SDLK_ESCAPE) {
                    return true;
                }
                // Add more key mappings as needed
            }
            if (e.type == SDL_QUIT) {
                g_quit_requested = true;
            }
        }
#endif
    }
    return false;
}

bool quit_requested() {
    if (g_io_handler) {
        return g_io_handler->quit_requested();
    } else {
#ifdef RBASIC_SDL_SUPPORT
        process_sdl_events();  // Process events first
        return g_quit_requested;
#endif
    }
    return false;
}

void sleep_ms(int ms) {
    if (g_io_handler) {
        g_io_handler->sleep_ms(ms);
    } else {
#ifdef RBASIC_SDL_SUPPORT
        process_sdl_events();  // Process events before sleeping
        SDL_Delay(ms);
#endif
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

#ifdef RBASIC_SQLITE_SUPPORT
// SQLite support for compiled programs
static sqlite3* g_sqlite_db = nullptr;
static std::string g_sqlite_last_error;

BasicValue db_open(const std::string& database_path) {
    if (g_sqlite_db) {
        sqlite3_close(g_sqlite_db);
        g_sqlite_db = nullptr;
    }
    
    int result = sqlite3_open(database_path.c_str(), &g_sqlite_db);
    if (result != SQLITE_OK) {
        g_sqlite_last_error = sqlite3_errmsg(g_sqlite_db);
        sqlite3_close(g_sqlite_db);
        g_sqlite_db = nullptr;
        return false;
    }
    
    g_sqlite_last_error.clear();
    return true;
}

BasicValue db_close() {
    if (g_sqlite_db) {
        sqlite3_close(g_sqlite_db);
        g_sqlite_db = nullptr;
    }
    g_sqlite_last_error.clear();
    return true;
}

BasicValue db_exec(const std::string& sql) {
    if (!g_sqlite_db) {
        g_sqlite_last_error = "Database not open";
        return false;
    }
    
    char* error_msg = nullptr;
    int result = sqlite3_exec(g_sqlite_db, sql.c_str(), nullptr, nullptr, &error_msg);
    
    if (result != SQLITE_OK) {
        g_sqlite_last_error = error_msg ? error_msg : "Unknown error";
        if (error_msg) {
            sqlite3_free(error_msg);
        }
        return false;
    }
    
    g_sqlite_last_error.clear();
    return true;
}

BasicValue db_query(const std::string& sql) {
    if (!g_sqlite_db) {
        g_sqlite_last_error = "Database not open";
        return std::string("");
    }
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(g_sqlite_db, sql.c_str(), -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        g_sqlite_last_error = sqlite3_errmsg(g_sqlite_db);
        return std::string("");
    }
    
    std::ostringstream output;
    bool first_row = true;
    
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (!first_row) {
            output << "\n";
        }
        first_row = false;
        
        int col_count = sqlite3_column_count(stmt);
        for (int i = 0; i < col_count; i++) {
            if (i > 0) output << "\t";
            
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            output << (text ? text : "");
        }
    }
    
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        g_sqlite_last_error = sqlite3_errmsg(g_sqlite_db);
        return std::string("");
    }
    
    g_sqlite_last_error.clear();
    return output.str();
}

BasicValue db_error() {
    return g_sqlite_last_error;
}

BasicValue db_escape(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.length() * 2);
    
    for (char c : str) {
        if (c == '\'') {
            escaped += "''";
        } else {
            escaped += c;
        }
    }
    
    return escaped;
}
#endif

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

} // namespace basic_runtime