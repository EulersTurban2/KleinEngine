#include "model.hpp"
#include "core/logger.hpp"
#include "renderer/tessellate.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine::Renderer {

    // Geodesic subdivision depth applied to every loaded mesh (2^level segments
    // per edge). Keeps Poincare/Klein edges smooth; a cube (12 tris) -> 768.
    static constexpr int kHyperbolicTessLevel = 3;

    Model::Model(const std::string& path) {
        loadModel(path);
    }

    void Model::draw() const {
        for (auto& mesh : mMeshes) {
            mesh.draw();
        }
    }

    void Model::loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            LOG_ERROR("Assimp error: " + std::string(importer.GetErrorString()));
            return;
        }
        mDirectory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            mMeshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* /*scene*/) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Vertex members default to zero; only present attributes are filled in.
            Vertex vert;
            vert.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            // Original vertices coincide in both spaces; tessellation splits the
            // flat and geodesic tracks apart from here.
            vert.hyperPosition = vert.position;

            if (mesh->HasNormals()) {
                vert.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }

            if (mesh->mTextureCoords[0]) {
                vert.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

                if (mesh->HasTangentsAndBitangents()) {
                    vert.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                    vert.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
                }
            }

            vertices.push_back(vert);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Subdivide along hyperboloid geodesics so curved (Poincare) edges stay
        // smooth. The vertex shader still just lifts + flattens each vertex.
        tessellateHyperbolic(vertices, indices, kHyperbolicTessLevel);

        return Mesh(vertices, indices);
    }
}
