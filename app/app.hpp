#ifndef __APP_HPP
#define __APP_HPP

#include "core/window.hpp"
#include "core/platform.hpp"
#include "core/input_manager.hpp"
#include "core/timer.hpp"
#include "scene/camera.hpp" 
#include "scene/entity.hpp"  
#include "scene/scene.hpp"  
#include "scene/octree.hpp"      
#include "math/lorentz.hpp"
#include "core/enviroment.hpp"
#include "resources/entity_loader.hpp"
#include "resources/material_loader.hpp"
#include "resources/resource_cache.hpp"

// --- New Unified Renderer Include ---
#include "renderer/renderer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <memory>

using INPUT = Engine::Core::InputManager;

namespace App {
    class App {
    public:
        App() {
            // Updated Window Title
            m_window = new Engine::Core::Window(800, 600, "Klein Engine - Unified Renderer");
            if (!m_window->init()) {
                LOG_CRITICAL("Failed to initialize application window."); 
            }
        }

        ~App() {
            Engine::Resources::EntityLoader::getInstance().clearCache();
            delete m_window;
        }

        void run() {
            
            GLFWwindow* nativeWin = m_window->getNativeWindow();
            INPUT::init(nativeWin);
            Engine::Core::Environment::get().loadConfig("engine.json");
            Engine::Core::Environment::get().bootResourceDatabase();

            auto& entityLoader = Engine::Resources::EntityLoader::getInstance();

            Engine::Camera::Camera rCamera;
            Engine::Scene::Scene mainScene(rCamera);

            Engine::Scene::AABB worldBounds{{0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}};
            mainScene.setSpatialIndex(std::make_unique<Engine::Scene::Octree>(worldBounds, 8));

            std::vector<glm::vec3> startPositions = {
                { 0.0f,  0.0f,  0.0f},   
                { 2.0f,  2.0f, -15.0f},   
                {-1.0f, -1.0f,  0.0f},   
                {-3.8f, -2.0f, -12.3f}   
            };

            for (size_t i = 0; i < startPositions.size(); i++) {
                auto cubeInstance = std::make_unique<Engine::Scene::Entity>(entityLoader.instantiate("cube"));
                cubeInstance->name = "Cube_" + std::to_string(i);
                cubeInstance->transform.position = startPositions[i];
                
                if (i == 0) {
                    cubeInstance->onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.position.y = glm::sin(glfwGetTime()) * 2.0f;
                        self.transform.rotation.y += 45.0f * dt; 
                    };
                } else if (i == 1) {
                    cubeInstance->onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.rotation.x += 200.0f * dt;
                        self.transform.rotation.z += 3000.0f * dt;
                    };
                } else if (i == 2) {
                    cubeInstance->onUpdate = [](Engine::Scene::Entity& self, float dt) {
                        self.transform.scale.x = glm::sin(glfwGetTime()) * 2.0f;
                        self.transform.scale.z = glm::sin(glfwGetTime()) * 2.0f;
                    };
                }
                
                mainScene.addEntity(std::move(cubeInstance));
            }

            auto lightTemplate = entityLoader.getEntityTemplate("lightSource");
            if (lightTemplate) {
                auto lightInstance = std::make_unique<Engine::Scene::Entity>(*lightTemplate);
                lightInstance->name = "MainLight";
                lightInstance->transform.position = { 0.0f, 0.0f, -1.0f };
                
                lightInstance->onUpdate = [](Engine::Scene::Entity& self, float dt) {
                    self.transform.rotation.x += 30.0f * dt;
                };
                mainScene.addEntity(std::move(lightInstance));
            }

            // Material on bind callback
            auto& cam = mainScene.getCamera();
            auto defaultMat = Engine::Resources::EntityLoader::getInstance().getMaterial("default");
             if (defaultMat) {
                defaultMat->onUse = [](std::shared_ptr<Engine::Renderer::ShaderProgram> shader, const Engine::Renderer::RenderContext ctx) {
                    glm::mat4 view = ctx.cam.getViewMatrix();
                    glm::vec3 lightOrigin = { 0.0f, 0.0f, -1.0f };
                    glm::vec4 lightWorldPos;
                    
                    if (ctx.cam.getIsHyperbolic()) {
                        float w = std::sqrt(1.0f + glm::dot(lightOrigin, lightOrigin)); 
                        lightWorldPos = glm::vec4(lightOrigin, w);
                    } else {
                        lightWorldPos = glm::vec4(lightOrigin, 1.0f);
                    }
                    
                    shader->setVec4("uLightPos", view * lightWorldPos);
                    shader->setFloat("uLightRadius", 40.0f);
                    shader->setVec3("uLightColor", glm::vec3(1.0f, 0.2f, 0.2f)); 
                };
            }

            Engine::Renderer::Renderer::init();
            Engine::Core::Timer::init();
            while (!m_window->shouldClose()) {
                Engine::Core::Timer::update();
                float deltaTime = Engine::Core::Timer::getDeltaTime();
                
                INPUT::Update();
                m_window->onUpdate();
                
                glClearColor(0.1f, 0.1f, 0.15f, 1.0f); 

                cam.handleInput(deltaTime); 
                mainScene.update(deltaTime);
                mainScene.draw(m_window->getWidth(), m_window->getHeight());
            }
        }

    private:
        Engine::Core::Window* m_window;
    };    
}

#endif