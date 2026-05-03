#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "renderer/renderer.hpp"

namespace Engine::Scene {

    Scene::Scene(Camera::Camera& cam) : mCamera(cam), mSpatialIndex(nullptr) {}

    Entity Scene::createEntity(const std::string& name) {
        uint32_t handle = m_Registry.createEntity();
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
            
            auto entities = m_Registry.view<TransformComponent>();
            for (uint32_t id : entities) {
                auto& transform = m_Registry.getComponent<TransformComponent>(id);
                mSpatialIndex->insert(id, transform.position);
            }
        }
    }

    void Scene::update(float deltaTime) {

        auto scripts = m_Registry.view<NativeScriptComponent>();
        for (uint32_t id : scripts) {
            auto& script = m_Registry.getComponent<NativeScriptComponent>(id);
            if (script.onUpdate) {
                Entity entity(id, this);
                script.onUpdate(entity, deltaTime);
            }
        }

        if (mSpatialIndex) {
            auto entities = m_Registry.view<TransformComponent>();
            for (uint32_t id : entities) {
                auto& transform = m_Registry.getComponent<TransformComponent>(id);
                
                mSpatialIndex->update(id, transform.position); 
            }
        }
    }

    void Scene::draw(float screenWidth, float screenHeight) {
        Engine::Renderer::Renderer::beginScene(mCamera, screenWidth, screenHeight);

        auto renderables = m_Registry.view<MeshRendererComponent>();

        for (uint32_t id : renderables) {
            if (m_Registry.hasComponent<TransformComponent>(id)) {
                
                auto& mesh = m_Registry.getComponent<MeshRendererComponent>(id);
                auto& transform = m_Registry.getComponent<TransformComponent>(id);

                if (mesh.model && mesh.material) {
                    Engine::Renderer::Renderer::submit(mesh.model, mesh.material, transform.getModelMatrix());
                }
            }
        }

        Engine::Renderer::Renderer::endScene();
    }
}
