#include "basic_runtime.h"
#include "../include/io_handler.h"
#include <iostream>
#include <sstream>
#include <algorithm>

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

// Global IOHandler for compiled programs
static rbasic::IOHandler* g_io_handler = nullptr;

void init_io_handler(rbasic::IOHandler* handler) {
    g_io_handler = handler;
}

rbasic::IOHandler* get_io_handler() {
    return g_io_handler;
}

void init_runtime() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
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
    }
}

void print_line() {
    if (g_io_handler) {
        g_io_handler->newline();
    } else {
        std::cout << std::endl;
    }
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
        if (line.find('.') != std::string::npos) {
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
    start = std::max(0, start - 1); // BASIC is 1-indexed
    
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
    int start = std::max(0, static_cast<int>(s.length()) - length);
    return s.substr(start);
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

void set_color(int r, int g, int b) {
    if (g_io_handler) {
        g_io_handler->set_color(r, g, b);
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
        if (idx >= 1 && idx <= static_cast<int>(array.elements.size())) {
            return array.elements[idx - 1]; // Convert to 0-based indexing
        }
    }
    return BasicValue(0); // Default value
}

void set_array_element(BasicValue& arrayVar, BasicValue index, BasicValue value) {
    if (std::holds_alternative<BasicArray>(arrayVar)) {
        BasicArray& array = std::get<BasicArray>(arrayVar);
        int idx = to_int(index);
        if (idx >= 1 && idx <= static_cast<int>(array.elements.size())) {
            array.elements[idx - 1] = value; // Convert to 0-based indexing
        }
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

} // namespace basic_runtime