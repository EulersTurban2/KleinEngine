#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include "app.hpp"


int main() {
    std::cout << "Hello, Klein Engine!" << std::endl;
    App::App* app = new App::App();
    app->run();
    delete app;
    return 0;
}