#ifndef __MATERIAL_LOADER
#define __MATERIAL_LOADER

#include <memory>

#include "renderer/material.hpp"

namespace Engine::Resources{
    class MaterialLoader{
        private:
            static MaterialLoader& getInstance(){
                static MaterialLoader instance;
                return instance;
            }

            MaterialLoader(const MaterialLoader&) = delete;
            MaterialLoader& operator=(const MaterialLoader&) = delete;

            MaterialLoader() = default;
            ~MaterialLoader() = default;

            std::shared_ptr<Engine::Renderer::Material> getMaterial(const std::string& matName);

            friend class EntityLoader;
    };
}

#endif