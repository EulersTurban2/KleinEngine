#ifndef __ENTITY_HPP
#define __ENTITY_HPP

#include <memory>
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/model.hpp"
#include "renderer/material.hpp" // Replaced shaders.hpp with material.hpp

namespace Engine{
    namespace Scene{

        struct Transform{
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 rotation = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(1.0f);

            glm::mat4 getModelMatrix() const {
                glm::mat4 mat(1.0f);
                mat = glm::translate(mat, position);
                mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                mat = glm::scale(mat, scale);
                return mat;
            };
        };

        struct Entity{
            std::string name;
            Transform transform;

            std::shared_ptr<Renderer::Model> model;
            std::shared_ptr<Renderer::Material> material;

            std::function<void(Entity&, float)> onUpdate = nullptr;
        };
    }
}

#endif