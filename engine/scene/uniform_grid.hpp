#ifndef __UNIFORM_GRID_HPP
#define __UNIFORM_GRID_HPP

#include "spatial_index.hpp"
#include <unordered_map>

struct CellHash {
    std::size_t operator()(const glm::ivec3& v) const {
        return ((std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1)) >> 1) ^ (std::hash<float>()(v.z) << 1);
    }
};

namespace Engine::Scene {
    class UniformGrid : public SpatialIndex {
    public:
        explicit UniformGrid(float cellSize = 2.0f) : mCellSize(cellSize) {}

        void insert(Entity* entity) override;
        void remove(Entity* entity) override;
        void update(Entity* entity) override;
        
        std::vector<Entity*> query(const glm::vec3& position, float radius) override;
        void clear() override { mCells.clear(); mEntityCellMap.clear(); }

    private:
        float mCellSize;
        std::unordered_map<Entity*, glm::ivec3> mEntityCellMap;
        std::unordered_map<glm::ivec3, std::vector<Entity*>, CellHash> mCells;

        glm::ivec3 getCellCoords(const glm::vec3& pos) const {
            return { std::floor(pos.x / mCellSize), 
                     std::floor(pos.y / mCellSize), 
                     std::floor(pos.z / mCellSize) };
        }
    };
}

#endif