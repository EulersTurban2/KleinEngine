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
                auto shader = ent.material->getShader();
                if (!ent.model || !shader) continue;
                
                ent.material->apply();
                shader->use();

                shader->setMat4("uView", view);
                shader->setMat4("uProj", projection);
                shader->setBool("uIsHyperbolic", isHyp);

                shader->setMat4("uModel", ent.transform.getModelMatrix());

                ent.model->draw(*(shader));
            }
            
        }

    }
}