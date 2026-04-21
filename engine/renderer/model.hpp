#ifndef __MODEL_HPP
#define __MODEL_HPP

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "mesh.hpp"
#include "shaders.hpp"
#include "core/logger.hpp"

namespace Engine{
    namespace Renderer{
        class Model{
            public:
                Model(const std::string& path);
                void draw(const Renderer::ShaderProgram& shader) const;
                
            private:
                std::vector<Mesh> m_meshes;
                std::string m_directory;

                void loadModel(const std::string& path);
                void processNode(aiNode *node, const aiScene *scene);
                Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        };
    }
}


#endif // __MODEL_HPP