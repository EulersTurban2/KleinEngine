#ifndef __RENDERER_HPP
#define __RENDERER_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "scene/camera.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"

namespace Engine::Renderer {
    struct RenderCommand {
        std::shared_ptr<Model> model;
        std::shared_ptr<Material> material;
        glm::mat4 transformMatrix;

        float distanceToCamera = 0.0f;
    };

    class Renderer {
        public:
            static void init();
            static void beginScene(const Camera::Camera& camera, const float screenWidth, const float screenHeight);
            static void submit(std::shared_ptr<Model> model, std::shared_ptr<Material> material, const glm::mat4& transformMatrix);
            static void endScene();
        private:

            struct SceneData {
                glm::mat4 projectionMatrix;
                glm::mat4 viewProjectionMatrix;
                glm::vec3 cameraPosition;
                bool isHyperbolic;
                const Engine::Camera::Camera* cam;
            };

            static void flush();

            static SceneData sSceneData;

            static std::vector<RenderCommand> sOpaqueCommands;
            static std::vector<RenderCommand> sTransparentCommands;
    };
}

#endif