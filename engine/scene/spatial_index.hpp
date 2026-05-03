#ifndef __SPATIAL_INDEX_HPP
#define __SPATIAL_INDEX_HPP

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

namespace Engine::Scene {

    struct SpatialItem {
        uint32_t id;
        glm::vec3 position;
    };

    class SpatialIndex {
    public:
        virtual ~SpatialIndex() = default;
        
        virtual void insert(uint32_t entity, const glm::vec3& position) = 0;
        virtual void remove(uint32_t entity) = 0;
        virtual void update(uint32_t entity, const glm::vec3& position) = 0;
        
        virtual std::vector<uint32_t> query(const glm::vec3& position, float radius) = 0;
        virtual void clear() = 0;
    };
}

#endif