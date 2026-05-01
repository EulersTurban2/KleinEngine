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

#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"

#include "scene/entity.hpp"

using json = nlohmann::json;


template<class T, class A, class S>
using full_map = std::unordered_map<T, std::unordered_map<A, S>>;


namespace Engine::Resources{
    class ModelLoader{
        private:
            
            static ModelLoader& getInstance(){
                static ModelLoader instance;
                return instance;
            }

            ModelLoader(const ModelLoader&) = delete;
            ModelLoader& operator=(const ModelLoader&) = delete;

            std::shared_ptr<Engine::Renderer::Model> getModel(const std::string& modelName);

            ModelLoader() = default;
            ~ModelLoader() = default;

            friend class EntityLoader;
    };
}

#endif // __MODEL_LOADER_HPP