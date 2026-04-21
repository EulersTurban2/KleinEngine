#include "model.hpp"

namespace Engine{
    namespace Renderer{
        Model::Model(const std::string &path)
        {
            loadModel(path);
        }
        void Model::draw(const Renderer::ShaderProgram &shader) const
        {
            for(auto& mesh: m_meshes){
                mesh.draw(shader);
            }
        }
        void Model::loadModel(const std::string &path)
        {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LOG_ERROR("Assimp error: " + std::string(importer.GetErrorString()));
                return;
            }
            m_directory = path.substr(0, path.find_last_of('/'));
            processNode(scene->mRootNode, scene);
            
        }
        void Model::processNode(aiNode *node, const aiScene *scene)
        {
            for(unsigned int i = 0; i < node->mNumMeshes; i++){
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                m_meshes.push_back(processMesh(mesh, scene));
            }
            for(unsigned int i = 0; i < node->mNumChildren; i++){
                processNode(node->mChildren[i], scene);
            }
        }
        Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned> indices;

            for(unsigned i = 0;i < mesh->mNumVertices; i++){
                Vertex vert;
                // Position
                vert.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                
                // Normal
                if (mesh->HasNormals()) {
                    vert.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                }

                // Texture, Tangents, and Bitangents
                if(mesh->mTextureCoords[0]) {
                    vert.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                    
                    if (mesh->HasTangentsAndBitangents()) {
                        vert.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                        vert.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
                    }
                } else {
                    vert.texCoords = glm::vec2(0.0f, 0.0f);
                    vert.tangent = glm::vec3(0.0f);
                    vert.bitangent = glm::vec3(0.0f);
                }

                vertices.push_back(vert);
            }
        }
    }
}