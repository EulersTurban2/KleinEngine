#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "spatial_index.hpp"
#include "aabb.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace Engine::Scene {

    // Entities outside the root boundary are not tracked: insert() drops them
    // and update() re-tries insertion each time it is called.
    class Octree : public SpatialIndex {
    public:
        Octree(const AABB& boundary, std::size_t capacity = 8) : mBoundary(boundary), mCapacity(capacity), mParent(nullptr) {}

        void insert(uint32_t entity, const glm::vec3& position) override;
        void remove(uint32_t entity) override;
        void update(uint32_t entity, const glm::vec3& position) override;
        void clear() override;

        std::vector<uint32_t> query(const glm::vec3& position, float radius) override;
        std::vector<uint32_t> query(const AABB& range);

    private:
        void subdivide();

        Octree* getRoot();
        Octree* getParent() const { return mParent; }

        AABB mBoundary;
        std::size_t mCapacity;
        Octree* mParent;
        bool mDivided = false;

        std::vector<SpatialItem> mItems;
        std::array<std::unique_ptr<Octree>, 8> mChildren;

        // Entity-to-node lookup for the whole tree; only the root's map is used.
        std::unordered_map<uint32_t, Octree*> mEntityNodeMap;
    };
}

#endif // OCTREE_HPP
