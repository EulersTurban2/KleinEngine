#include "uniform_grid.hpp"

#include <glm/gtx/norm.hpp>

void Engine::Scene::UniformGrid::insert(Entity *entity)
{
    glm::ivec3 cell = getCellCoords(entity->transform.position);
    mCells[cell].push_back(entity);
    mEntityCellMap[entity] = cell;
}

void Engine::Scene::UniformGrid::remove(Entity *entity)
{
    auto it = mEntityCellMap.find(entity);
    if (it != mEntityCellMap.end())
    {
        glm::ivec3 cell = it->second;
        auto& cellVector = mCells[cell];
        cellVector.erase(std::remove(cellVector.begin(), cellVector.end(), entity), cellVector.end());
        mEntityCellMap.erase(it);

        if (cellVector.empty())
        {
            mCells.erase(cell);
        }
    }
    
}

void Engine::Scene::UniformGrid::update(Entity *entity)
{
    glm::ivec3 newCell = getCellCoords(entity->transform.position);

    auto it = mEntityCellMap.find(entity);
    if (it != mEntityCellMap.end()) {
        glm::ivec3 oldCell = it->second;
        if (newCell != oldCell) {
            remove(entity);
            insert(entity);
        }
    } else {
        insert(entity);
    }
}

std::vector<Engine::Scene::Entity*> Engine::Scene::UniformGrid::query(const glm::vec3 &position, float radius)
{
    std::vector<Entity*> found;
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
                glm::ivec3 currCell(x,y,z);
                
                auto it = mCells.find(currCell);
                if (it != mCells.end())
                {
                    for(auto& entity : it->second){
                        float distSq = glm::distance2(position, entity->transform.position);
                        if (distSq <= (radius * radius)) {
                            found.push_back(entity);
                        }
                    }
                }
            }
        }
    }
    return found;
}
