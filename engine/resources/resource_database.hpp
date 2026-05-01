#ifndef __RESOURCE_DB_HPP
#define __RESOURCE_DB_HPP

#include <fstream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "core/enviroment.hpp"

#include "renderer/model.hpp"
#include "renderer/shaders.hpp"
#include "renderer/texture.hpp"
#include "renderer/material.hpp"

#include "resources/model_loader.hpp"
#include "resources/material_loader.hpp"
#include "resources/texture_loader.hpp"
#include "resources/shader_loader.hpp"

#include "scene/entity.hpp"

using json = nlohmann::json;

template<class T, class A, class S>
using full_map = std::unordered_map<T, std::unordered_map<A, S>>;

namespace Engine{
    namespace Resources{
        class ResourceDatabase{
            private:

                static ResourceDatabase& getInstance() {
                    static ResourceDatabase instance;
                    return instance;
                }

                void loadFile(const std::string& path, const std::string& appInstName);

                ResourceDatabase() = default;
                ~ResourceDatabase() = default;

                std::unordered_map<std::string, std::string> mModelPaths;
                full_map<std::string, std::string, std::string> mShaderPaths;
                full_map<std::string, Engine::Renderer::TextureType, std::string> mTexturePaths;
                full_map<std::string, std::string, std::string> mEntityProperties;
                std::unordered_map<std::string, std::optional<Engine::Renderer::LightSourceSpec>> mLightSourceSpecs;
                std::unordered_map<std::string, std::optional<Engine::Renderer::LightSpec>> mLightSpecs;

                void parseAssets(const json& assetJson);
                void parseEntities(const json& entityJson);
                
                // parseAssets poziva parseMaterials i parseModels
                void parseModels(const json& modelJson);
                void parseMaterials(const json& materialJson);
                
                // parseMaterials poziva getShaderPaths i getLightSpecs()
                void getShaderPaths(const json& shaderJson, const std::string& matName);
                void getLightSpecs(const json& lightJson, const std::string& matName);
                void getTexturePaths(const json& textureJson, const std::string& matName);

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
}

#endif