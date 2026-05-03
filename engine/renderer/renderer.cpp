#include "renderer/renderer.hpp"
#include "core/timer.hpp"

namespace Engine::Renderer {
    Renderer::SceneData Renderer::sSceneData;
    std::vector<RenderCommand> Renderer::sOpaqueCommands;
    std::vector<RenderCommand> Renderer::sTransparentCommands;

    void Renderer::init() {
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::beginScene(const Camera::Camera& camera, const float screenWidth, const float screenHeight) {
        sSceneData.viewProjectionMatrix = camera.getViewMatrix();
        sSceneData.projectionMatrix = camera.getProjectionMatrix(screenWidth, screenHeight);
        sSceneData.cameraPosition = camera.getPosition();
        sSceneData.isHyperbolic = camera.getIsHyperbolic();
        sSceneData.cam = &camera;

        sOpaqueCommands.clear();
        sTransparentCommands.clear();
    }

    void Renderer::submit(std::shared_ptr<Model> model, std::shared_ptr<Material> material, const glm::mat4& transformMatrix) {
        if(!model || !material) return;

        RenderCommand command{ model, material, transformMatrix };
        
        command.distanceToCamera = glm::distance(sSceneData.cameraPosition, glm::vec3(transformMatrix[3])); 
        sOpaqueCommands.push_back(command);
    }

    void Renderer::endScene() {
        flush();
        // post-processing goes here
        sOpaqueCommands.clear();
        sTransparentCommands.clear();
    }

    void Renderer::flush() {

        RenderContext ctx {
            *sSceneData.cam,
            (float)glfwGetTime(),
            Engine::Core::Timer::getDeltaTime()
        };

        std::sort(sOpaqueCommands.begin(), sOpaqueCommands.end(), [](const RenderCommand& a, const RenderCommand& b) {
            return a.distanceToCamera < b.distanceToCamera;
        });

        for (const auto& cmd : sOpaqueCommands) {
            auto shader = cmd.material->getShader();
            cmd.material->apply();
            shader->use();
            shader->setMat4("uView", sSceneData.viewProjectionMatrix);
            shader->setMat4("uProj", sSceneData.projectionMatrix);
            shader->setBool("uIsHyperbolic", sSceneData.isHyperbolic);
            shader->setMat4("uModel", cmd.transformMatrix);

            if (cmd.material->onUse != nullptr && sSceneData.cam != nullptr)
            {
                cmd.material->onUse(shader, ctx);
            }
            cmd.model->draw(*shader);
        }
    }

}