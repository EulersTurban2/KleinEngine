#ifndef __APP_HPP
#define __APP_HPP

#include "core/window.hpp"
#include "core/platform.hpp"
#include "core/input_manager.hpp"

#include "scene/camera.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"

#include "math/lorentz.hpp"

#include "resources/model_loader.hpp" 

#include <glm/gtc/type_ptr.hpp>
#include <vector>

using INPUT = Engine::Core::InputManager;

namespace App {
    class App {
    public:
        App() {
            m_window = new Engine::Core::Window(800, 600, "Klein Engine - Hyperbolic Sandbox");
            if (m_window->init() != true) {
                LOG_CRITICAL("Failed to initialize application window.");
            }
        }

        ~App() {
            Engine::Resources::ModelLoader::get().cleanUp();
            delete m_window;
        }

        void run() {
            GLFWwindow* nativeWin = m_window->getNativeWindow();
            INPUT::init(nativeWin);
            glfwSetInputMode(nativeWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // 1. Load Assets into Cache
            Engine::Resources::ModelLoader::get().loadFile("../resources/resources.json", "app");
            auto cubeModel = Engine::Resources::ModelLoader::get().genModel("cube");
            auto cubeShader = Engine::Resources::ModelLoader::get().createShaderProgram("cube");

            if (!cubeModel || !cubeShader) {
                LOG_CRITICAL("Core assets failed to load. Exiting.");
                return;
            }

            // 2. Create the Scene and populate it
            Engine::Camera::Camera rCamera;
            Engine::Scene::Scene mainScene(rCamera);

            std::vector<glm::vec3> startPositions = {
                { 0.0f,  0.0f,  0.0f},   // Cube 0
                { 2.0f,  5.0f, -15.0f},  // Cube 1
                {-1.5f, -2.2f, -2.5f},   // Cube 2
                {-3.8f, -2.0f, -12.3f}   // Cube 3
            };

            for (int i = 0; i < startPositions.size(); i++) {
                Engine::Scene::Entity newCube;
                newCube.name = "Cube_" + std::to_string(i);
                newCube.model = cubeModel;
                newCube.shader = cubeShader;
                newCube.transform.position = startPositions[i];
                
                if (i == 0) {
                    // Script for Cube 0: Hover up and down smoothly
                    newCube.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.position.y = glm::sin(glfwGetTime()) * 2.0f;
                        self.transform.rotation.y += 45.0f * dt; 
                    };
                } 
                else if (i == 1) {
                    // Script for Cube 1: Spin aggressively out of control
                    newCube.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.rotation.x += 200.0f * dt;
                        self.transform.rotation.z += 3000.0f * dt;
                    };
                }
                else if (i == 2) {
                    // Script for Cube 2: Grow and shrink
                    newCube.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        float scale = 1.0f + 0.5f * glm::sin(glfwGetTime() * 3.0f);
                        self.transform.scale = glm::vec3(scale);
                    };
                }
                
                mainScene.addEntity(newCube);
            }
            
            float deltaTime = 0.0f;
            float lastFrame = 0.0f;
            bool transparent_flag = true;
            
            // --- MAIN LOOP ---
            while (!m_window->shouldClose()) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                
                INPUT::Update();
                m_window->onUpdate();

                // --- Input (Routing directly to the Scene's Camera) ---
                auto& cam = mainScene.getCamera();
                cam.processMouseMovement(INPUT::getMouseDelta().x, INPUT::getMouseDelta().y);

                if (INPUT::isKeyDown(GLFW_KEY_W)) cam.processKeyboard(Engine::Camera::CameraMovement::FORWARD, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_S)) cam.processKeyboard(Engine::Camera::CameraMovement::BACKWARD, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_A)) cam.processKeyboard(Engine::Camera::CameraMovement::LEFT, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_D)) cam.processKeyboard(Engine::Camera::CameraMovement::RIGHT, deltaTime);

                // Toggles
                if (INPUT::isKeyPressed(GLFW_KEY_T)) transparent_flag = !transparent_flag;
                if (INPUT::isKeyPressed(GLFW_KEY_G)) cam.toggleGeometry();
                if (INPUT::isKeyPressed(GLFW_KEY_SPACE)) cam.reset();

                // --- Update Game Logic ---
                // This single line triggers all those custom lambdas you wrote above!
                mainScene.update(deltaTime);

                // --- Rendering ---
                glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                if (transparent_flag) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                // ONE FUNCTION CALL TO DRAW THE ENTIRE WORLD
                mainScene.draw(m_window->getWidth(), m_window->getHeight());
                
                // Reset to fill mode so UI/other elements don't draw as lines later
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

    private:
        Engine::Core::Window* m_window;
    };    
}

#endif