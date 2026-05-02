#ifndef __SPATIAL_INDEX_HPP
#define __SPATIAL_INDEX_HPP

#include <vector>
#include <glm/glm.hpp>
#include "entity.hpp"

namespace Engine::Scene {
    class SpatialIndex {
    public:
        virtual ~SpatialIndex() = default;
        
        virtual void insert(Entity* entity) = 0;
        virtual void remove(Entity* entity) = 0;
        virtual void update(Entity* entity) = 0;
        
        virtual std::vector<Entity*> query(const glm::vec3& position, float radius) = 0;
        virtual void clear() = 0;
    };
}

#endif