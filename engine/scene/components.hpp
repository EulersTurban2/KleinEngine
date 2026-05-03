#ifndef __COMPONENTS_HPP
#define __COMPONENTS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <string>

#include "renderer/model.hpp"
#include "renderer/material.hpp"

namespace Engine::Scene {

    class Entity;

    struct TransformComponent {
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale    = { 1.0f, 1.0f, 1.0f };

        glm::mat4 getModelMatrix() const {
            glm::mat4 mat(1.0f);
            mat = glm::translate(mat, position);
            mat = glm::rotate(mat, glm::radians(rotation.x), { 1.0f, 0.0f, 0.0f });
            mat = glm::rotate(mat, glm::radians(rotation.y), { 0.0f, 1.0f, 0.0f });
            mat = glm::rotate(mat, glm::radians(rotation.z), { 0.0f, 0.0f, 1.0f });
            mat = glm::scale(mat, scale);
            return mat;
        }
    };

    struct MeshRendererComponent {
        std::shared_ptr<Renderer::Model> model;
        std::shared_ptr<Renderer::Material> material;
    };

    struct PointLightComponent {
        glm::vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
        float radius = 40.0f;
    };

    struct TagComponent {
        std::string name;
    };

    // The Flat Scene Graph
    struct RelationshipComponent {
        uint32_t parent = 0;       // 0 means no parent (Root)
        uint32_t firstChild = 0;   
        uint32_t nextSibling = 0;  
        uint32_t prevSibling = 0;  
    };

    struct NativeScriptComponent {
        std::function<void(Entity&, float)> onUpdate = nullptr;
    };

}

#endif