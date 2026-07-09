#ifndef MATERIAL_LOADER_HPP
#define MATERIAL_LOADER_HPP

#include <memory>
#include <string>

namespace Engine::Renderer {
    class Material;
}

namespace Engine::Resources {

    class MaterialLoader {
        private:
            static MaterialLoader& getInstance() {
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

#endif // MATERIAL_LOADER_HPP
