#include "uniform_grid.hpp"

#include <algorithm>
#include <glm/gtx/norm.hpp>

void Engine::Scene::UniformGrid::insert(uint32_t entity, const glm::vec3& position)
{
    glm::ivec3 cell = getCellCoords(position);
    mCells[cell].push_back({entity, position});
    mEntityCellMap[entity] = cell;
}

void Engine::Scene::UniformGrid::remove(uint32_t entity)
{
    auto it = mEntityCellMap.find(entity);
    if (it != mEntityCellMap.end())
    {
        glm::ivec3 cell = it->second;
        auto& cellVector = mCells[cell];
        cellVector.erase(std::remove_if(cellVector.begin(), cellVector.end(), 
            [entity](const SpatialItem& item) { return item.id == entity; }), cellVector.end());
        mEntityCellMap.erase(it);

        if (cellVector.empty())
        {
            mCells.erase(cell);
        }
    }
    
}

void Engine::Scene::UniformGrid::update(uint32_t entity, const glm::vec3& position)
{
    glm::ivec3 newCell = getCellCoords(position);

    auto it = mEntityCellMap.find(entity);
    if (it != mEntityCellMap.end()) {
        glm::ivec3 oldCell = it->second;
        if (newCell != oldCell) {
            remove(entity);
            insert(entity,position);
        } else {
            for (auto& item : mCells[oldCell]) {
                if (item.id == entity) {
                    item.position = position;
                    break;
                }
            }
        }
    } else {
        insert(entity, position);
    }
}

std::vector<uint32_t> Engine::Scene::UniformGrid::query(const glm::vec3 &position, float radius)
{
    std::vector<uint32_t> found;
    glm::vec3 minBound = position - glm::vec3(radius);
    glm::vec3 maxBound = position + glm::vec3(radius);

    glm::ivec3 minCell = getCellCoords(minBound);
    glm::ivec3 maxCell = getCellCoords(maxBound);

    for (int x = minCell.x; x < maxCell.x; x++)
    {
        for (int y = minCell.y; y < maxCell.y; y++)
        {
            for (int z = minCell.z; z < maxCell.z; z++)
            {
                auto it = mCells.find(glm::ivec3(x,y,z));
                if (it != mCells.end())
                {
                    for(const auto& item : it->second) {
                        if (glm::distance2(position, item.position) <= (radius * radius))
                        {
                            found.push_back(item.id);
                        }
                    }
                }
                
            }
        }
    }
    return found;
}
