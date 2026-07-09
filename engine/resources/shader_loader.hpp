#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include <memory>
#include <string>

namespace Engine::Renderer {
    class ShaderProgram;
}

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

#endif // SHADER_LOADER_HPP
