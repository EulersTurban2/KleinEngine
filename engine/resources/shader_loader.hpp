#ifndef __SHADER_LOADER_HPP
#define __SHADER_LOADER_HPP

#include <string>
#include <memory>

#include "renderer/shaders.hpp"
#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"

namespace Engine::Resources {
    class ShaderLoader {
        private:
            
            static ShaderLoader& getInstance() {
                static ShaderLoader instance;
                return instance;
            }

            ShaderLoader(const ShaderLoader&) = delete;
            ShaderLoader& operator=(const ShaderLoader&) = delete;

            std::shared_ptr<Engine::Renderer::ShaderProgram> getShaderProgram(const std::string& matName);

            ShaderLoader() = default;
            ~ShaderLoader() = default;

            friend class MaterialLoader;
            friend class EntityLoader;
    };
}

#endif // __SHADER_LOADER_HPP