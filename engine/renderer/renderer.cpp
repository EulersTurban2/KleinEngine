#include "renderer/renderer.hpp"
#include "core/timer.hpp"

#include <algorithm>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine::Renderer {

    Renderer::SceneData Renderer::sSceneData;
    Engine::Math::HyperbolicProjection Renderer::sProjectionModel = Engine::Math::HyperbolicProjection::Klein;
    std::vector<RenderCommand> Renderer::sOpaqueCommands;
    std::vector<RenderCommand> Renderer::sTransparentCommands;

    void Renderer::init() {
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::setProjectionModel(Engine::Math::HyperbolicProjection model) {
        sProjectionModel = model;
    }

    void Renderer::beginScene(const Scene::Camera& camera, float screenWidth, float screenHeight) {
        sSceneData.viewMatrix = camera.getViewMatrix();
        sSceneData.projectionMatrix = camera.getProjectionMatrix(screenWidth, screenHeight);
        sSceneData.cameraPosition = camera.getPosition();
        sSceneData.isHyperbolic = camera.getIsHyperbolic();
        sSceneData.cam = &camera;
    }

    void Renderer::submit(std::shared_ptr<Model> model, std::shared_ptr<Material> material, const glm::mat4& transformMatrix) {
        if (!model || !material) return;

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
        if (sSceneData.cam == nullptr) return;

        RenderContext ctx{
            *sSceneData.cam,
            static_cast<float>(glfwGetTime()),
            Engine::Core::Timer::getDeltaTime()
        };

        // Opaque geometry is sorted front to back to reduce overdraw.
        std::sort(sOpaqueCommands.begin(), sOpaqueCommands.end(), [](const RenderCommand& a, const RenderCommand& b) {
            return a.distanceToCamera < b.distanceToCamera;
        });

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        for (const auto& cmd : sOpaqueCommands) {
            auto shader = cmd.material->getShader();
            cmd.material->apply(); // binds the shader and sets material uniforms
            shader->setMat4("uView", sSceneData.viewMatrix);
            shader->setMat4("uProj", sSceneData.projectionMatrix);
            shader->setBool("uIsHyperbolic", sSceneData.isHyperbolic);
            shader->setInt("uProjectionModel", static_cast<int>(sProjectionModel));
            shader->setMat4("uModel", cmd.transformMatrix);

            if (cmd.material->onUse != nullptr) {
                cmd.material->onUse(shader, ctx);
            }
            cmd.model->draw();
        }
    }

    void Renderer::renderFullscreenQuad() {
        static uint32_t quadVAO = 0;
        if (quadVAO == 0) {
            float quadVertices[] = {
                // positions (x, y) | texture coords (u, v)
                -1.0f,  1.0f,  0.0f, 1.0f,
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 1.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
            };
            uint32_t quadVBO;
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0); // Position
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1); // TexCoords
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}
