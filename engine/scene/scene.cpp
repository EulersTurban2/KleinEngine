#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "renderer/renderer.hpp"

namespace Engine::Scene {

    Scene::Scene(Camera& cam) : mCamera(cam), mSpatialIndex(nullptr) {}

    Entity Scene::createEntity(const std::string& name) {
        uint32_t handle = mRegistry.createEntity();
        Entity entity(handle, this);

        entity.addComponent<TransformComponent>();
        entity.addComponent<TagComponent>(TagComponent{name});

        if (mSpatialIndex) {
            mSpatialIndex->insert(handle, glm::vec3(0.0f));
        }

        return entity;
    }

    void Scene::setSpatialIndex(std::unique_ptr<SpatialIndex> index) {
        mSpatialIndex = std::move(index);

        if (mSpatialIndex) {
            mSpatialIndex->clear();

            auto entities = mRegistry.view<TransformComponent>();
            for (uint32_t id : entities) {
                auto& transform = mRegistry.getComponent<TransformComponent>(id);
                mSpatialIndex->insert(id, transform.position);
            }
        }
    }

    void Scene::update(float deltaTime) {
        auto scripts = mRegistry.view<NativeScriptComponent>();
        for (uint32_t id : scripts) {
            auto& script = mRegistry.getComponent<NativeScriptComponent>(id);
            if (script.onUpdate) {
                Entity entity(id, this);
                script.onUpdate(entity, deltaTime);
            }
        }

        if (mSpatialIndex) {
            auto entities = mRegistry.view<TransformComponent>();
            for (uint32_t id : entities) {
                auto& transform = mRegistry.getComponent<TransformComponent>(id);

                mSpatialIndex->update(id, transform.position);
            }
        }
    }

    void Scene::draw(float screenWidth, float screenHeight) {
        Engine::Renderer::Renderer::beginScene(mCamera, screenWidth, screenHeight);

        auto renderables = mRegistry.view<MeshRendererComponent>();

        for (uint32_t id : renderables) {
            if (mRegistry.hasComponent<TransformComponent>(id)) {

                auto& mesh = mRegistry.getComponent<MeshRendererComponent>(id);
                auto& transform = mRegistry.getComponent<TransformComponent>(id);

                if (mesh.model && mesh.material) {
                    Engine::Renderer::Renderer::submit(mesh.model, mesh.material, transform.getModelMatrix());
                }
            }
        }

        Engine::Renderer::Renderer::endScene();
    }
}
