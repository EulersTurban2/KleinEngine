#include "input_manager.hpp"

namespace Engine{
    namespace Core{
        GLFWwindow* InputManager::m_window = nullptr;
        std::unordered_map<int, bool> InputManager::m_keyState;
        std::unordered_map<int, bool> InputManager::m_prevKeyState;
        glm::vec2 InputManager::m_mousePosition(0.0f);
        glm::vec2 InputManager::m_prevMousePosition(0.0f);
        bool InputManager::m_firstMouse = true;

        void InputManager::init(GLFWwindow* window){
            m_window = window;
        }

        void InputManager::keyCallback(int key, int action){
            if (action == 1) {
                m_keyState[key] = true;
            } else if (action == 0) {
                m_keyState[key] = false;
            }
        }

        void InputManager::mouseCallback(double xpos, double ypos){
            m_mousePosition.x = static_cast<float>(xpos);
            m_mousePosition.y = static_cast<float>(ypos);

            if (m_firstMouse) {
                m_prevMousePosition = m_mousePosition;
                m_firstMouse = false;
            }
        }

        void InputManager::Update(){
            m_prevKeyState = m_keyState;
            for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key){
                m_keyState[key] = glfwGetKey(m_window, key) == GLFW_PRESS;
            }

            m_prevMousePosition = m_mousePosition;
            double x, y;
            glfwGetCursorPos(m_window, &x, &y);
            m_mousePosition = glm::vec2(x, y);
            if (m_firstMouse)
            {
                m_prevMousePosition = m_mousePosition;
                m_firstMouse = false;
            }
        }

        bool InputManager::isKeyPressed(int key){
            return m_keyState[key] && !m_prevKeyState[key];
        }

        bool InputManager::isKeyReleased(int key){
            return !m_keyState[key] && m_prevKeyState[key];
        }

        bool InputManager::isKeyDown(int key){
            return m_keyState[key];
        }

        glm::vec2 InputManager::getMousePosition(){
            return m_mousePosition;
        }

        glm::vec2 InputManager::getMouseDelta(){
            return m_mousePosition - m_prevMousePosition;
        }

    }
}