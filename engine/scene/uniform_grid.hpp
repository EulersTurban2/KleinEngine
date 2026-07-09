#ifndef UNIFORM_GRID_HPP
#define UNIFORM_GRID_HPP

#include "spatial_index.hpp"
#include <cmath>
#include <unordered_map>

namespace Engine::Scene {

    struct CellHash {
        std::size_t operator()(const glm::ivec3& v) const {
            std::size_t h = std::hash<int>()(v.x);
            h ^= std::hash<int>()(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>()(v.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    class UniformGrid : public SpatialIndex {
    public:
        explicit UniformGrid(float cellSize = 2.0f) : mCellSize(cellSize) {}

        void insert(uint32_t entity, const glm::vec3& position) override;
        void remove(uint32_t entity) override;
        void update(uint32_t entity, const glm::vec3& position) override;

        std::vector<uint32_t> query(const glm::vec3& position, float radius) override;
        void clear() override { mCells.clear(); mEntityCellMap.clear(); }

    private:
        float mCellSize;
        std::unordered_map<uint32_t, glm::ivec3> mEntityCellMap;
        std::unordered_map<glm::ivec3, std::vector<SpatialItem>, CellHash> mCells;

        glm::ivec3 getCellCoords(const glm::vec3& pos) const {
            return { static_cast<int>(std::floor(pos.x / mCellSize)),
                     static_cast<int>(std::floor(pos.y / mCellSize)),
                     static_cast<int>(std::floor(pos.z / mCellSize)) };
        }
    };
}

#endif // UNIFORM_GRID_HPP
