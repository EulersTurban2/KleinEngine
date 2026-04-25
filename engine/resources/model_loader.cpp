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
            auto appInstDict = j[appInstName];
            if (appInstDict.contains("models")){
                auto modelDict = appInstDict["models"];
                for(auto& [modelName, modelData] : modelDict.items()){
                    for(auto& [key, value] : modelData.items()){
                        if(value.is_string())
                            modelMap[modelName][key] = value.get<std::string>();
                    }
                    LOG_INFO("Successfully loaded all paths for " + modelName);
                }
                LOG_INFO("Successfully loaded in resource paths");
            } else {
                LOG_CRITICAL(appInstName + " doesn't contain the \" model \" key");
            }
        } else {
            LOG_CRITICAL("App instance " + appInstName + " not found in resources file");
        }
    }
    std::string ModelLoader::getAttrib(const std::string& model, const std::string& attribute){
        auto it = modelMap.find(model);
        if (it != modelMap.end())
        {
            // pronadjeno nesto 
            auto other = (it->second).find(attribute);
            if (other != (it->second).end())
            {
                return other->second;
            }
            LOG_ERROR("Failed to find attribute " + attribute + " for model " + model);
            return "";
            
        }
        LOG_ERROR("Failed to find model " + model);
        return "";
    }
    std::shared_ptr<Renderer::Model> ModelLoader::genModel(const std::string &model)
    {
        if (mModelCache.find(model) != mModelCache.end()) {
            return mModelCache[model];
        }

        std::string path = getAttrib(model, "path");
        if (path == "")
        {
            LOG_ERROR("Empty path for model " + model);
            return nullptr;
        }
        
        auto newModel = std::make_shared<Renderer::Model>(path);
        mModelCache[model] = newModel;
        
        return newModel;
    }
    std::shared_ptr<Renderer::ShaderProgram> ModelLoader::createShaderProgram(const std::string &model)
    {
        if (mShaderCache.find(model) != mShaderCache.end()) {
            return mShaderCache[model];
        }

        std::string vertShaderPath = getAttrib(model, "vertex");
        std::string fragShaderPath = getAttrib(model, "fragment");

        if (vertShaderPath == "" || fragShaderPath == "")
        {
            LOG_WARN("Shaders for " + model + " are empty");
            return nullptr;
        }
        
        Renderer::Shader vert(vertShaderPath, Renderer::SHADER_TYPE::VERTEX_SHADER);
        Renderer::Shader frag(fragShaderPath, Renderer::SHADER_TYPE::FRAGMENT_SHADER);

        std::vector<Renderer::Shader*> shaders = { &vert, &frag };
        
        auto newProgram = std::make_shared<Renderer::ShaderProgram>(shaders);
        mShaderCache[model] = newProgram;
        return newProgram;
    }

    void ModelLoader::cleanUp(){
        mModelCache.clear();
        mShaderCache.clear();
    }

}