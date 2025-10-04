#include "sdl_io_handler.h"

#ifdef RBASIC_SDL_SUPPORT
#include <iostream>
#include <cmath>
#include <algorithm>
#include <SDL_ttf.h>

namespace rbasic {

SDLIOHandler::SDLIOHandler() : start_ticks(0) {
    // Initialise SDL but don't create window yet
    // Window creation happens in graphics_mode()
}

SDLIOHandler::~SDLIOHandler() {
    cleanup_sdl();
}

bool SDLIOHandler::init_sdl() {
    if (initialized) return true;
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialise! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialise! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    start_ticks = SDL_GetTicks();
    initialized = true;
    return true;
}

void SDLIOHandler::cleanup_sdl() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (initialized) {
        TTF_Quit();
        SDL_Quit();
        initialized = false;
    }
}

void SDLIOHandler::handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                should_quit = true;
                break;
                
            case SDL_KEYDOWN:
                keys_pressed[e.key.keysym.scancode] = true;
                
                // Handle text input for console
                if (input_active) {
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        input_queue.push(current_input);
                        add_console_line("> " + current_input);
                        current_input.clear();
                        input_active = false;
                    } else if (e.key.keysym.sym == SDLK_BACKSPACE && !current_input.empty()) {
                        current_input.pop_back();
                    }
                }
                break;
                
            case SDL_KEYUP:
                keys_pressed[e.key.keysym.scancode] = false;
                break;
                
            case SDL_TEXTINPUT:
                if (input_active) {
                    current_input += e.text.text;
                }
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_button_pressed = true;
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouse_button_pressed = false;
                }
                break;
                
            case SDL_MOUSEMOTION:
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
                break;
        }
    }
}

void SDLIOHandler::render_console() {
    if (!graphics_mode_active || !renderer) return;
    
    // Set text colour (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Simple text rendering (would need SDL_ttf for proper fonts)
    int y = 10;
    for (const auto& line : console_lines) {
        // For now, just indicate text positions
        // Real implementation would use SDL_ttf to render actual text
        print_at(10, y, line);
        y += 20;
    }
    
    // Show current input if active
    if (input_active) {
        print_at(10, y, "> " + current_input + "_");
    }
}

void SDLIOHandler::add_console_line(const std::string& line) {
    console_lines.push_back(line);
    
    // Keep only last 25 lines to avoid memory issues
    if (console_lines.size() > 25) {
        console_lines.erase(console_lines.begin());
    }
}

// Text output methods
void SDLIOHandler::print(const std::string& text) {
    if (graphics_mode_active) {
        // Add to console buffer
        if (!console_lines.empty()) {
            console_lines.back() += text;
        } else {
            add_console_line(text);
        }
    } else {
        // Fallback to standard output
        std::cout << text;
        std::cout.flush();
    }
}

void SDLIOHandler::println(const std::string& text) {
    if (graphics_mode_active) {
        add_console_line(text);
    } else {
        std::cout << text << std::endl;
    }
}

void SDLIOHandler::newline() {
    if (graphics_mode_active) {
        add_console_line("");
    } else {
        std::cout << std::endl;
    }
}

// Text input methods
std::string SDLIOHandler::input() {
    if (graphics_mode_active) {
        input_active = true;
        current_input.clear();
        
        // Wait for input to be completed
        while (input_active && !should_quit) {
            handle_events();
            refresh_screen();
            SDL_Delay(16); // ~60 FPS
        }
        
        if (!input_queue.empty()) {
            std::string result = input_queue.front();
            input_queue.pop();
            return result;
        }
        return "";
    } else {
        // Fallback to standard input
        std::string line;
        std::getline(std::cin, line);
        return line;
    }
}

std::string SDLIOHandler::input(const std::string& prompt) {
    print(prompt);
    return input();
}

// Graphics methods
void SDLIOHandler::graphics_mode(int width, int height) {
    if (!init_sdl()) return;
    
    window_width = width;
    window_height = height;
    
    if (!window) {
        window = SDL_CreateWindow("rbasic Graphics",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                width, height, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
    }
    
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
    }
    
    // Load a font for text rendering
    if (!font) {
        // Try to load a system font (Windows)
        font = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 16);
        if (!font) {
            // Fallback to courier
            font = TTF_OpenFont("C:/Windows/Fonts/cour.ttf", 16);
        }
        if (!font) {
            // Another fallback
            font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
        }
        if (!font) {
            std::cerr << "Could not load font! TTF_Error: " << TTF_GetError() << std::endl;
            // Continue without font - will disable text rendering
        }
    }
    
    graphics_mode_active = true;
    
    // Enable text input
    SDL_StartTextInput();
    
    // Clear screen to black
    clear_screen();
}

void SDLIOHandler::text_mode() {
    graphics_mode_active = false;
    SDL_StopTextInput();
}

void SDLIOHandler::clear_screen() {
    if (!renderer) return;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);
}

void SDLIOHandler::set_colour(int r, int g, int b) {
    current_colour = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255};
    if (renderer) {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    }
}

void SDLIOHandler::draw_pixel(int x, int y) {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, current_colour.r, current_colour.g, current_colour.b, current_colour.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

void SDLIOHandler::draw_line(int x1, int y1, int x2, int y2) {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, current_colour.r, current_colour.g, current_colour.b, current_colour.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void SDLIOHandler::draw_rect(int x, int y, int width, int height, bool filled) {
    if (!renderer) return;
    
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, current_colour.r, current_colour.g, current_colour.b, current_colour.a);
    
    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void SDLIOHandler::draw_circle(int x, int y, int radius, bool filled) {
    if (!renderer) return;
    
    SDL_SetRenderDrawColor(renderer, current_colour.r, current_colour.g, current_colour.b, current_colour.a);
    
    // Simple circle drawing algorithm
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                if (filled || (dx*dx + dy*dy) > ((radius-1) * (radius-1))) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            }
        }
    }
}

void SDLIOHandler::print_at(int x, int y, const std::string& text) {
    if (!renderer || !font || text.empty()) return;
    
    // Create text surface
    SDL_Color textColor = current_colour;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }
    
    // Create texture from surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }
    
    // Get text dimensions
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    
    // Render the text
    SDL_Rect destRect = {x, y, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    
    // Clean up
    SDL_DestroyTexture(textTexture);
}

void SDLIOHandler::draw_text(int x, int y, const std::string& text) {
    // draw_text is just an alias for print_at for direct text rendering
    print_at(x, y, text);
}

void SDLIOHandler::refresh_screen() {
    if (!renderer) return;
    
    handle_events();
    
    if (graphics_mode_active) {
        render_console();
    }
    
    SDL_RenderPresent(renderer);
}

// Input handling methods
bool SDLIOHandler::key_pressed(const std::string& key) {
    handle_events();
    
    if (key == "SPACE") return keys_pressed[SDL_SCANCODE_SPACE];
    if (key == "ENTER") return keys_pressed[SDL_SCANCODE_RETURN];
    if (key == "ESC") return keys_pressed[SDL_SCANCODE_ESCAPE];
    if (key == "LEFT") return keys_pressed[SDL_SCANCODE_LEFT];
    if (key == "RIGHT") return keys_pressed[SDL_SCANCODE_RIGHT];
    if (key == "UP") return keys_pressed[SDL_SCANCODE_UP];
    if (key == "DOWN") return keys_pressed[SDL_SCANCODE_DOWN];
    
    if (key.length() == 1) {
        char c = key[0];
        if (c >= 'A' && c <= 'Z') {
            return keys_pressed[SDL_SCANCODE_A + (c - 'A')];
        }
        if (c >= 'a' && c <= 'z') {
            return keys_pressed[SDL_SCANCODE_A + (c - 'a')];
        }
        if (c >= '0' && c <= '9') {
            return keys_pressed[SDL_SCANCODE_1 + (c - '1')];
        }
    }
    
    return false;
}

bool SDLIOHandler::mouse_clicked() {
    handle_events();
    return mouse_button_pressed;
}

void SDLIOHandler::get_mouse_pos(int& x, int& y) {
    handle_events();
    x = mouse_x;
    y = mouse_y;
}

bool SDLIOHandler::quit_requested() {
    handle_events();
    return should_quit;
}

// Audio methods
void SDLIOHandler::play_sound(const std::string& filename) {
    // TODO: Implement SDL audio loading and playback
    std::cout << "[SDL: Playing sound " << filename << "]" << std::endl;
}

void SDLIOHandler::play_tone(int frequency, int duration_ms) {
    // TODO: Implement tone generation
    std::cout << "[SDL: Playing tone " << frequency << "Hz for " << duration_ms << "ms]" << std::endl;
}

// Utility methods
void SDLIOHandler::sleep_ms(int milliseconds) {
    SDL_Delay(milliseconds);
}

int SDLIOHandler::get_ticks() {
    if (initialized) {
        return SDL_GetTicks() - start_ticks;
    }
    return 0;
}

} // namespace rbasic

#endif // RBASIC_SDL_SUPPORT