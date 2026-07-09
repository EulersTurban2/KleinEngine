#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "logger.hpp"

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
