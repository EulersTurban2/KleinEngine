#include "model_loader.hpp"

namespace Engine::Resources{
    void ModelLoader::loadFile(const std::string& path, const std::string& appInstName){
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

    void ModelLoader::parseAssets(const json &assetJson)
    {
        if (!assetJson.contains("models") || !assetJson.contains("materials"))
        {
            LOG_CRITICAL("resources.json doesn't contain either models or materials");
        }
        parseModels(assetJson["models"]);
        parseMaterials(assetJson["materials"]);
    }   

    void ModelLoader::parseEntities(const json &entityJson)
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

    void ModelLoader::parseModels(const json &modelJson)
    {
        for(auto& [key, value] : modelJson.items()){
            mModelPaths[key] = value;
        }
    }

    void ModelLoader::parseMaterials(const json &materialJson)
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

    void ModelLoader::getShaderPaths(const json &shaderJson, const std::string &matName)
    {
        auto vertex = shaderJson.value("vertex","");
        auto fragment = shaderJson.value("fragment","");
        auto geometry = shaderJson.value("geometry","");

        mShaderPaths[matName]["vertex"] = vertex;
        mShaderPaths[matName]["fragment"] = fragment;
        mShaderPaths[matName]["geometry"] = geometry;
    }

    void ModelLoader::getLightSpecs(const json &lightJson, const std::string &matName)
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

    void ModelLoader::getTexturePaths(const json &textureJson, const std::string &matName)
    {
        auto albedo = textureJson.value("albedo","");
        auto specular = textureJson.value("specular","");
        auto normalMap = textureJson.value("normal","");

        mTexturePaths[matName][Engine::Renderer::TextureType::Albedo] = albedo;
        mTexturePaths[matName][Engine::Renderer::TextureType::Specluar] = specular;
        mTexturePaths[matName][Engine::Renderer::TextureType::Normal] = normalMap;
    }

    std::shared_ptr<Renderer::Model> ModelLoader::fetchModel(const std::string &modelName)
    {
        auto cacheIt = mModelCache.find(modelName);
        if (cacheIt != mModelCache.end()) {
            return cacheIt->second;
        }

        auto pathIt = mModelPaths.find(modelName);
        if (pathIt == mModelPaths.end()) {
            LOG_ERROR("Path to model " + modelName + " not found in json");
            return nullptr;
        }

        auto newModel = std::make_shared<Renderer::Model>(pathIt->second);
        mModelCache[modelName] = newModel;
        return newModel;
    }
    std::shared_ptr<Renderer::ShaderProgram> ModelLoader::fetchShaderProgram(const std::string &matName)
    {
        auto cacheIt = mShaderCache.find(matName);
        if (cacheIt != mShaderCache.end()) {
            return cacheIt->second;
        }

        auto pathIt = mShaderPaths.find(matName);
        if (pathIt == mShaderPaths.end()) {
            return nullptr;
        }

        const auto& paths = pathIt->second;
        auto vertIt = paths.find("vertex");
        auto fragIt = paths.find("fragment");

        if (vertIt == paths.end() || fragIt == paths.end()) {
            LOG_ERROR("Shaders for material " + matName + " are missing");
            return nullptr;
        }

        Engine::Renderer::Shader* vertShader = new Engine::Renderer::Shader(vertIt->second, Engine::Renderer::SHADER_TYPE::VERTEX_SHADER);
        Engine::Renderer::Shader* fragShader = new Engine::Renderer::Shader(fragIt->second, Engine::Renderer::SHADER_TYPE::FRAGMENT_SHADER);

        std::vector<Engine::Renderer::Shader*> shaders = { vertShader, fragShader };

        auto shaderProgram = std::make_shared<Renderer::ShaderProgram>(shaders);

        delete vertShader;
        delete fragShader;
        
        mShaderCache[matName] = shaderProgram;
        return shaderProgram;
    }

    std::shared_ptr<Renderer::Texture2D> ModelLoader::fetchTexture(const std::string &matName,const Engine::Renderer::TextureType& type)
    {
        auto matCacheIt = mTextureCache.find(matName);
        if (matCacheIt != mTextureCache.end()) {
            auto texIt = matCacheIt->second.find(type);
            if (texIt != matCacheIt->second.end()) {
                return texIt->second; 
            }
        }

        auto matPathIt = mTexturePaths.find(matName);
        if (matPathIt == mTexturePaths.end()) {
            return nullptr;
        }

        auto typePathIt = matPathIt->second.find(type);
        if (typePathIt == matPathIt->second.end()) {
            return nullptr;
        }

        std::string filepath = typePathIt->second;
        auto texture = std::make_shared<Renderer::Texture2D>(filepath);
        
        mTextureCache[matName][type] = texture;
        return texture;
        
    }

    std::shared_ptr<Renderer::Material> ModelLoader::fetchMaterial(const std::string &matName)
    {
        auto cacheIt = mMaterialCache.find(matName);
        if (cacheIt != mMaterialCache.end()) {
            return cacheIt->second;
        }

        auto shader = fetchShaderProgram(matName);
        if (!shader) {
            LOG_ERROR("Failed to fetch Shader for material: " + matName);
            return nullptr; 
        }

        auto material = std::make_shared<Renderer::Material>(shader);

        auto matPathIt = mTexturePaths.find(matName);
        if (matPathIt != mTexturePaths.end()) {
            for (const auto& [type, path] : matPathIt->second) {
                if (path == "")
                {
                    continue;
                }
                auto texture = fetchTexture(matName, type);
                if (texture) {
                    material->addTexture(type, texture);
                }
            }
        }

        auto lightSpecIt = mLightSpecs.find(matName);
        if (lightSpecIt != mLightSpecs.end() && lightSpecIt->second.has_value()) {
            material->setLightSpec(lightSpecIt->second.value());
        }

        auto lightSourceIt = mLightSourceSpecs.find(matName);
        if (lightSourceIt != mLightSourceSpecs.end() && lightSourceIt->second.has_value()) {
            material->setLightSourceSpec(lightSourceIt->second.value());
        }

        mMaterialCache[matName] = material;
        return material;
    }

    std::shared_ptr<Scene::Entity> ModelLoader::fetchEntity(const std::string &entityName)
    {
        auto entityIt = mEntityCache.find(entityName);
        if (entityIt != mEntityCache.end())
        {
            return mEntityCache[entityName];
        }
        
        auto modelNameIt = mEntityProperties[entityName].find("model");
        if (modelNameIt == mEntityProperties[entityName].end())
        {
            LOG_ERROR("Model for " + entityName + " doesn't exist");
            return nullptr;
        }
        
        auto modelName = mEntityProperties[entityName]["model"];
        std::shared_ptr<Renderer::Model> model = fetchModel(modelName);
        
        auto materialNameIt = mEntityProperties[entityName].find("material");
        if (materialNameIt == mEntityProperties[entityName].end())
        {
            LOG_ERROR("material for " + entityName + " doesn't exist");
            return nullptr;
        }
        auto materialName = mEntityProperties[entityName]["material"];
        std::shared_ptr<Renderer::Material> material = fetchMaterial(materialName);


        auto entity = std::make_shared<Scene::Entity>();
        entity->material = material;
        entity->model = model;
        mEntityCache[entityName] = entity;
        return entity;
    }

    Scene::Entity ModelLoader::instantiate(const std::string &entityName)
    {
        auto cubeEntity = fetchEntity(entityName);
                
        if (!cubeEntity) {
            LOG_CRITICAL("Failed to fetch entity '" + entityName + "'. Check resources.json.");
            return Engine::Scene::Entity{};
        }
        Engine::Scene::Entity cubeInstance = *cubeEntity;
        return cubeInstance; 
    }

    void ModelLoader::cleanUp(){
        mModelCache.clear();
        mShaderCache.clear();
        mTextureCache.clear();
        mMaterialCache.clear();
        mEntityCache.clear();
    }

    
}