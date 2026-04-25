#ifndef __MODEL_LOADER_HPP
#define __MODEL_LOADER_HPP

#include <fstream>
#include <string_view>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "renderer/model.hpp"
#include "renderer/shaders.hpp"

using json = nlohmann::json;

template<typename T, typename S>
using full_map = std::unordered_map<T,std::unordered_map<T,S>>;

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

            std::shared_ptr<Renderer::Model> genModel(const std::string& model);
            std::shared_ptr<Renderer::ShaderProgram> createShaderProgram(const std::string& model);

            void cleanUp();

        private:
            ModelLoader() = default;
            ~ModelLoader() = default;

            full_map<std::string, std::string> modelMap;            
            std::unordered_map<std::string, std::shared_ptr<Renderer::Model>> mModelCache;
            std::unordered_map<std::string, std::shared_ptr<Renderer::ShaderProgram>> mShaderCache;
    };
}

#endif // __MODEL_LOADER_HPP