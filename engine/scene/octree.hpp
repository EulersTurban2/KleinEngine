#ifndef __OCTREE_HPP
#define __OCTREE_HPP

#include "spatial_index.hpp"
#include "aabb.hpp"
#include <array>
#include <memory>

namespace Engine::Scene {

    class Octree : public SpatialIndex {
    public:
        Octree(const AABB& boundary, int capacity = 8) : mBoundary(boundary), mCapacity(capacity), mParent(nullptr) {}

        void insert(Entity* entity) override;
        void remove(Entity* entity) override;
        void update(Entity* entity) override;
        void clear() override;

        std::vector<Entity*> query(const glm::vec3& position, float radius) override;
        std::vector<Entity*> query(const AABB& range);

    private:
        void subdivide();

        Octree* getRoot();
        Octree* getParent() const { return mParent;}

        AABB mBoundary;
        int mCapacity;
        Octree* mParent;
        bool mDivided = false;

        std::vector<Entity*> mEntities;
        std::array<std::unique_ptr<Octree>, 8> mChildren;


        static std::unordered_map<Entity*, Octree*> sEntityNodeMap;
    };
}
#endif