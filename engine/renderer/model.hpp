#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>

#include "mesh.hpp"

struct aiNode;
struct aiMesh;
struct aiScene;

namespace Engine::Renderer {

    class Model {
        public:
            Model(const std::string& path);

            // Draws every mesh; the caller binds the shader beforehand.
            void draw() const;

        private:
            std::vector<Mesh> mMeshes;
            std::string mDirectory;

            void loadModel(const std::string& path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    };
}

#endif // MODEL_HPP
