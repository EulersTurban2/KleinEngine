#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logger.hpp"

namespace Engine{
    namespace Core{
        class Window {

            public:
                Window(int width, int height, const std::string& title, bool vsync = false): _width(width), _height(height), _title(title), _vsync(vsync) {};
                ~Window() {
                    if (window)
                    {
                        LOG_INFO("Destroying window and releasing resources");
                        glfwDestroyWindow(window);
                    }
                    glfwTerminate();
                };

                // zabranjujemo kopiranje prozora, jer ne zelimo da se desi da imamo 
                // vise instanci prozora koje dele isti GLFWwindow* pointer, sto bi dovelo do problema sa resursima i stabilnoscu
                Window(const Window&) = delete;
                Window& operator=(const Window&) = delete;

                bool init();
                void onUpdate();
                bool shouldClose() const;
                void processInput();

                GLFWwindow* getNativeWindow() const;
                int getWidth() const { return _width; }
                int getHeight() const { return _height; }

            private:
                std::string _title;
                int _width, _height;
                GLFWwindow* window;
                bool _vsync;

                void setCallbacks();

                // potrebnosti za callback funkcije
                void setWidth(int width) { _width = width; }
                void setHeight(int height) { _height = height; }

                static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        };
    }
}
#endif // __WINDOW_HPP__