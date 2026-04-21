#ifndef __MODEL_LOADER_HPP
#define __MODEL_LOADER_HPP

#include <string_view>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "renderer/model.hpp"

template<typename T, typename S>
using full_map = std::unordered_map<T,std::unordered_map<T,S>>;


namespace Engine::Resources{
    class ModelLoader{
        public:
            
            ModelLoader& get(){
                static ModelLoader instance;
                return instance;
            }

            ModelLoader(const ModelLoader&) = delete;
            ModelLoader& operator=(const ModelLoader&) = delete;



        private:
            ModelLoader();
            ~ModelLoader();

            full_map<std::string, std::string_view> modelMap;            
    };
}

#endif // __MODEL_LOADER_HPP