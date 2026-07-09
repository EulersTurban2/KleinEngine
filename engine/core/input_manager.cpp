#include "input_manager.hpp"

namespace Engine::Core {

    GLFWwindow* InputManager::mWindow = nullptr;
    std::unordered_map<int, bool> InputManager::mKeyState;
    std::unordered_map<int, bool> InputManager::mPrevKeyState;
    glm::vec2 InputManager::mMousePosition(0.0f);
    glm::vec2 InputManager::mPrevMousePosition(0.0f);
    bool InputManager::mFirstMouse = true;

    void InputManager::init(GLFWwindow* window) {
        mWindow = window;
    }

    
    void InputManager::update() {
        mPrevKeyState = mKeyState;
        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key) {
            mKeyState[key] = glfwGetKey(mWindow, key) == GLFW_PRESS;
        }

        mPrevMousePosition = mMousePosition;
        double x, y;
        glfwGetCursorPos(mWindow, &x, &y);
        mMousePosition = glm::vec2(x, y);
        if (mFirstMouse) {
            mPrevMousePosition = mMousePosition;
            mFirstMouse = false;
        }
        glfwPollEvents();
    }

    bool InputManager::isKeyPressed(int key) {
        return mKeyState[key] && !mPrevKeyState[key];
    }

    bool InputManager::isKeyReleased(int key) {
        return !mKeyState[key] && mPrevKeyState[key];
    }

    bool InputManager::isKeyDown(int key) {
        return mKeyState[key];
    }

    glm::vec2 InputManager::getMousePosition() {
        return mMousePosition;
    }

    glm::vec2 InputManager::getMouseDelta() {
        return mMousePosition - mPrevMousePosition;
    }
}
