#include "window.hpp"
#include "platform.hpp"

namespace Engine::Core {

    bool Window::init() {
        // Request the OpenGL 3.3 core profile
        if (!glfwInit()) {
            LOG_CRITICAL("Failed to initialize GLFW");
            return false;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        LOG_INFO("Creating window with title: " + mTitle + ", width: " + std::to_string(mWidth) + ", height: " + std::to_string(mHeight));
        GLFWmonitor* monitor = mFullscreen ? glfwGetPrimaryMonitor() : nullptr;
        mWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), monitor, nullptr);
        if (mWindow == nullptr) {
            LOG_CRITICAL("Error in allocating window resources");
            glfwTerminate();
            return false;
        }
        LOG_INFO("Window created successfully");
        LOG_INFO("Setting up OpenGL context and loading OpenGL functions with GLAD");
        glfwMakeContextCurrent(mWindow);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            LOG_CRITICAL("Failed to initialize GLAD");
            return false;
        }
        LOG_INFO("GLAD initialized successfully");

        glfwSetWindowUserPointer(mWindow, this);
        setCallbacks();
        Platform::PlatformManager::init(this);
        glfwSwapInterval(mVsync ? 1 : 0);

        return true;
    }

    void Window::onUpdate() {
        glfwSwapBuffers(mWindow);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(mWindow);
    }

    GLFWwindow* Window::getNativeWindow() const {
        return mWindow;
    }

    void Window::setCallbacks() {
        glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (win) {
            win->setWidth(width);
            win->setHeight(height);
        }
        glViewport(0, 0, width, height);
        LOG_INFO("Framebuffer resized: new width = " + std::to_string(width) + ", new height = " + std::to_string(height));
    }
}
