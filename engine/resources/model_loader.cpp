#include "model_loader.hpp"

namespace Engine::Resources{
    std::shared_ptr<Engine::Renderer::Model>  ModelLoader::getModel(const std::string &modelName)
    {
        if (ResourceCache::getInstance().hasModel(modelName))
        {
            return ResourceCache::getInstance().getModel(modelName);
        }
        if (!ResourceDatabase::getInstance().hasModelPath(modelName))
        {
            LOG_ERROR("The model with that name " + modelName + " doesn't have a path"); 
            return nullptr;
        }
        auto model = std::make_shared<Engine::Renderer::Model>(ResourceDatabase::getInstance().getModelPath(modelName));
        ResourceCache::getInstance().addModel(modelName,model);
        return model;
    }
}