#include "io_handler.h"
#include "console_io_handler.h"

namespace rbasic {

std::unique_ptr<IOHandler> createIOHandler(const std::string& type) {
    // Always return console handler - FFI will handle external interfaces
    (void)type; // Suppress unused parameter warning
    return std::make_unique<ConsoleIOHandler>();
}

} // namespace rbasic