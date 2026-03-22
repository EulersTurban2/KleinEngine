#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include <core/app.hpp>


int main() {
    std::cout << "Hello, Klein Engine!" << std::endl;
    
    App::App* app = new App::App();
    app->run();
    app->~App();
    return 0;
}