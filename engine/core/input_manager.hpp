#ifndef __INPUT_MANAGER_HPP
#define __INPUT_MANAGER_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Engine{
    namespace Core{
        class InputManager{
            public:
                static void Update();

                //Keyboard API
                static bool isKeyPressed(int key);
                static bool isKeyReleased(int key);
                static bool isKeyDown(int key);

                //Mouse API
                static glm::vec2 getMousePosition();
                static glm::vec2 getMouseDelta();

                static void keyCallback(int key, int action);
                static void mouseCallback(double xpos, double ypos);

                static void init(GLFWwindow* window);
            private:
                static GLFWwindow* m_window;
                static std::unordered_map<int, bool> m_keyState;
                static std::unordered_map<int, bool> m_prevKeyState;
                static glm::vec2 m_mousePosition;
                static glm::vec2 m_prevMousePosition;
                static bool m_firstMouse;
        };
    }
}


#endif // __INPUT_MANAGER_HPP