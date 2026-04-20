#include <core/window.hpp>
#include <core/platform.hpp>
#include "window.hpp"

namespace Engine{
    namespace Core {
        bool Window::init(){
            // postavljanje opengl specifikacije
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
            glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    
            LOG_INFO("Creating window with title: " + _title + ", width: " + std::to_string(_width) + ", height: " + std::to_string(_height));
            this->window = glfwCreateWindow(_width,_height,_title.c_str(),NULL,NULL);
            if (this->window == NULL)
            {
                glfwTerminate();
                LOG_CRITICAL("Error in allocating window resources");
            }
            LOG_INFO("Window created successfully");
            LOG_INFO("Setting up OpenGL context and loading OpenGL functions with GLAD");
            glfwMakeContextCurrent(this->window);
            // setting up GLAD
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                LOG_CRITICAL("Failed to initialize GLAD");
            }   
            LOG_INFO("GLAD initialized successfully");

            setCallbacks();
            glfwSwapInterval(_vsync ? 1 : 0);


            return true;
        }

        void Window::onUpdate()
        {
            processInput();
            glfwPollEvents();
            glfwSwapBuffers(this->window);
        }
        bool Window::shouldClose() const
        {
            return glfwWindowShouldClose(this->window);
        }
        GLFWwindow *Window::getNativeWindow() const
        {
            return window;
        }
        void Window::processInput()
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                LOG_INFO("Escape key pressed. Closing window.");
                glfwSetWindowShouldClose(window, true);
            }
        }
        void Window::setCallbacks()
        {
            glfwSetFramebufferSizeCallback(this->window,framebuffer_size_callback);
        }
        void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
            win->setWidth(width);
            win->setHeight(height);
            glViewport(0,0,width,height);
            LOG_INFO("Framebuffer resized: new width = " + std::to_string(width) + ", new height = " + std::to_string(height));
        }
    }
}
