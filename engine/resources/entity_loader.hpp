#ifndef ENTITY_LOADER_HPP
#define ENTITY_LOADER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "renderer/texture.hpp"
#include "renderer/shaders.hpp"

namespace Engine::Resources {

    // Public facade of the resource system: instantiates entities described
    // in resources.json and hands out the underlying GPU resources.
    class EntityLoader {

        public:
            static EntityLoader& getInstance() {
                static EntityLoader instance;
                return instance;
            }

            EntityLoader(const EntityLoader&) = delete;
            EntityLoader& operator=(const EntityLoader&) = delete;

            Engine::Scene::Entity instantiate(const std::string& entityName, Engine::Scene::Scene& targetScene);

            std::shared_ptr<Engine::Renderer::Model> getModel(const std::string& modelName);
            std::shared_ptr<Engine::Renderer::Material> getMaterial(const std::string& matName);
            std::shared_ptr<Engine::Renderer::Texture2D> getTexture(const std::string& matName, const Engine::Renderer::TextureType& type);
            std::shared_ptr<Engine::Renderer::ShaderProgram> getShaderProgram(const std::string& matName);

            std::vector<std::string> getAllEntityNames() const;
            std::unordered_map<std::string, std::string> getEntityMetadata(const std::string& entityName) const;

            void clearCache();

        private:
            EntityLoader() = default;
            ~EntityLoader() = default;
    };
}

#endif // ENTITY_LOADER_HPP
