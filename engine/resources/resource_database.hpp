#ifndef RESOURCE_DATABASE_HPP
#define RESOURCE_DATABASE_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "renderer/material.hpp" // LightSpec / LightSourceSpec
#include "renderer/texture.hpp"  // TextureType

namespace Engine::Core {
    class Environment;
}

namespace Engine::Resources {

    using json = nlohmann::json;

    // Maps logical asset names to file paths and material parameters, parsed
    // from resources.json. Accessed only by the loaders (via friendship);
    // application code goes through EntityLoader.
    class ResourceDatabase {
        private:
            static ResourceDatabase& getInstance() {
                static ResourceDatabase instance;
                return instance;
            }

            void loadFile(const std::string& path, const std::string& appInstName);

            ResourceDatabase() = default;
            ~ResourceDatabase() = default;

            std::unordered_map<std::string, std::string> mModelPaths;
            std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mShaderPaths;
            std::unordered_map<std::string, std::unordered_map<Engine::Renderer::TextureType, std::string>> mTexturePaths;
            std::unordered_map<std::string, std::unordered_map<std::string, std::string>> mEntityProperties;
            std::unordered_map<std::string, std::optional<Engine::Renderer::LightSourceSpec>> mLightSourceSpecs;
            std::unordered_map<std::string, std::optional<Engine::Renderer::LightSpec>> mLightSpecs;

            void parseAssets(const json& assetJson);
            void parseEntities(const json& entityJson);

            // parseAssets calls parseModels and parseMaterials
            void parseModels(const json& modelJson);
            void parseMaterials(const json& materialJson);

            // parseMaterials calls the per-section parsers below
            void parseShaderPaths(const json& shaderJson, const std::string& matName);
            void parseLightSpec(const json& lightJson, const std::string& matName);
            void parseTexturePaths(const json& textureJson, const std::string& matName);

            bool hasModelPath(const std::string& name);
            std::string getModelPath(const std::string& name);

            bool hasTexturePath(const std::string& name, const Engine::Renderer::TextureType& type);
            std::string getTexturePath(const std::string& name, const Engine::Renderer::TextureType& type);

            bool hasShaderPaths(const std::string& name);
            std::unordered_map<std::string, std::string> getShaderPaths(const std::string& name);

            bool hasLightSpec(const std::string& matName);
            Engine::Renderer::LightSpec getLightSpec(const std::string& matName);

            bool hasLightSourceSpec(const std::string& matName);
            Engine::Renderer::LightSourceSpec getLightSourceSpec(const std::string& matName);

            bool hasEntityProperties(const std::string& entityName);
            std::unordered_map<std::string, std::string> getEntityProperties(const std::string& entityName);

            friend class ModelLoader;
            friend class MaterialLoader;
            friend class TextureLoader;
            friend class ShaderLoader;
            friend class EntityLoader;
            friend class Engine::Core::Environment;
    };
}

#endif // RESOURCE_DATABASE_HPP
