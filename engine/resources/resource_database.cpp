#include "resource_database.hpp"
#include "core/logger.hpp"

#include <fstream>

namespace Engine::Resources {

    void ResourceDatabase::loadFile(const std::string& path, const std::string& appInstName) {
        std::ifstream file(path);
        if (!file.is_open()) {
            LOG_CRITICAL("Failed to open resources file at: " + path);
            return;
        }
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            LOG_CRITICAL("Failed to parse resources file: " + std::string(e.what()));
            return;
        }

        if (j.contains(appInstName)) {
            auto app = j[appInstName];
            if (!app.contains("assets") || !app.contains("entities")) {
                LOG_CRITICAL("resources.json doesn't contain either assets or entities");
                return;
            }
            parseAssets(app["assets"]);
            parseEntities(app["entities"]);
        } else {
            LOG_CRITICAL("App instance " + appInstName + " not found in resources file");
        }
    }

    void ResourceDatabase::parseAssets(const json& assetJson) {
        if (!assetJson.contains("models") || !assetJson.contains("materials")) {
            LOG_CRITICAL("resources.json doesn't contain either models or materials");
            return;
        }
        parseModels(assetJson["models"]);
        parseMaterials(assetJson["materials"]);
    }

    void ResourceDatabase::parseEntities(const json& entityJson) {
        for (auto& [key, value] : entityJson.items()) {
            if (!value.contains("model") || !value.contains("material")) {
                LOG_ERROR("Entity '" + key + "' is missing model or material; skipping it");
                continue;
            }

            for (auto& [propKey, propValue] : value.items()) {
                if (propValue.is_string()) {
                    mEntityProperties[key][propKey] = propValue.get<std::string>();
                }
            }
        }
    }

    void ResourceDatabase::parseModels(const json& modelJson) {
        for (auto& [key, value] : modelJson.items()) {
            mModelPaths[key] = value;
        }
    }

    void ResourceDatabase::parseMaterials(const json& materialJson) {
        for (auto& [key, value] : materialJson.items()) {
            if (value.contains("shaders")) {
                parseShaderPaths(value["shaders"], key);
            }
            if (value.contains("textures")) {
                parseTexturePaths(value["textures"], key);
            }
            if (value.contains("lightSpec")) {
                parseLightSpec(value["lightSpec"], key);
            } else if (value.contains("lightSourceSpec")) {
                parseLightSpec(value["lightSourceSpec"], key);
            }
        }
    }

    void ResourceDatabase::parseShaderPaths(const json& shaderJson, const std::string& matName) {
        // Only registered stages end up in the map, so a missing lookup means
        // "this material has no such stage".
        for (const auto& stage : {"vertex", "fragment", "geometry"}) {
            std::string path = shaderJson.value(stage, "");
            if (!path.empty()) {
                mShaderPaths[matName][stage] = path;
            }
        }
    }

    void ResourceDatabase::parseLightSpec(const json& lightJson, const std::string& matName) {
        // The presence of "color" distinguishes an emitting light source from
        // a surface light response.
        if (lightJson.contains("color")) {
            Renderer::LightSourceSpec lss;
            lss.color = glm::vec3(lightJson["color"][0], lightJson["color"][1], lightJson["color"][2]);
            lss.emission_strength = lightJson.value("emission_strength", lss.emission_strength);
            lss.intensity = lightJson.value("intensity", lss.intensity);

            mLightSourceSpecs[matName] = lss;
        } else {
            Renderer::LightSpec ls;
            ls.ambient = lightJson.value("ambient", ls.ambient);
            ls.diffuse = lightJson.value("diffuse", ls.diffuse);
            ls.specular = lightJson.value("specular", ls.specular);

            mLightSpecs[matName] = ls;
        }
    }

    void ResourceDatabase::parseTexturePaths(const json& textureJson, const std::string& matName) {
        const std::pair<const char*, Engine::Renderer::TextureType> slots[] = {
            {"albedo",   Engine::Renderer::TextureType::Albedo},
            {"normal",   Engine::Renderer::TextureType::Normal},
            {"specular", Engine::Renderer::TextureType::Specular},
            {"emission", Engine::Renderer::TextureType::Emission},
        };

        for (const auto& [key, type] : slots) {
            std::string path = textureJson.value(key, "");
            if (!path.empty()) {
                mTexturePaths[matName][type] = path;
            }
        }
    }

    bool ResourceDatabase::hasModelPath(const std::string& name) {
        return mModelPaths.find(name) != mModelPaths.end();
    }

    std::string ResourceDatabase::getModelPath(const std::string& name) {
        auto it = mModelPaths.find(name);
        if (it != mModelPaths.end()) {
            return it->second;
        }
        LOG_ERROR("Requested model path for '" + name + "' does not exist in the database.");
        return {};
    }

    bool ResourceDatabase::hasTexturePath(const std::string& name, const Engine::Renderer::TextureType& type) {
        auto it = mTexturePaths.find(name);
        return it != mTexturePaths.end() && it->second.find(type) != it->second.end();
    }

    std::string ResourceDatabase::getTexturePath(const std::string& name, const Engine::Renderer::TextureType& type) {
        auto it = mTexturePaths.find(name);
        if (it != mTexturePaths.end()) {
            auto typeIt = it->second.find(type);
            if (typeIt != it->second.end()) {
                return typeIt->second;
            }
        }
        LOG_ERROR("Requested texture path for material '" + name + "' does not exist in the database.");
        return {};
    }

    bool ResourceDatabase::hasShaderPaths(const std::string& name) {
        return mShaderPaths.find(name) != mShaderPaths.end();
    }

    std::unordered_map<std::string, std::string> ResourceDatabase::getShaderPaths(const std::string& name) {
        auto it = mShaderPaths.find(name);
        if (it != mShaderPaths.end()) {
            return it->second;
        }
        LOG_ERROR("Requested shader paths for material '" + name + "' do not exist in the database.");
        return {};
    }

    bool ResourceDatabase::hasLightSpec(const std::string& matName) {
        auto it = mLightSpecs.find(matName);
        return it != mLightSpecs.end() && it->second.has_value();
    }

    Engine::Renderer::LightSpec ResourceDatabase::getLightSpec(const std::string& matName) {
        auto it = mLightSpecs.find(matName);
        if (it != mLightSpecs.end() && it->second.has_value()) {
            return it->second.value();
        }

        LOG_ERROR("Requested LightSpec for material '" + matName + "' does not exist in the database.");
        return Engine::Renderer::LightSpec{};
    }

    bool ResourceDatabase::hasLightSourceSpec(const std::string& matName) {
        auto it = mLightSourceSpecs.find(matName);
        return it != mLightSourceSpecs.end() && it->second.has_value();
    }

    Engine::Renderer::LightSourceSpec ResourceDatabase::getLightSourceSpec(const std::string& matName) {
        auto it = mLightSourceSpecs.find(matName);
        if (it != mLightSourceSpecs.end() && it->second.has_value()) {
            return it->second.value();
        }

        LOG_ERROR("Requested LightSourceSpec for material '" + matName + "' does not exist in the database.");
        return Engine::Renderer::LightSourceSpec{};
    }

    bool ResourceDatabase::hasEntityProperties(const std::string& entityName) {
        return mEntityProperties.find(entityName) != mEntityProperties.end();
    }

    std::unordered_map<std::string, std::string> ResourceDatabase::getEntityProperties(const std::string& entityName) {
        auto it = mEntityProperties.find(entityName);
        if (it != mEntityProperties.end()) {
            return it->second;
        }

        LOG_ERROR("Requested properties for entity '" + entityName + "' do not exist in the database.");
        return {};
    }
}
