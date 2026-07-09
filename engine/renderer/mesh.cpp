#include "mesh.hpp"

#include <glad/glad.h>
#include <cstddef>

namespace Engine::Renderer {

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
        : mVertices(vertices), mIndices(indices)
    {
        setupMesh();
    }

    Mesh::~Mesh() {
        if (mVAO != 0) glDeleteVertexArrays(1, &mVAO);
        if (mVBO != 0) glDeleteBuffers(1, &mVBO);
        if (mEBO != 0) glDeleteBuffers(1, &mEBO);
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : mVertices(std::move(other.mVertices)),
          mIndices(std::move(other.mIndices)),
          mVAO(other.mVAO),
          mVBO(other.mVBO),
          mEBO(other.mEBO)
    {
        other.mVAO = 0;
        other.mVBO = 0;
        other.mEBO = 0;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept {
        if (this != &other) {
            if (mVAO != 0) glDeleteVertexArrays(1, &mVAO);
            if (mVBO != 0) glDeleteBuffers(1, &mVBO);
            if (mEBO != 0) glDeleteBuffers(1, &mEBO);

            mVertices = std::move(other.mVertices);
            mIndices = std::move(other.mIndices);
            mVAO = other.mVAO;
            mVBO = other.mVBO;
            mEBO = other.mEBO;

            other.mVAO = 0;
            other.mVBO = 0;
            other.mEBO = 0;
        }
        return *this;
    }

    void Mesh::draw() const {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::setupMesh() {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

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
