#include "shader_loader.hpp"
#include "resources/resource_cache.hpp"
#include "resources/resource_database.hpp"
#include "renderer/shaders.hpp"
#include "core/logger.hpp"

#include <optional>
#include <vector>

namespace Engine::Resources {

    std::shared_ptr<Engine::Renderer::ShaderProgram> ShaderLoader::getShaderProgram(const std::string& matName) {
        if (ResourceCache::getInstance().hasShader(matName)) {
            return ResourceCache::getInstance().getShader(matName);
        }

        if (!ResourceDatabase::getInstance().hasShaderPaths(matName)) {
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

        // The stage objects only need to outlive program linking.
        Engine::Renderer::Shader vertShader(vertIt->second, Engine::Renderer::ShaderType::Vertex);
        Engine::Renderer::Shader fragShader(fragIt->second, Engine::Renderer::ShaderType::Fragment);

        std::vector<Engine::Renderer::Shader*> shaders = { &vertShader, &fragShader };

        std::optional<Engine::Renderer::Shader> geomShader;
        auto geomIt = paths.find("geometry");
        if (geomIt != paths.end() && !geomIt->second.empty()) {
            geomShader.emplace(geomIt->second, Engine::Renderer::ShaderType::Geometry);
            shaders.push_back(&geomShader.value());
        }

        auto shaderProgram = std::make_shared<Engine::Renderer::ShaderProgram>(shaders);

        ResourceCache::getInstance().addShader(matName, shaderProgram);

        return shaderProgram;
    }

}
