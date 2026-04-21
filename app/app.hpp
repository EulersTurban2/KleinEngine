#ifndef __APP_HPP
#define __APP_HPP

#include "core/window.hpp"
#include "renderer/shaders.hpp"
#include "core/platform.hpp"
#include "scene/camera.hpp"
#include "math/lorentz.hpp"
#include "core/input_manager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

using INPUT = Engine::Core::InputManager;

namespace App {
    class App {
    public:
        App() {
            // In the future, this can be loaded via a JSON parser
            m_window = new Engine::Core::Window(800, 600, "Klein Engine - Hyperbolic Sandbox");
            if (m_window->init() != true) {
                LOG_CRITICAL("Failed to initialize application window.");
            }
        }

        ~App() {
            delete m_window;
        }

        void run() {
            GLFWwindow* nativeWin = m_window->getNativeWindow();

            INPUT::init(nativeWin);
            // --- Resource Setup ---
            // We use the stack for Shaders because the Program links them and then we don't need them
            Engine::Resources::Shader vertexShader("../resources/shaders/vertex/vert1.glsl", Engine::Resources::VERTEX_SHADER);
            Engine::Resources::Shader fragmentShader("../resources/shaders/fragment/frag1.glsl", Engine::Resources::FRAGMENT_SHADER);
            
            // Using our new unique-type-checked constructor
            Engine::Resources::ShaderProgram shaderProgram({ &vertexShader, &fragmentShader });

            // --- Geometry Setup (Buffer logic remains similar but encapsulated) ---
            unsigned int VBO, VAO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(solidIndices) + sizeof(lineIndices), NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(solidIndices), solidIndices);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(solidIndices), sizeof(lineIndices), lineIndices);

            // Attributes: Position (0), Normal/Color (1)
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);            
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            
            // --- State Setup ---
            Engine::Camera::Camera camera;
            glfwSetInputMode(nativeWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            
            float deltaTime = 0.0f;
            float lastFrame = 0.0f;
            bool transparent_flag = true;
            
            // --- Main Loop ---
            while (!m_window->shouldClose()) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                
                INPUT::Update();
                m_window->onUpdate();

                float xoffset = INPUT::getMouseDelta().x;
                float yoffset = INPUT::getMouseDelta().y;

                camera.processMouseMovement(xoffset, yoffset);

                // Keyboard Input (To be moved to an InputManager later)
                if (INPUT::isKeyDown(GLFW_KEY_W)) camera.processKeyboard(Engine::Camera::CameraMovement::FORWARD, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_S)) camera.processKeyboard(Engine::Camera::CameraMovement::BACKWARD, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_A)) camera.processKeyboard(Engine::Camera::CameraMovement::LEFT, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_D)) camera.processKeyboard(Engine::Camera::CameraMovement::RIGHT, deltaTime);

                // Toggle Logic (Consider adding a 'key released' check to prevent flickering)
                if (INPUT::isKeyPressed(GLFW_KEY_T)) transparent_flag = !transparent_flag;
                if (INPUT::isKeyPressed(GLFW_KEY_G)) camera.toggleGeometry();
                if (INPUT::isKeyPressed(GLFW_KEY_SPACE)) camera.reset();

                // --- Rendering ---
                glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Slightly darker for better hyperbolic contrast
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                shaderProgram.use();

                // Calculate Matrices
                glm::mat4 view = camera.getViewMatrix();
                glm::mat4 projection = camera.getProjectionMatrix(800.0f, 600.0f);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));
                float scale = 1.0f + 0.5f * glm::sin((float)glfwGetTime());
                model = glm::scale(model, glm::vec3(scale));

                // --- THE CLEAN PART: Using the new Setters ---
                shaderProgram.setMat4("uModel", model);
                shaderProgram.setMat4("uView", view);
                shaderProgram.setMat4("uProj", projection);
                shaderProgram.setBool("uIsHyperbolic", camera.getIsHyperbolic());

                glBindVertexArray(VAO);
                
                if (transparent_flag) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                } else {
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                }
                
                // Draw wireframe overlay
                glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)sizeof(solidIndices));
            }

            // Cleanup
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

    private:
        Engine::Core::Window* m_window;

        // --- Data (Ideally moved to a Mesh class next) ---
        float cubeVertices[144] = {
            // Positions          // Normals
            // Front face (Normal: 0.0f, 0.0f, 1.0f)
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 0: Bottom-Left
            0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 1: Bottom-Right
            0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 2: Top-Right
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 3: Top-Left

            // Back face (Normal: 0.0f, 0.0f, -1.0f)
            0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f, // 4: Bottom-Left
            -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f, // 5: Bottom-Right
            -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f, // 6: Top-Right
            0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f, // 7: Top-Left

            // Left face (Normal: -1.0f, 0.0f, 0.0f)
            -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, // 8: Bottom-Left
            -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, // 9: Bottom-Right
            -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f, // 10: Top-Right
            -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f, // 11: Top-Left

            // Right face (Normal: 1.0f, 0.0f, 0.0f)
            0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f, // 12: Bottom-Left
            0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f, // 13: Bottom-Right
            0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f, // 14: Top-Right
            0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f, // 15: Top-Left

            // Top face (Normal: 0.0f, 1.0f, 0.0f)
            -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f, // 16: Bottom-Left
            0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f, // 17: Bottom-Right
            0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f, // 18: Top-Right
            -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f, // 19: Top-Left

            // Bottom face (Normal: 0.0f, -1.0f, 0.0f)
            -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f, // 20: Bottom-Left
            0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f, // 21: Bottom-Right
            0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f, // 22: Top-Right
            -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f  // 23: Top-Left
        };

        unsigned int solidIndices[36] = {
            0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
            8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20
        };

        unsigned int lineIndices[24] = {
            0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 
        };
    };    
}

#endif