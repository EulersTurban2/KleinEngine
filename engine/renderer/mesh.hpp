#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
#include <vector>

namespace Engine::Renderer {

    struct Vertex {
        glm::vec3 position{0.0f};      // flat model position: used by the Euclidean path
        glm::vec3 normal{0.0f};
        glm::vec2 texCoords{0.0f};
        glm::vec3 tangent{0.0f};
        glm::vec3 bitangent{0.0f};
        glm::vec3 hyperPosition{0.0f}; // geodesic position: used by the hyperbolic path
    };

    class Mesh {
        public:
            Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
            ~Mesh();

            Mesh(const Mesh&) = delete;
            Mesh& operator=(const Mesh&) = delete;

            Mesh(Mesh&&) noexcept;
            Mesh& operator=(Mesh&&) noexcept;

            // Issues the draw call; the caller is responsible for binding the
            // shader and its uniforms beforehand.
            void draw() const;

        private:
            // Data
            std::vector<Vertex> mVertices;
            std::vector<unsigned int> mIndices;
            // Render data
            unsigned int mVAO = 0, mVBO = 0, mEBO = 0;
            void setupMesh();
    };
}

#endif // MESH_HPP
