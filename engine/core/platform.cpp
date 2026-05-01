#include "platform.hpp"
#include <core/window.hpp>
#include <core/input_manager.hpp>
#include <GLFW/glfw3.h>

namespace Engine {
    namespace Platform {

        // --- RAW GLFW CALLBACKS ---
        static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            // Hardcode the escape key for now to retain your old window-closing behavior
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }

            Core::InputManager::keyCallback(key, action);
        }

        static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
            Core::InputManager::mouseCallback(xpos, ypos);
        }

        void PlatformManager::init(Core::Window* window) {
            GLFWwindow* nativeWin = window->getNativeWindow();

            glfwSetKeyCallback(nativeWin, glfw_key_callback);
            glfwSetCursorPosCallback(nativeWin, glfw_cursor_position_callback);
        }

    }
}