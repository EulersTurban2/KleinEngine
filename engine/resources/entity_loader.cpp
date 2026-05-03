#include "entity_loader.hpp"
#include "model_loader.hpp"
#include "material_loader.hpp"
#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"
#include "scene/components.hpp"
#include "core/logger.hpp"

namespace Engine::Resources {

    Engine::Scene::Entity EntityLoader::instantiate(const std::string& entityName, Engine::Scene::Scene& targetScene)
    {
        if (!ResourceDatabase::getInstance().hasEntityProperties(entityName))
        {
            LOG_ERROR("Entity '" + entityName + "' does not exist in the database.");
            return targetScene.createEntity("MissingPrefab"); 
        }

        auto properties = ResourceDatabase::getInstance().getEntityProperties(entityName);
        auto modelIt = properties.find("model");
        auto materialIt = properties.find("material");

        if (modelIt == properties.end() || materialIt == properties.end())
        {
            LOG_ERROR("Entity '" + entityName + "' is missing model or material properties.");
            return targetScene.createEntity("BrokenPrefab");
        }

        auto model = ModelLoader::getInstance().getModel(modelIt->second);
        auto material = MaterialLoader::getInstance().getMaterial(materialIt->second);

        if (!model || !material)
        {
            LOG_CRITICAL("Failed to instantiate entity '" + entityName + "'. Missing GPU resources.");
            return targetScene.createEntity("BrokenPrefab");
        }

        Engine::Scene::Entity entity = targetScene.createEntity(entityName);

        entity.addComponent<Engine::Scene::MeshRendererComponent>(
            Engine::Scene::MeshRendererComponent{model, material}
        );

        return entity;
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