#ifndef __MESH_HPP
#define __MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "renderer/shaders.hpp"

namespace Engine{
    namespace Renderer{
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoords;
            glm::vec3 tangent;
            glm::vec3 bitangent;
        };

        class Mesh{
            public:
                Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
                ~Mesh();

                Mesh (const Mesh&) = delete;
                Mesh& operator=(const Mesh&) = delete;

                Mesh (Mesh&&) noexcept;
                Mesh& operator=(Mesh&&) noexcept;

                void draw(const Renderer::ShaderProgram& shader) const;
            private:
                // Data
                std::vector<Vertex> m_vertices;
                std::vector<unsigned int> m_indices;
                // Render data
                unsigned int VAO = 0, VBO = 0, EBO = 0;
                void setupMesh();
        };
    }
}

#endif // __MESH_HPP