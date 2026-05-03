#ifndef __ENTITY_LOADER_HPP
#define __ENTITY_LOADER_HPP

#include <string>
#include <memory>

#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "renderer/texture.hpp"
#include "renderer/shaders.hpp"

namespace Engine::Resources {
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

            void clearCache();

        private:
            EntityLoader() = default;
            ~EntityLoader() = default;
    };
}

#endif // __ENTITY_LOADER_HPP