#ifndef __APP_HPP
#define __APP_HPP

#include "core/window.hpp"
#include "renderer/shaders.hpp"
#include "core/platform.hpp"
#include "scene/camera.hpp"
#include "math/lorentz.hpp"

#include <glm/gtc/type_ptr.hpp>

// Poenta jeste da APP bude samo petlja u kojoj se crtaju i definisu stvari, van toga ne bi trebalo ista da se nalazi!
namespace App {
    class App {
        public:
        App(){
            //TODO: read from a json file, everything 
            window = new Engine::Resources::Window(800,600,"test");
            window->init();   
        }

        ~App(){
            delete window;
        }

        void run(){
            GLFWwindow* ref = window->return_ref();

            Engine::Resources::Shader* vertexShader = new Engine::Resources::Shader("../resources/shaders/vertex/vert1.glsl", Engine::Resources::VERTEX_SHADER);
            Engine::Resources::Shader* fragmentShader = new Engine::Resources::Shader("../resources/shaders/fragment/frag1.glsl", Engine::Resources::FRAGMENT_SHADER);
            std::vector<Engine::Resources::Shader*> shaders = {vertexShader, fragmentShader};
            Engine::Resources::ShaderProgram* shaderProgramObj = new Engine::Resources::ShaderProgram(shaders);
            

            unsigned int VBO,VAO,EBO;

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

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
            glEnableVertexAttribArray(0);            
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindVertexArray(0);

            Engine::Camera::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
            glfwSetInputMode(ref, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // Vreme za kalkulaciju brzine
            float deltaTime = 0.0f;
            float lastFrame = 0.0f;


            double lastX = 400.0; // Center of your 800x600 window
            double lastY = 300.0;
            bool firstMouse = true;

            bool transparent_flag = true;

            while (!glfwWindowShouldClose(ref))
            {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                double xpos, ypos;
                glfwGetCursorPos(ref, &xpos, &ypos); // Get the current mouse position

                if (firstMouse) {
                    lastX = xpos;
                    lastY = ypos;
                    firstMouse = false;
                }

                float xoffset = xpos - lastX;
                float yoffset = lastY - ypos; // Reversed since Y-coordinates go from bottom to top

                lastX = xpos;
                lastY = ypos;

                
                camera.processMouseMovement(xoffset, yoffset);

                Engine::Platform::processInput(ref);

                if (glfwGetKey(ref, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(Engine::Camera::CameraMovement::FORWARD, deltaTime);
                if (glfwGetKey(ref, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(Engine::Camera::CameraMovement::BACKWARD, deltaTime);
                if (glfwGetKey(ref, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(Engine::Camera::CameraMovement::LEFT, deltaTime);
                if (glfwGetKey(ref, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(Engine::Camera::CameraMovement::RIGHT, deltaTime);
                if (glfwGetKey(ref, GLFW_KEY_SPACE) == GLFW_PRESS) camera.returnToOrigin();
                if (glfwGetKey(ref, GLFW_KEY_T) == GLFW_PRESS) transparent_flag = !transparent_flag;
                // Taster za mijenjanje geometrije (Dodajte logiku da se okine samo jednom po pritisku)
                if (glfwGetKey(ref, GLFW_KEY_G) == GLFW_PRESS) camera.toggleGeometry();


                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                
                shaderProgramObj->use();
                glm::mat4 view = camera.getViewMatrix();
                glm::mat4 projection = camera.getProjectionMatrix(800.0f, 600.0f);
                glm::mat4 model = glm::mat4(1.0f); // Kocka stoji na centru (0,0,0)
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f)); // Rotacija kocke kroz vreme
                model = glm::scale(model, glm::vec3(2*glm::cos((float)glfwGetTime()), 2*glm::cos((float)glfwGetTime()), 2*glm::cos((float)glfwGetTime()))); // Pulsiranje kocke kroz vreme

                // Slanje matrica u shader
                glUniformMatrix4fv(glGetUniformLocation(shaderProgramObj->getID(), "uModel"), 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgramObj->getID(), "uView"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgramObj->getID(), "uProj"), 1, GL_FALSE, glm::value_ptr(projection));

                // Slanje geometry flag-a
                glUniform1i(glGetUniformLocation(shaderProgramObj->getID(), "uIsHyperbolic"), camera.getIsHyperbolic());
                glBindVertexArray(VAO);
                
                if (transparent_flag == true)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }else{
                    glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
                }
                
                
                glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)sizeof(solidIndices));

                glfwSwapBuffers(ref);
                glfwPollEvents();
            }

            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            delete shaderProgramObj;

            glfwTerminate();
        }
        private:
        Engine::Resources::Window* window;

        // 8 Vertices of a standard 1x1x1 cube
        float cubeVertices[6*24] = {
            // Front face (Normal: 0.0f, 0.0f, 1.0f)
            -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 0: Bottom-Left
            0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 1: Bottom-Right
            0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 2: Top-Right
            -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f, // 3: Top-Left

            // Back face (Normal: 0.0f, 0.0f, -1.0f)
            0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f, // 4: Bottom-Left (looking from back)
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
            0,  1,  2,  2,  3,  0,  // Front
            4,  5,  6,  6,  7,  4,  // Back
            8,  9,  10, 10, 11, 8,  // Left
            12, 13, 14, 14, 15, 12, // Right
            16, 17, 18, 18, 19, 16, // Top
            20, 21, 22, 22, 23, 20  // Bottom
        };

        unsigned int lineIndices[24] = {
            0, 1, 1, 2, 2, 3, 3, 0,    // Front face edges
            4, 5, 5, 6, 6, 7, 7, 4,    // Back face edges
            0, 4, 1, 5, 2, 6, 3, 7     // Connection edges
        };
    };    
}


#endif // __APP_HPP