#include <iostream>

#include "app.hpp"
#include "core/logger.hpp"

int main() {
    Engine::Core::Logger::get().setStream(std::cout);
    LOG_INFO("Starting application...");
    App::App app;
    app.run();
    return 0;
}
