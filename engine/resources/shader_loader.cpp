#include "shader_loader.hpp"
#include "core/logger.hpp"
#include <vector>

namespace Engine::Resources {

    std::shared_ptr<Engine::Renderer::ShaderProgram> ShaderLoader::getShaderProgram(const std::string& matName)
    {
        if (ResourceCache::getInstance().hasShader(matName))
        {
            return ResourceCache::getInstance().getShader(matName);
        }

        if (!ResourceDatabase::getInstance().hasShaderPaths(matName))
        {
            LOG_ERROR("Shaders for material '" + matName + "' do not have registered paths in the database.");
            return nullptr;
        }

        auto paths = ResourceDatabase::getInstance().getShaderPaths(matName);

        auto vertIt = paths.find("vertex");
        auto fragIt = paths.find("fragment");

        if (vertIt == paths.end() || fragIt == paths.end()) {
            LOG_ERROR("Vertex or fragment shader paths for material '" + matName + "' are missing.");
            return nullptr;
        }

        Engine::Renderer::Shader* vertShader = new Engine::Renderer::Shader(vertIt->second, Engine::Renderer::SHADER_TYPE::VERTEX_SHADER);
        Engine::Renderer::Shader* fragShader = new Engine::Renderer::Shader(fragIt->second, Engine::Renderer::SHADER_TYPE::FRAGMENT_SHADER);

        std::vector<Engine::Renderer::Shader*> shaders = { vertShader, fragShader };

        auto geomIt = paths.find("geometry");
        Engine::Renderer::Shader* geomShader = nullptr;
        if (geomIt != paths.end() && !geomIt->second.empty()) {
            geomShader = new Engine::Renderer::Shader(geomIt->second, Engine::Renderer::SHADER_TYPE::GEOMETRY_SHADER);
            shaders.push_back(geomShader);
        }

        auto shaderProgram = std::make_shared<Engine::Renderer::ShaderProgram>(shaders);

        delete vertShader;
        delete fragShader;
        if (geomShader) delete geomShader;

        ResourceCache::getInstance().addShader(matName, shaderProgram);

        return shaderProgram;
    }

}