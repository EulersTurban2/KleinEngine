#ifndef __MODEL_LOADER_HPP
#define __MODEL_LOADER_HPP

#include <fstream>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "renderer/model.hpp"
#include "renderer/shaders.hpp"
#include "renderer/texture.hpp"
#include "renderer/material.hpp"
#include "scene/entity.hpp"

using json = nlohmann::json;


template<class T, class A, class S>
using full_map = std::unordered_map<T, std::unordered_map<A, S>>;

namespace Engine::Resources{
    class ModelLoader{
        public:
            
            static ModelLoader& get(){
                static ModelLoader instance;
                return instance;
            }

            ModelLoader(const ModelLoader&) = delete;
            ModelLoader& operator=(const ModelLoader&) = delete;

            void loadFile(const std::string& path, const std::string& appInstName);
            std::string getAttrib(const std::string& model, const std::string& attribute);

            std::shared_ptr<Renderer::Model> fetchModel(const std::string& modelName);
            std::shared_ptr<Renderer::ShaderProgram> fetchShaderProgram(const std::string& entityName);
            std::shared_ptr<Renderer::Texture2D> fetchTexture(const std::string& matName,const Engine::Renderer::TextureType& type);
            std::shared_ptr<Renderer::Material> fetchMaterial(const std::string& matName);
            std::shared_ptr<Scene::Entity> fetchEntity(const std::string& entityName);

            Scene::Entity instantiate(const std::string& entityName);

            void cleanUp();

        private:
            ModelLoader() = default;
            ~ModelLoader() = default;
          
            std::unordered_map<std::string, std::string> mModelPaths;
            full_map<std::string, std::string, std::string> mShaderPaths;
            full_map<std::string, Engine::Renderer::TextureType, std::string> mTexturePaths;
            full_map<std::string, std::string, std::string> mEntityProperties;
            std::unordered_map<std::string, std::optional<Engine::Renderer::LightSourceSpec>> mLightSourceSpecs;
            std::unordered_map<std::string, std::optional<Engine::Renderer::LightSpec>> mLightSpecs;

            std::unordered_map<std::string, std::shared_ptr<Renderer::Model>> mModelCache;
            std::unordered_map<std::string, std::shared_ptr<Renderer::ShaderProgram>> mShaderCache;
            full_map<std::string, Engine::Renderer::TextureType, std::shared_ptr<Renderer::Texture2D>> mTextureCache;
            std::unordered_map<std::string, std::shared_ptr<Renderer::Material>> mMaterialCache;
            std::unordered_map<std::string, std::shared_ptr<Scene::Entity>> mEntityCache;

            void parseAssets(const json& assetJson);
            void parseEntities(const json& entityJson);
            
            // parseAssets poziva parseMaterials i parseModels
            void parseModels(const json& modelJson);
            void parseMaterials(const json& materialJson);
            
            // parseMaterials poziva getShaderPaths i getLightSpecs()
            void getShaderPaths(const json& shaderJson, const std::string& matName);
            void getLightSpecs(const json& lightJson, const std::string& matName);
            void getTexturePaths(const json& textureJson, const std::string& matName);

    };
}

#endif // __MODEL_LOADER_HPP