#ifndef __SCENE_HPP
#define __SCENE_HPP

#include <vector>
#include <memory>

#include "scene/camera.hpp"
#include "scene/entity.hpp"
#include "scene/spatial_index.hpp"
#include "scene/aabb.hpp"

namespace Engine::Scene {

    class Scene {
    public:
        explicit Scene(const Camera::Camera& cam);
        ~Scene() = default;

        void update(float deltaTime);
        void draw(float screenWidth, float screenHeight);

        void addEntity(std::unique_ptr<Entity> entity);
        
        void setSpatialIndex(std::unique_ptr<SpatialIndex> index);
        SpatialIndex* getSpatialIndex() const { return mSpatialIndex.get(); }

        Camera::Camera& getCamera() { return mCamera; }

    private:
        Camera::Camera mCamera;
        
        std::vector<std::unique_ptr<Entity>> mEntities;
        std::unique_ptr<SpatialIndex> mSpatialIndex;
    };
}

#endif