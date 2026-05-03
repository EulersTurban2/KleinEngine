#ifndef __OCTREE_HPP
#define __OCTREE_HPP

#include "spatial_index.hpp"
#include "aabb.hpp"
#include <array>
#include <memory>
#include <cstdint>
#include <unordered_map>

namespace Engine::Scene {

    class Octree : public SpatialIndex {
    public:
        Octree(const AABB& boundary, int capacity = 8) : mBoundary(boundary), mCapacity(capacity), mParent(nullptr) {}

        void insert(uint32_t entity, const glm::vec3& position) override;
        void remove(uint32_t entity) override;
        void update(uint32_t entity, const glm::vec3& position) override;
        void clear() override;

        std::vector<uint32_t> query(const glm::vec3& position, float radius) override;
        std::vector<uint32_t> query(const AABB& range);

    private:
        void subdivide();

        Octree* getRoot();
        Octree* getParent() const { return mParent;}

        AABB mBoundary;
        int mCapacity;
        Octree* mParent;
        bool mDivided = false;

        std::vector<SpatialItem> mItems;
        std::array<std::unique_ptr<Octree>, 8> mChildren;

        static std::unordered_map<uint32_t, Octree*> sEntityNodeMap;
    };
}
#endif