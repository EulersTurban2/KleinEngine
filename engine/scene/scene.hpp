#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>
#include <string>
#include "scene/camera.hpp"
#include "scene/registry.hpp"
#include "scene/spatial_index.hpp"

namespace Engine::Scene {

    class Entity;

    class Scene {
    public:
        explicit Scene(Camera& cam);
        ~Scene() = default;

        Entity createEntity(const std::string& name = "Entity");

        void update(float deltaTime);
        void draw(float screenWidth, float screenHeight);

        void setSpatialIndex(std::unique_ptr<SpatialIndex> index);
        SpatialIndex* getSpatialIndex() const { return mSpatialIndex.get(); }

        Registry& getRegistry() { return mRegistry; }
        Camera& getCamera() { return mCamera; }

    private:
        Camera& mCamera;
        Registry mRegistry;
        std::unique_ptr<SpatialIndex> mSpatialIndex;
    };
}

#endif // SCENE_HPP
