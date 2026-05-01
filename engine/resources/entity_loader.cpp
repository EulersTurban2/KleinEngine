#include "entity_loader.hpp"
#include "model_loader.hpp"
#include "material_loader.hpp"
#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"
#include "core/logger.hpp"

namespace Engine::Resources {

    std::shared_ptr<Engine::Scene::Entity> EntityLoader::getEntityTemplate(const std::string& entityName)
    {
        if (ResourceCache::getInstance().hasEntity(entityName))
        {
            return ResourceCache::getInstance().getEntity(entityName);
        }

        if (!ResourceDatabase::getInstance().hasEntityProperties(entityName))
        {
            LOG_ERROR("Entity '" + entityName + "' does not exist in the database.");
            return nullptr;
        }

        auto properties = ResourceDatabase::getInstance().getEntityProperties(entityName);

        auto modelIt = properties.find("model");
        auto materialIt = properties.find("material");

        if (modelIt == properties.end() || materialIt == properties.end())
        {
            LOG_ERROR("Entity '" + entityName + "' is missing model or material properties.");
            return nullptr;
        }

        auto model = ModelLoader::getInstance().getModel(modelIt->second);
        auto material = MaterialLoader::getInstance().getMaterial(materialIt->second);

        if (!model || !material)
        {
            LOG_ERROR("Failed to load components for entity '" + entityName + "'.");
            return nullptr;
        }

        auto entity = std::make_shared<Engine::Scene::Entity>();
        entity->model = model;
        entity->material = material;

        ResourceCache::getInstance().addEntity(entityName, entity);

        return entity;
    }

    Engine::Scene::Entity EntityLoader::instantiate(const std::string& entityName)
    {
        auto entityTemplate = getEntityTemplate(entityName);

        if (!entityTemplate)
        {
            LOG_CRITICAL("Failed to instantiate entity '" + entityName + "'.");
            return Engine::Scene::Entity{};
        }

        return *entityTemplate;
    }

    std::shared_ptr<Engine::Renderer::Model> EntityLoader::getModel(const std::string& modelName)
    {
        return ModelLoader::getInstance().getModel(modelName);
    }

    std::shared_ptr<Engine::Renderer::Material> EntityLoader::getMaterial(const std::string& matName)
    {
        return MaterialLoader::getInstance().getMaterial(matName);
    }

    std::shared_ptr<Engine::Renderer::Texture2D> EntityLoader::getTexture(const std::string& matName, const Engine::Renderer::TextureType& type)
    {
        return TextureLoader::getInstance().getTexture(matName, type);
    }

    std::shared_ptr<Engine::Renderer::ShaderProgram> EntityLoader::getShaderProgram(const std::string& matName)
    {
        return ShaderLoader::getInstance().getShaderProgram(matName);
    }

    void EntityLoader::clearCache()
    {
        ResourceCache::getInstance().clearAll();
    }

}