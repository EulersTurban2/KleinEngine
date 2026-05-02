#include "scene/scene.hpp"
#include "renderer/material.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Scene {

    Scene::Scene(const Camera::Camera& cam) : mCamera(cam), mSpatialIndex(nullptr) {}

    void Scene::addEntity(std::unique_ptr<Entity> entity) {
        if (mSpatialIndex) {
            mSpatialIndex->insert(entity.get());
        }
        
        mEntities.push_back(std::move(entity));
    }

    void Scene::setSpatialIndex(std::unique_ptr<SpatialIndex> index) {
        mSpatialIndex = std::move(index);
        
        if (mSpatialIndex) {
            mSpatialIndex->clear();
            for (auto& ent : mEntities) {
                mSpatialIndex->insert(ent.get());
            }
        }
    }

    void Scene::update(float deltaTime) {
        for (auto& ent : mEntities) {
            if (ent->onUpdate) {
                ent->onUpdate(*ent, deltaTime);
            }

            if (mSpatialIndex) {
                mSpatialIndex->update(ent.get());
            }
        }
    }

    void Scene::draw(float screenWidth, float screenHeight) {
        glm::mat4 view = mCamera.getViewMatrix();
        glm::mat4 projection = mCamera.getProjectionMatrix(screenWidth, screenHeight);
        bool isHyp = mCamera.getIsHyperbolic(); 

        for (const auto& ent : mEntities) {
            if (!ent->model || !ent->material) continue;

            auto shader = ent->material->getShader();
            
            ent->material->apply();
            shader->use();

            shader->setMat4("uView", view);
            shader->setMat4("uProj", projection);
            shader->setBool("uIsHyperbolic", isHyp);

            shader->setMat4("uModel", ent->transform.getModelMatrix());

            ent->model->draw(*shader);
        }
    }
}