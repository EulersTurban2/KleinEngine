#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "logger.hpp"

/**
    THE WINDOW CLASS!

    The Window class is responsible for creating and managing the application window using GLFW.
    It provides methods to initialize the window, handle window updates, and manage input events.
    The class is designed to be used in conjunction with the InputManager, Platform, Timer class.

    SAMPLE USAGE:
    #include <engine/all.hpp> // has the window class included, as well as a shorthand in preprocessing that allows you to use the Window class as "Window" instead of "Engine::Core::Window"
    ...
    // Create a window instance with the desired width, height, title, vsync, and fullscreen settings
    std::unique_ptr<Engine::Core::Window> mWindow = std::make_unique<Engine::Core::Window>(800, 600, "My Application", true, false);
    if (!mWindow->init()) {
        LOG_CRITICAL("Failed to initialize application window.");
        std::exit(EXIT_FAILURE);
    }
    ... 
*/



namespace Engine::Core {

    class Window {
        public:
            Window(int width, int height, const std::string& title, bool vsync = false, bool fullscreen = false)
                : mTitle(title), mWidth(width), mHeight(height), mVsync(vsync), mFullscreen(fullscreen) {}

            ~Window() {
                if (mWindow) {
                    LOG_INFO("Destroying window and releasing resources");
                    glfwDestroyWindow(mWindow);
                }
                glfwTerminate();
            }

            // Copying is forbidden: two Window instances sharing one GLFWwindow*
            // would double-destroy the native window.
            Window(const Window&) = delete;
            Window& operator=(const Window&) = delete;

            bool init();
            void onUpdate();
            bool shouldClose() const;

            GLFWwindow* getNativeWindow() const;
            int getWidth() const { return mWidth; }
            int getHeight() const { return mHeight; }

        private:
            std::string mTitle;
            int mWidth, mHeight;
            GLFWwindow* mWindow = nullptr;
            bool mVsync;
            bool mFullscreen;

            void setCallbacks();

            // Used by the GLFW callbacks
            void setWidth(int width) { mWidth = width; }
            void setHeight(int height) { mHeight = height; }

            static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    };
}

#endif // WINDOW_HPP
