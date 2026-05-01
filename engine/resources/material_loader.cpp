#include "material_loader.hpp"
#include "shader_loader.hpp"
#include "texture_loader.hpp"
#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"
#include "core/logger.hpp"
#include <vector>

namespace Engine::Resources {

    std::shared_ptr<Engine::Renderer::Material> MaterialLoader::getMaterial(const std::string& matName)
    {

        if (ResourceCache::getInstance().hasMaterial(matName))
        {
            return ResourceCache::getInstance().getMaterial(matName);
        }

        auto shader = ShaderLoader::getInstance().getShaderProgram(matName);
        if (!shader)
        {
            LOG_ERROR("Failed to generate Material '" + matName + "': Shader program could not be loaded.");
            return nullptr;
        }

        auto material = std::make_shared<Engine::Renderer::Material>(shader);

        std::vector<Engine::Renderer::TextureType> textureTypes = {
            Engine::Renderer::TextureType::Albedo,
            Engine::Renderer::TextureType::Specluar, 
            Engine::Renderer::TextureType::Normal
        };

        for (const auto& type : textureTypes)
        {
            
            if (ResourceDatabase::getInstance().hasTexturePath(matName, type))
            {
                auto texture = TextureLoader::getInstance().getTexture(matName, type);
                if (texture)
                {
                    material->addTexture(type, texture);
                }
            }
        }

        if (ResourceDatabase::getInstance().hasLightSpec(matName))
        {
            material->setLightSpec(ResourceDatabase::getInstance().getLightSpec(matName));
        }
        else if (ResourceDatabase::getInstance().hasLightSourceSpec(matName))
        {
            material->setLightSourceSpec(ResourceDatabase::getInstance().getLightSourceSpec(matName));
        }

        ResourceCache::getInstance().addMaterial(matName, material);

        return material;
    }

} // namespace Engine::Resources