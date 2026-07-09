#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "scene/camera.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "math/projections.hpp"

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
            static void beginScene(const Scene::Camera& camera, float screenWidth, float screenHeight);
            static void submit(std::shared_ptr<Model> model, std::shared_ptr<Material> material, const glm::mat4& transformMatrix);
            static void endScene();

            // Which flattening of the hyperboloid the vertex shaders use.
            // Persists across frames until changed (e.g. from the UI dropdown).
            static void setProjectionModel(Engine::Math::HyperbolicProjection model);

            // Orbit-camera parameters for the half-space view. Exposed live so the
            // UI can tune framing; the hyperbolic origin maps to (0,0,1) and h is up.
            struct HalfSpaceView {
                float distance = 3.0f;      // eye distance from the target
                float azimuth = 0.0f;       // degrees, around the up (h) axis
                float elevation = 13.0f;    // degrees above the horizontal
                float targetHeight = 0.9f;  // height (h) the camera looks at
                float fov = 60.0f;          // vertical field of view, degrees
            };
            static HalfSpaceView& halfSpaceView() { return sHalfSpaceView; }

            // Draws a screen-covering quad; intended for post-processing passes.
            static void renderFullscreenQuad();

        private:
            struct SceneData {
                glm::mat4 viewMatrix;
                glm::mat4 projectionMatrix;
                glm::vec3 cameraPosition;
                bool isHyperbolic = false;
                const Scene::Camera* cam = nullptr;
            };

            static void flush();

            static SceneData sSceneData;
            static Engine::Math::HyperbolicProjection sProjectionModel;
            static HalfSpaceView sHalfSpaceView;

            static std::vector<RenderCommand> sOpaqueCommands;
            // Reserved for the planned transparency pass; nothing submits here yet.
            static std::vector<RenderCommand> sTransparentCommands;
    };
}

#endif // RENDERER_HPP
