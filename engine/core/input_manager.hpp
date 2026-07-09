#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>

/**
    THE INPUT MANAGER!

    The InputManager class is responsible for managing and handling user input events, including keyboard and mouse interactions.
    It provides a simple interface to query the state of keys and mouse buttons, as well as retrieving the current mouse position and movement delta.
    The class is designed to be used in conjunction with the Window class, which provides the necessary GLFW window context for input handling.

    SAMPLE USAGE:
    #include <engine/all.hpp> // has the input manager class included, as well as a shorthand in preprocessing
                              // that allows you to use the InputManager class as "Input" instead of "Engine::Core::InputManager"
    ...
    // Initialize the InputManager with the GLFW window context
    GLFWwindow* window = mWindow->getNativeWindow(); // mWindow is a pointer to the Window class instance!
    Input::init(window);
    ...
    while (running) {
        // Poll events and update input state
        Input::update();

        // Check if a specific key is pressed
        if (Input::isKeyPressed(GLFW_KEY_W)) {
            // Move forward
        }

        // Get the current mouse position
        glm::vec2 mousePos = Input::getMousePosition();
    }
    

*/


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
