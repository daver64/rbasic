#include "io_handler.h"
#include "console_io_handler.h"

#ifdef RBASIC_SDL_SUPPORT
#include "sdl_io_handler.h"
#endif

namespace rbasic {

std::unique_ptr<IOHandler> createIOHandler(const std::string& type) {
    if (type == "console") {
        return std::make_unique<ConsoleIOHandler>();
    }
#ifdef RBASIC_SDL_SUPPORT
    else if (type == "sdl" || type == "graphics") {
        return std::make_unique<SDLIOHandler>();
    }
#endif
    else {
        // Default to console
        return std::make_unique<ConsoleIOHandler>();
    }
}

} // namespace rbasic