#ifndef __TEXTURE_LOADER_HPP
#define __TEXTURE_LOADER_HPP

#include <string>
#include <memory>

#include "renderer/texture.hpp"
#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"

namespace Engine::Resources {
    class TextureLoader {
        private:
            
            static TextureLoader& getInstance() {
                static TextureLoader instance;
                return instance;
            }

            TextureLoader(const TextureLoader&) = delete;
            TextureLoader& operator=(const TextureLoader&) = delete;

            std::shared_ptr<Engine::Renderer::Texture2D> getTexture(const std::string& matName, const Engine::Renderer::TextureType& type);

            TextureLoader() = default;
            ~TextureLoader() = default;

            friend class MaterialLoader;
            friend class EntityLoader;
    };
}

#endif // __TEXTURE_LOADER_HPP