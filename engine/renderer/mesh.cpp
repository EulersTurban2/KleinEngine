#include "renderer/mesh.hpp"
#include "mesh.hpp"

namespace Engine{
    namespace Renderer{

        Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
            : m_vertices(vertices), m_indices(indices)
        {
            setupMesh();
        }

        Mesh::~Mesh() {
            if (VAO != 0) glDeleteVertexArrays(1, &VAO);
            if (VBO != 0) glDeleteBuffers(1, &VBO);
            if (EBO != 0) glDeleteBuffers(1, &EBO);
        }

        Mesh::Mesh(Mesh &&other) noexcept
        : m_vertices(std::move(other.m_vertices)), 
              m_indices(std::move(other.m_indices)),
              VAO(other.VAO), 
              VBO(other.VBO), 
              EBO(other.EBO) 
        {
            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        void Mesh::draw(const Renderer::ShaderProgram& shader) const {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
        Mesh &Mesh::operator=(Mesh &&other) noexcept
        {
            if (this != &other) {
                if (VAO != 0) glDeleteVertexArrays(1, &VAO);
                if (VBO != 0) glDeleteBuffers(1, &VBO);
                if (EBO != 0) glDeleteBuffers(1, &EBO);

                m_vertices = std::move(other.m_vertices);
                m_indices = std::move(other.m_indices);
                VAO = other.VAO;
                VBO = other.VBO;
                EBO = other.EBO;

                other.VAO = 0;
                other.VBO = 0;
                other.EBO = 0;
            }
            return *this;
        }
        void Mesh::setupMesh()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

            // Attribute 0: Position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            
            // Attribute 1: Normal
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

            // Attribute 2: TexCoords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

            // Attribute 3: Tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

            // Attribute 4: Bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

            glBindVertexArray(0);
        }
    }
}