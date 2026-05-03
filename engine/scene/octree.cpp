#include "octree.hpp"
#include <algorithm>

std::unordered_map<uint32_t, Engine::Scene::Octree*> Engine::Scene::Octree::sEntityNodeMap;

void Engine::Scene::Octree::insert(uint32_t entity, const glm::vec3& position)
{
    if (!mBoundary.contains(position)) return;

    if (!mDivided) {
        if (mItems.size() < mCapacity) {
            mItems.push_back({entity, position});
            sEntityNodeMap[entity] = this;
        } else {
            subdivide();
            insert(entity, position);
        }
    } else {
        for (auto& child : mChildren) {
            if (child->mBoundary.contains(position)) {
                child->insert(entity, position);
                break;
            }
        }
    }
}

void Engine::Scene::Octree::remove(uint32_t entity)
{
    auto it = sEntityNodeMap.find(entity);
    if (it != sEntityNodeMap.end()) {
        Octree* node = it->second;

        auto& v = node->mItems;
        v.erase(std::remove_if(v.begin(), v.end(), 
            [entity](const SpatialItem& item) { return item.id == entity; }), v.end());

        sEntityNodeMap.erase(it);
    }
}

void Engine::Scene::Octree::update(uint32_t entity, const glm::vec3& position)
{
    auto it = sEntityNodeMap.find(entity);
    if (it == sEntityNodeMap.end()) {
        insert(entity, position);
        return;
    }

    Octree* node = it->second;
    if(!node->mBoundary.contains(position)){
        node->remove(entity);
        node->getRoot()->insert(entity, position);
    } else {
        for(auto& item : node->mItems){
            if(item.id == entity){
                item.position = position;
                break;
            }
        }
    }
}

void Engine::Scene::Octree::clear()
{
    mItems.clear();
    sEntityNodeMap.clear();
    if (mDivided)
    {
        for(auto& child: mChildren) child->clear();
        mDivided = false;
    }
}

std::vector<uint32_t> Engine::Scene::Octree::query(const glm::vec3 &position, float radius)
{
    return query(AABB{position, glm::vec3(radius)});
}

std::vector<uint32_t> Engine::Scene::Octree::query(const AABB &range)
{
    std::vector<uint32_t> found;
    if(!mBoundary.intersects(range)) return found;

    if (mDivided)
    {
        for (auto& child : mChildren)
        {
            auto childFound = child->query(range);
            found.insert(found.end(), childFound.begin(), childFound.end());
        }
    } else {
        for(const auto& item: mItems) {
            if(range.contains(item.position))
                found.push_back(item.id);
        }
    }
    return found;
}

void Engine::Scene::Octree::subdivide()
{
    glm::vec3  newHalf = mBoundary.halfExtent * 0.5f;

    for (int i = 0; i < 8; ++i)
    {
        glm::vec3 offset;
        offset.x = ((i & 1) ? newHalf.x : -newHalf.x);
        offset.y = ((i & 2) ? newHalf.y : -newHalf.y);
        offset.z = ((i & 4) ? newHalf.z : -newHalf.z);

        mChildren[i] = std::make_unique<Octree>(
            AABB{mBoundary.center + offset, newHalf}, mCapacity
        );
        mChildren[i]->mParent = this;
    }
    
    mDivided = true;

    for(const auto& item: mItems){
        for (auto& child : mChildren)
        {
            if (child->mBoundary.contains(item.position))
            {
                child->insert(item.id, item.position);
                break;
            }
        }
    }

    mItems.clear();
}

Engine::Scene::Octree* Engine::Scene::Octree::getRoot()
{
    Octree* current = this;
    while(current->mParent != nullptr)
        current = current->mParent;
    return current;
}
