#include "octree.hpp"

std::unordered_map<Engine::Scene::Entity*, Engine::Scene::Octree*> Engine::Scene::Octree::sEntityNodeMap;

void Engine::Scene::Octree::insert(Entity *entity)
{
    if (!mBoundary.contains(entity->transform.position)) return;

    if (!mDivided) {
        if (mEntities.size() < mCapacity) {
            mEntities.push_back(entity);
            sEntityNodeMap[entity] = this;
        } else {
            subdivide();
            insert(entity);
        }
    } else {
        for (auto& child : mChildren) {
            if (child->mBoundary.contains(entity->transform.position)) {
                child->insert(entity);
                break;
            }
        }
    }
}

void Engine::Scene::Octree::remove(Entity *entity)
{
    auto it = sEntityNodeMap.find(entity);
    if (it != sEntityNodeMap.end()) {
        Octree* node = it->second;

        auto& v = node->mEntities;
        v.erase(std::remove(v.begin(), v.end(), entity), v.end());

        sEntityNodeMap.erase(it);
    }
}

void Engine::Scene::Octree::update(Entity *entity)
{
    auto it = sEntityNodeMap.find(entity);
    if (it == sEntityNodeMap.end()) {
        insert(entity);
        return;
    }

    Octree* node = it->second;
    if(!node->mBoundary.contains(entity->transform.position)){
        node->remove(entity);
        node->getRoot()->insert(entity);
    }
}

void Engine::Scene::Octree::clear()
{
    mEntities.clear();
    sEntityNodeMap.clear();
    if (mDivided)
    {
        for(auto& child: mChildren) child->clear();
        mDivided = false;
    }
}

std::vector<Engine::Scene::Entity*> Engine::Scene::Octree::query(const glm::vec3 &position, float radius)
{
    return query(AABB{position, glm::vec3(radius)});
}

std::vector<Engine::Scene::Entity*> Engine::Scene::Octree::query(const AABB &range)
{
    std::vector<Entity*> found;
    if(!mBoundary.intersects(range)) return found;

    if (mDivided)
    {
        for (auto& child : mChildren)
        {
            auto childFound = child->query(range);
            found.insert(found.end(), childFound.begin(), childFound.end());
        }
    } else {
        for(auto* e: mEntities) {
            if(range.contains(e->transform.position))
                found.push_back(e);
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

    for(auto* e: mEntities){
        for (auto& child : mChildren)
        {
            if (child->mBoundary.contains(e->transform.position))
            {
                child->insert(e);
                break;
            }
        }
    }
    
    mEntities.clear();
}

Engine::Scene::Octree* Engine::Scene::Octree::getRoot()
{
    Octree* current = this;
    while(current->mParent != nullptr)
        current = current->mParent;
    return current;
}
