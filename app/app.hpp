#ifndef APP_HPP
#define APP_HPP

#include "all.hpp"
#include "ui_panels/inspector.hpp"

#include <cmath>
#include <cstdlib>
#include <memory>
#include <vector>

namespace App {

    class App {
    public:
        App() {
            // Configuration must be loaded before the window exists so that
            // window-related settings (e.g. FULLSCREEN) can take effect.
            loadEnvironment("engine.json");

            bool fullscreen = env.getBool("FULLSCREEN", false);
            mWindow = std::make_unique<Engine::Core::Window>(800, 600, "Klein Engine - ECS Powered", false, fullscreen);
            if (!mWindow->init()) {
                LOG_CRITICAL("Failed to initialize application window.");
                std::exit(EXIT_FAILURE);
            }
        }

        ~App() {
            entityLoader.clearCache();
        }

        void run() {
            initManagers();
            Camera rCamera;
            Scene mainScene(rCamera);
            Keymap cameraKeymap;
            cameraKeymap.bind("camera.forward", GLFW_KEY_W);
            cameraKeymap.bind("camera.backward", GLFW_KEY_S);
            cameraKeymap.bind("camera.left", GLFW_KEY_A);
            cameraKeymap.bind("camera.right", GLFW_KEY_D);
            cameraKeymap.bind("camera.up", GLFW_KEY_E);
            cameraKeymap.bind("camera.down", GLFW_KEY_Q);
            cameraKeymap.bind("camera.toggle_geometry", GLFW_KEY_G);
            cameraKeymap.bind("camera.reset", GLFW_KEY_SPACE);

            BoundingBox worldBounds{{0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}};
            mainScene.setSpatialIndex(std::make_unique<Octree>(worldBounds, 8));
            initObjects(mainScene);

            auto& cam = mainScene.getCamera();
            auto mainFramebuffer = std::make_shared<Framebuffer>(
                mWindow->getWidth(),
                mWindow->getHeight()
            );
            mainFramebuffer->cacheBuffer("mainScene", TextureType::Albedo);

            glm::vec3 clearColor(0.1f, 0.1f, 0.15f);
            auto background = env.getFloatArray("BACKGROUND");
            if (background.size() >= 3) {
                clearColor = glm::vec3(background[0], background[1], background[2]);
            }

            // Preview for UI panel
            auto previewFramebuffer = std::make_shared<Framebuffer>(512, 512);
            std::shared_ptr<Material> previewMat = nullptr;
            std::shared_ptr<Model> previewModel = nullptr;

            Camera previewCam;
            glm::vec3 previewColor = glm::vec3(1.0f);

            glm::vec3 previewPosition = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 previewRotation = glm::vec3(0.0f); // Euler angles in degrees
            glm::vec3 previewScale    = glm::vec3(1.0f, 1.0f, 1.0f);

            HyperbolicProjection currentProjection = HyperbolicProjection::Klein;

            bool isUIActive = false;
            glfwSetInputMode(mWindow->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            while (!mWindow->shouldClose()) {
                Timer::update();
                float deltaTime = Timer::getDeltaTime();

                Input::update();
                if (Input::isKeyPressed(GLFW_KEY_TAB)) {
                    isUIActive = !isUIActive;

                    if (isUIActive) {
                        glfwSetInputMode(mWindow->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    } else {
                        glfwSetInputMode(mWindow->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    }
                }
                UIManager::begin();
                if (!isUIActive) {
                    cam.handleInput(deltaTime, cameraKeymap);
                }

                previewFramebuffer->bind();
                glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(previewRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(previewRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(previewRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                modelMatrix = glm::scale(modelMatrix, previewScale);

                if (previewMat) {
                    previewMat->onUse = [previewColor](std::shared_ptr<ShaderProgram> shader, const RenderContext& ctx) {
                        glm::vec4 lightWorldPos  = glm::vec4(0.0f, 2.0f, 5.0f, 1.0f);
                        glm::mat4 view           = ctx.cam.getViewMatrix();
                        glm::vec4 lightViewPos   = view * lightWorldPos;

                        shader->setVec4("uLightPos", lightViewPos);
                        shader->setVec3("uLightColor", previewColor);
                        shader->setFloat("uLightRadius", 1000.0f);
                    };
                }

                previewCam.reset();

                Renderer::beginScene(previewCam, 512, 512);
                if (previewModel && previewMat) {
                    Renderer::submit(previewModel, previewMat, modelMatrix);
                }
                Renderer::endScene();
                previewFramebuffer->unbind();

                mainFramebuffer->bind();
                glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                if (!isUIActive) {
                    mainScene.update(deltaTime);
                }

                // Feed the UI-selected projection to the renderer so the vertex
                // shaders flatten the hyperboloid with the matching model.
                Renderer::setProjectionModel(currentProjection);
                mainScene.draw(mWindow->getWidth(), mWindow->getHeight());

                mainFramebuffer->unbind();
                mainFramebuffer->resolveToScreen(mWindow->getWidth(), mWindow->getHeight());
                mainFramebuffer->handleResize(mWindow->getWidth(), mWindow->getHeight());
                if (isUIActive) {
                    InspectorPanel::onImGuiRender(currentProjection, mainScene,
                                                  previewFramebuffer->getColorTexture()->getRendererID(),
                                                  previewColor, previewPosition, previewRotation, previewScale, previewModel, previewMat);
                }
                UIManager::end();
                mWindow->onUpdate();
            }
        }

    private:
        void loadEnvironment(const std::string& path) {
            env.loadConfig(path);
            env.bootResourceDatabase();
        }

        void initManagers() {
            GLFWwindow* nativeWin = mWindow->getNativeWindow();
            Input::init(nativeWin);
            Renderer::init();
            Timer::init();
            UIManager::init(nativeWin);
        }

        void initObjects(Scene& mainScene) {
            std::vector<glm::vec3> startPositions = {
                { 0.0f,  0.0f,   0.0f},
                { 2.0f,  2.0f, -15.0f},
                {-1.0f, -1.0f,   0.0f},
                {-3.8f, -2.0f, -12.3f}
            };

            for (size_t i = 0; i < startPositions.size(); i++) {
                auto cube = entityLoader.instantiate("cube", mainScene);

                auto& transform = cube.getComponent<TransformComponent>();
                transform.position = startPositions[i];

                auto& tag = cube.getComponent<TagComponent>();
                tag.name = "Cube_" + std::to_string(i);

                if (i == 0) {
                    cube.addComponent<NativeScriptComponent>(
                        NativeScriptComponent{
                            [](Entity& self, float dt) {
                                auto& t = self.getComponent<TransformComponent>();
                                t.position.y = glm::sin(glfwGetTime()) * 2.0f;
                                t.rotation.y += 45.0f * dt;
                            }
                        }
                    );
                } else if (i == 1) {
                    cube.addComponent<NativeScriptComponent>(
                        NativeScriptComponent{
                            [](Entity& self, float dt) {
                                auto& t = self.getComponent<TransformComponent>();
                                t.rotation.x += 200.0f * dt;
                                t.rotation.z += 3000.0f * dt;
                            }
                        }
                    );
                } else if (i == 2) {
                    cube.addComponent<NativeScriptComponent>(
                        NativeScriptComponent{
                            [](Entity& self, float /*dt*/) {
                                auto& t = self.getComponent<TransformComponent>();
                                t.scale.x = glm::sin(glfwGetTime()) * 2.0f;
                                t.scale.z = glm::sin(glfwGetTime()) * 2.0f;
                            }
                        }
                    );
                }
            }

            auto lightInstance = entityLoader.instantiate("lightSource", mainScene);
            {
                auto& transform = lightInstance.getComponent<TransformComponent>();
                transform.position = { 0.0f, 0.0f, -1.0f };

                auto& tag = lightInstance.getComponent<TagComponent>();
                tag.name = "MainLight";

                lightInstance.addComponent<NativeScriptComponent>(
                    NativeScriptComponent{
                        [](Entity& self, float dt) {
                            auto& t = self.getComponent<TransformComponent>();
                            t.rotation.x += 30.0f * dt;
                        }
                    }
                );
            }

            // Scene lighting is app-driven for now: the "default" material gets
            // its light position/color from this hook every frame. The light
            // source's own look (uLightSourceSpec) comes from resources.json.
            auto defaultMat = entityLoader.getMaterial("default");
            if (defaultMat) {
                defaultMat->onUse = [](std::shared_ptr<ShaderProgram> shader, const RenderContext& ctx) {
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
        }

        std::unique_ptr<Engine::Core::Window> mWindow;
    };
}

#endif // APP_HPP
