#ifndef __SCENE_HPP
#define __SCENE_HPP

#include "renderer/model.hpp"
#include "renderer/shaders.hpp"

#include "scene/camera.hpp"
#include "scene/entity.hpp"

namespace Engine{
    namespace Scene{
        class Scene{
            public:
                Scene(const Camera::Camera&);
                ~Scene() = default;

                void update(float deltaTime); // done
                void draw(float screenWidth, float screenHeight); 

                void addEntity(const Entity& entity); // done

                Camera::Camera& getCamera() {return mCamera;}

            private:
                Camera::Camera mCamera;
                std::vector<Entity> mEntities;
        };
    }
}
#endif