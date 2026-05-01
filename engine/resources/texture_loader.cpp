#include "texture_loader.hpp"
#include "core/logger.hpp"

namespace Engine::Resources {

    std::shared_ptr<Engine::Renderer::Texture2D> TextureLoader::getTexture(const std::string& matName, const Engine::Renderer::TextureType& type)
    {
        if (ResourceCache::getInstance().hasTexture(matName, type))
        {
            return ResourceCache::getInstance().getTexture(matName, type);
        }

        if (!ResourceDatabase::getInstance().hasTexturePath(matName, type))
        {
            LOG_ERROR("The texture for material '" + matName + "' of the requested type doesn't have a registered path.");
            return nullptr;
        }

        std::string path = ResourceDatabase::getInstance().getTexturePath(matName, type);
        if (path.empty()) {
            return nullptr;
        }

        auto texture = std::make_shared<Engine::Renderer::Texture2D>(ResourceDatabase::getInstance().getTexturePath(matName, type));
        ResourceCache::getInstance().addTexture(matName, type, texture);
        return texture;
    }

} // namespace Engine::Resources