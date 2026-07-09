#include "platform.hpp"
#include <core/window.hpp>
#include <GLFW/glfw3.h>

namespace Engine::Platform {

    // --- RAW GLFW CALLBACKS ---
    static void glfw_key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
        // Escape closes the window; keyboard state is otherwise polled by InputManager::update().
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void PlatformManager::init(Core::Window* window) {
        GLFWwindow* nativeWin = window->getNativeWindow();

        glfwSetKeyCallback(nativeWin, glfw_key_callback);
    }
}
