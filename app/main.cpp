#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "app.hpp"
#include "core/logger.hpp"

using LOGGER = Engine::Core::Logger;
using LOG_LEVEL = Engine::Core::LOG_LEVEL;

int main() {
    LOGGER::get().setStream(std::cout);
    LOG_INFO("Starting application...");
    App::App* app = new App::App();
    app->run();
    delete app;
    return 0;
}