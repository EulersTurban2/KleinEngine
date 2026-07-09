#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <memory>
#include <string>

namespace Engine::Renderer {
    class Model;
}

namespace Engine::Resources {

    class ModelLoader {
        private:
            static ModelLoader& getInstance() {
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

#endif // MODEL_LOADER_HPP
