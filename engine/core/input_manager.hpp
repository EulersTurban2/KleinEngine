#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine::Core {

    class InputManager {
        public:
            static void init(GLFWwindow* window);

            // Polls GLFW events and snapshots keyboard/mouse state for the frame.
            static void update();

            // Keyboard API
            static bool isKeyPressed(int key);
            static bool isKeyReleased(int key);
            static bool isKeyDown(int key);

            // Mouse API
            static glm::vec2 getMousePosition();
            static glm::vec2 getMouseDelta();

        private:
            static GLFWwindow* mWindow;
            static std::unordered_map<int, bool> mKeyState;
            static std::unordered_map<int, bool> mPrevKeyState;
            static glm::vec2 mMousePosition;
            static glm::vec2 mPrevMousePosition;
            static bool mFirstMouse;
    };
}

#endif // INPUT_MANAGER_HPP
