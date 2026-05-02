#ifndef __AABB_HPP
#define __AABB_HPP

#include <glm/glm.hpp>

namespace Engine::Scene {
    struct AABB {
        glm::vec3 center;
        glm::vec3 halfExtent;

        bool contains(const glm::vec3& point) const {
            return (point.x >= center.x - halfExtent.x && point.x <= center.x + halfExtent.x &&
                    point.y >= center.y - halfExtent.y && point.y <= center.y + halfExtent.y &&
                    point.z >= center.z - halfExtent.z && point.z <= center.z + halfExtent.z);
        }

        bool intersects(const AABB& other) const {
            return (std::abs(center.x - other.center.x) <= (halfExtent.x + other.halfExtent.x) &&
                    std::abs(center.y - other.center.y) <= (halfExtent.y + other.halfExtent.y) &&
                    std::abs(center.z - other.center.z) <= (halfExtent.z + other.halfExtent.z));
        }
    };
}
#endif