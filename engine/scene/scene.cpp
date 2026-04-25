#include "scene/scene.hpp"
#include "scene.hpp"

namespace Engine{
    namespace Scene{
        void Scene::addEntity(const Entity& ent){
            mEntities.push_back(ent);
        }

        Scene::Scene(const Camera::Camera &cam): mCamera(cam) {}

        void Scene::update(float deltaTime){
            for(auto& ent : mEntities){
                if (ent.onUpdate != nullptr)
                    ent.onUpdate(ent,deltaTime);
            }
        }

        void Scene::draw(float screenWidth, float screenHeight){
            glm::mat4 view = mCamera.getViewMatrix();
            glm::mat4 projection = mCamera.getProjectionMatrix(screenWidth,screenHeight);
            bool isHyp = mCamera.getIsHyperbolic();

            for (const auto& ent: mEntities)
            {
                if (!ent.model || !ent.shader) continue;

                ent.shader->use();

                ent.shader->setMat4("uView", view);
                ent.shader->setMat4("uProj", projection);
                ent.shader->setBool("uIsHyperbolic", isHyp);

                ent.shader->setMat4("uModel", ent.transform.getModelMatrix());

                ent.model->draw(*(ent.shader));
            }
            
        }

    }
}