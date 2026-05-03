#ifndef __SCENE_HPP
#define __SCENE_HPP

#include <memory>
#include <string>
#include "scene/camera.hpp"
#include "scene/registry.hpp"
#include "scene/spatial_index.hpp"

namespace Engine::Scene {

    class Entity; 

    class Scene {
    public:
        explicit Scene(Camera::Camera& cam);
        ~Scene() = default;

        Entity createEntity(const std::string& name = "Entity");

        void update(float deltaTime);
        void draw(float screenWidth, float screenHeight);

        void setSpatialIndex(std::unique_ptr<SpatialIndex> index);
        SpatialIndex* getSpatialIndex() const { return mSpatialIndex.get(); }

        Registry& getRegistry() { return m_Registry; }
        Camera::Camera& getCamera() { return mCamera; }

    private:
        Camera::Camera& mCamera;
        Registry m_Registry;
        std::unique_ptr<SpatialIndex> mSpatialIndex;
    };
}

#endif