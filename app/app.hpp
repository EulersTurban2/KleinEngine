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
#include <cmath>
#include <stb_image.h>

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
            glfwSetInputMode(m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            auto& loader = Engine::Resources::ModelLoader::get();

            // Load resources from JSON
            loader.loadFile("../resources/resources.json", "app");

            // --- Entity Orchestration ---
            Engine::Camera::Camera rCamera;
            Engine::Scene::Scene mainScene(rCamera);

            // 1. Setup Standard Cubes using fetchEntity
            std::vector<glm::vec3> startPositions = {
                { 0.0f,  0.0f,  0.0f},   
                { 2.0f,  2.0f, -15.0f},   
                {-1.0f, -1.0f,  0.0f},   
                {-3.8f, -2.0f, -12.3f}   
            };

            for (size_t i = 0; i < startPositions.size(); i++) {
                
                Engine::Scene::Entity cubeInstance = loader.instantiate("cube");
                cubeInstance.name = "Cube_" + std::to_string(i);
                cubeInstance.transform.position = startPositions[i];
                
                if (i == 0) {
                    cubeInstance.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.position.y = glm::sin(glfwGetTime()) * 2.0f;
                        self.transform.rotation.y += 45.0f * dt; 
                    };
                } else if (i == 1) {
                    cubeInstance.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.rotation.x += 200.0f * dt;
                        self.transform.rotation.z += 3000.0f * dt;
                    };
                } else if (i == 2) {
                    cubeInstance.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        float scale = 1.0f + 0.5f * glm::sin(glfwGetTime() * 3.0f);
                        self.transform.scale = glm::vec3(scale);
                    };
                }
                
                mainScene.addEntity(cubeInstance);
            }

            // 2. Setup Light Source
            auto lightTemplate = loader.fetchEntity("lightSource");
            if (lightTemplate) {
                Engine::Scene::Entity lightInstance = *lightTemplate;
                lightInstance.name = "MainLight";
                lightInstance.transform.position = { 0.0f, 0.0f, -1.0f };
                
                lightInstance.onUpdate = [](Engine::Scene::Entity& self, float dt) {
                    self.transform.rotation.x += 30.0f * dt;
                    self.transform.rotation.z += 30.0f * dt;
                };
                mainScene.addEntity(lightInstance);
            }

            float deltaTime = 0.0f;
            float lastFrame = 0.0f;
            bool transparent_flag = false;
            glEnable(GL_DEPTH_TEST);

            // --- MAIN LOOP ---
            while (!m_window->shouldClose()) {
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                
                INPUT::Update();
                m_window->onUpdate();

                glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                auto& cam = mainScene.getCamera();
                cam.processMouseMovement(INPUT::getMouseDelta().x, INPUT::getMouseDelta().y);

                // Handle Camera Input
                if (INPUT::isKeyDown(GLFW_KEY_W)) cam.processKeyboard(Engine::Camera::CameraMovement::FORWARD, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_S)) cam.processKeyboard(Engine::Camera::CameraMovement::BACKWARD, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_A)) cam.processKeyboard(Engine::Camera::CameraMovement::LEFT, deltaTime);
                if (INPUT::isKeyDown(GLFW_KEY_D)) cam.processKeyboard(Engine::Camera::CameraMovement::RIGHT, deltaTime);

                if (INPUT::isKeyPressed(GLFW_KEY_T)) transparent_flag = !transparent_flag;
                if (INPUT::isKeyPressed(GLFW_KEY_G)) cam.toggleGeometry();
                if (INPUT::isKeyPressed(GLFW_KEY_SPACE)) cam.reset();

                mainScene.update(deltaTime);

                
                auto defaultMat = loader.fetchMaterial("default");
                if (defaultMat) {
                    auto shader = defaultMat->getShader(); 
                    shader->use(); 

                    glm::mat4 view = cam.getViewMatrix();
                    glm::vec4 lightWorldPos;
                    glm::vec3 lightOrigin = { 0.0f, 0.0f, -1.0f };
                    
                    if (cam.getIsHyperbolic()) {
                        float w = std::sqrt(1.0f + glm::dot(lightOrigin, lightOrigin)); 
                        lightWorldPos = glm::vec4(lightOrigin, w);
                    } else {
                        lightWorldPos = glm::vec4(lightOrigin, 1.0f);
                    }
                    
                    glm::vec4 lightViewPos = view * lightWorldPos; 
                    shader->setVec4("uLightPos", lightViewPos);
                    shader->setFloat("uLightRadius", 40.0f);
                    shader->setVec3("uLightColor", glm::vec3(1.0f, 0.2f, 0.2f)); 
                    
                    glm::vec3 camPos = cam.getPosition();
                    float w = std::sqrt(1.0f + glm::dot(camPos, camPos));
                    shader->setVec4("cameraPos", glm::vec4(camPos, w));
                }

                if (transparent_flag) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                mainScene.draw(m_window->getWidth(), m_window->getHeight());
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

    private:
        Engine::Core::Window* m_window;
    };    
}

#endif