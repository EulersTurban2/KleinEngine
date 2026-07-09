#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include <memory>
#include <string>

#include "renderer/texture.hpp"

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

#endif // TEXTURE_LOADER_HPP
