#ifndef __ENTITY_HPP
#define __ENTITY_HPP

#include <cstdint>
#include <utility>
#include "registry.hpp"
#include "scene/scene.hpp"

namespace Engine::Scene {

    class Scene; 

    class Entity {
    public:
        Entity() = default;
        Entity(uint32_t handle, Scene* scene) : mEntityHandle(handle), mScene(scene) {}

        template<typename T, typename... Args>
        T& addComponent(Args&&... args){
            mScene->getRegistry().addComponent<T>(mEntityHandle, T{std::forward<Args>(args)...});
            return getComponent<T>();
        }

        template<typename T>
        T& getComponent() {
            return mScene->getRegistry().getComponent<T>(mEntityHandle);
        }

        template<typename T>
        bool hasComponent() {
            return mScene->getRegistry().hasComponent<T>(mEntityHandle);
        }

        uint32_t getId() const { return mEntityHandle; }

    private:
        uint32_t mEntityHandle = 0; 
        Scene* mScene = nullptr;
    };
}

#endif 