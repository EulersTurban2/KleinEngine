#include "resource_database.hpp"

namespace Engine::Resources{
    void ResourceDatabase::loadFile(const std::string& path, const std::string& appInstName){
        std::ifstream file(path);
        if (!file.is_open())
        {
            LOG_CRITICAL("Failed to open resources file");
        }
        json j;
        try {
            file >> j;
        } catch(const json::parse_error e) {
            LOG_CRITICAL("Failed to parse resources file");
        }
        
        if (j.contains(appInstName))
        {
            auto app = j[appInstName];
            if (!app.contains("assets") || !app.contains("entities"))
            {
                LOG_CRITICAL("resources.json doesn't contain either assets or entities");
            }
            parseAssets(app["assets"]);
            parseEntities(app["entities"]);
        } else {
            LOG_CRITICAL("App instance " + appInstName + " not found in resources file");
        }
    }

    void ResourceDatabase::parseAssets(const json &assetJson)
    {
        if (!assetJson.contains("models") || !assetJson.contains("materials"))
        {
            LOG_CRITICAL("resources.json doesn't contain either models or materials");
        }
        parseModels(assetJson["models"]);
        parseMaterials(assetJson["materials"]);
    }   

    void ResourceDatabase::parseEntities(const json &entityJson)
    {
        for(auto& [key, value] : entityJson.items()){
            if (!value.contains("model") || !value.contains("material"))
            {
                LOG_CRITICAL("entity without model or material");
            }
            auto model = value["model"];
            auto material = value["material"];

            mEntityProperties[key]["model"] = model;
            mEntityProperties[key]["material"] = material;
        }
    }

    void ResourceDatabase::parseModels(const json &modelJson)
    {
        for(auto& [key, value] : modelJson.items()){
            mModelPaths[key] = value;
        }
    }

    void ResourceDatabase::parseMaterials(const json &materialJson)
    {
        for(auto& [key, value] : materialJson.items()){
            if (value.contains("shaders"))
            {
                getShaderPaths(value["shaders"],key);
            }
            if (value.contains("textures"))
            {
                getTexturePaths(value["textures"], key);
            }
            if (value.contains("lightSpec"))
            {
                getLightSpecs(value["lightSpec"],key);
            }else if(value.contains("lightSourceSpec"))
            {
                getLightSpecs(value["lightSourceSpec"],key);
            }            
            
        }
    }

    void ResourceDatabase::getShaderPaths(const json &shaderJson, const std::string &matName)
    {
        auto vertex = shaderJson.value("vertex","");
        auto fragment = shaderJson.value("fragment","");
        auto geometry = shaderJson.value("geometry","");

        mShaderPaths[matName]["vertex"] = vertex;
        mShaderPaths[matName]["fragment"] = fragment;
        mShaderPaths[matName]["geometry"] = geometry;
    }

    void ResourceDatabase::getLightSpecs(const json &lightJson, const std::string &matName)
    {
        if (lightJson.contains("color"))
        {
            Renderer::LightSourceSpec lss;
            lss.color = glm::vec3(lightJson["color"][0],lightJson["color"][1],lightJson["color"][2]);
            lss.emission_strength = lightJson.value("emission_strength",1.0f);
            lss.intensity = lightJson.value("intensity",1.0f);

            mLightSourceSpecs[matName] = lss;
        } else {
            Renderer::LightSpec ls;
            ls.ambient = lightJson.value("ambient",1.0f);
            ls.diffuse = lightJson.value("diffuse",1.0f);
            ls.spec = lightJson.value("specular",1.0f);

            mLightSpecs[matName] = ls;
        }
        
    }

    void ResourceDatabase::getTexturePaths(const json &textureJson, const std::string &matName)
    {
        auto albedo = textureJson.value("albedo","");
        auto specular = textureJson.value("specular","");
        auto normalMap = textureJson.value("normal","");

        mTexturePaths[matName][Engine::Renderer::TextureType::Albedo] = albedo;
        mTexturePaths[matName][Engine::Renderer::TextureType::Specluar] = specular;
        mTexturePaths[matName][Engine::Renderer::TextureType::Normal] = normalMap;
    }
    bool ResourceDatabase::hasModelPath(const std::string &name)
    {
        return mModelPaths.find(name) != mModelPaths.end();
    }
    std::string ResourceDatabase::getModelPath(const std::string &name)
    {
        return mModelPaths[name];
    }
    bool ResourceDatabase::hasTexturePath(const std::string &name, const Engine::Renderer::TextureType &type)
    {
        return mTexturePaths.find(name) != mTexturePaths.end() && mTexturePaths[name].find(type) != mTexturePaths[name].end();
    }
    std::string ResourceDatabase::getTexturePath(const std::string &name, const Engine::Renderer::TextureType &type)
    {
        return mTexturePaths[name][type];
    }
    bool ResourceDatabase::hasShaderPaths(const std::string& name)
    {
        return mShaderPaths.find(name) != mShaderPaths.end();
    }

    std::unordered_map<std::string, std::string> ResourceDatabase::getShaderPaths(const std::string& name)
    {
        if (hasShaderPaths(name))
        {
            return mShaderPaths[name];
        }
        LOG_ERROR("Requested shader paths for material '" + name + "' do not exist in the database.");
        return {}; 
    }

    bool ResourceDatabase::hasLightSpec(const std::string& matName)
    {
        auto it = mLightSpecs.find(matName);
        return it != mLightSpecs.end() && it->second.has_value();
    }

    Engine::Renderer::LightSpec ResourceDatabase::getLightSpec(const std::string& matName)
    {
        if (hasLightSpec(matName))
        {
            return mLightSpecs[matName].value();
        }
        
        LOG_ERROR("Requested LightSpec for material '" + matName + "' does not exist in the database.");
        return Engine::Renderer::LightSpec{}; 
    }

    bool ResourceDatabase::hasLightSourceSpec(const std::string& matName)
    {
        auto it = mLightSourceSpecs.find(matName);
        return it != mLightSourceSpecs.end() && it->second.has_value();
    }

    Engine::Renderer::LightSourceSpec ResourceDatabase::getLightSourceSpec(const std::string& matName)
    {
        if (hasLightSourceSpec(matName))
        {
            return mLightSourceSpecs[matName].value();
        }
        
        LOG_ERROR("Requested LightSourceSpec for material '" + matName + "' does not exist in the database.");
        return Engine::Renderer::LightSourceSpec{}; 
    }

    bool ResourceDatabase::hasEntityProperties(const std::string& entityName)
    {
        return mEntityProperties.find(entityName) != mEntityProperties.end();
    }

    std::unordered_map<std::string, std::string> ResourceDatabase::getEntityProperties(const std::string& entityName)
    {
        if (hasEntityProperties(entityName))
        {
            return mEntityProperties[entityName];
        }
        
        LOG_ERROR("Requested properties for entity '" + entityName + "' do not exist in the database.");
        return {}; 
    }
}