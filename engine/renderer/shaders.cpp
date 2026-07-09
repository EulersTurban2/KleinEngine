#include "shaders.hpp"
#include "core/logger.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace Engine::Renderer {

    namespace fs = std::filesystem;

    Shader::Shader(std::string_view shaderPath, ShaderType type) {
        std::string shaderStrSource = readShaderSource(shaderPath);
        const char* shaderSource = shaderStrSource.c_str();
        mId = glCreateShader(static_cast<GLenum>(type));
        mType = type;
        glShaderSource(mId, 1, &shaderSource, nullptr);
        glCompileShader(mId);

        int success;
        char infoLog[512];
        glGetShaderiv(mId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(mId, 512, nullptr, infoLog);
            LOG_ERROR("Shader compilation failed: \n" + std::string(infoLog));
        }
    }

    std::string Shader::readShaderSource(std::string_view shaderPath) {
        std::string shaderCode;
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            shaderFile.open(std::string(shaderPath));
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            // Shaders live in <root>/{vertex,fragment,...}; includes resolve
            // against <root>/imports.
            fs::path shaderDir = fs::path(shaderPath).parent_path().parent_path();
            std::string line;
            while (std::getline(shaderStream, line)) {
                if (line.find("#include ") != std::string::npos) {
                    auto firstChar = line.find('<');
                    auto lastChar = line.find('>');
                    if (firstChar != std::string::npos && lastChar != std::string::npos && firstChar < lastChar) {
                        std::string includeName = line.substr(firstChar + 1, lastChar - firstChar - 1);
                        fs::path importPath = shaderDir / "imports" / includeName.append(".glsl");
                        std::string code = readShaderSource(importPath.string());
                        shaderCode += code;
                        shaderCode += "\n";
                        LOG_INFO("Included shader: " + importPath.string());
                    } else {
                        LOG_ERROR("Invalid include directive: " + line);
                    }
                } else {
                    shaderCode += line + "\n";
                }
            }
            return shaderCode;
        } catch (const std::ifstream::failure& e) {
            LOG_ERROR("Failed to read shader file: " + std::string(e.what()));
            return "";
        }
    }

    /**
     * ShaderProgram: a linked GL program built from several shader stages.
     */
    ShaderProgram::ShaderProgram(const std::vector<Shader*>& shaders) {
        mId = glCreateProgram();
        std::unordered_map<ShaderType, bool> attachedType;
        for (auto shader : shaders) {
            if (!shader) continue;

            ShaderType type = shader->getType();
            if (attachedType[type]) {
                LOG_WARN("Shader program linking warning: Multiple shaders of type " + std::to_string(static_cast<GLenum>(type)) + " attached.");
                continue;
            }
            attachedType[type] = true;

            glAttachShader(mId, shader->getID());
        }
        glLinkProgram(mId);
        int success;
        char infoLog[512];
        glGetProgramiv(mId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(mId, 512, nullptr, infoLog);
            LOG_ERROR("Shader program linking failed: \n" + std::string(infoLog));
        }
    }

    ShaderProgram::~ShaderProgram() {
        glDeleteProgram(mId);
    }

    void ShaderProgram::use() const {
        glUseProgram(mId);
    }

    void ShaderProgram::unbind() const {
        glUseProgram(0);
    }

    int ShaderProgram::getUniformLocation(const std::string& name) const {
        auto it = mUniformLocations.find(name);
        if (it != mUniformLocations.end()) {
            return it->second;
        }
        int location = glGetUniformLocation(mId, name.c_str());
        mUniformLocations[name] = location;
        return location;
    }

    // Uniform setters
    void ShaderProgram::setBool(const std::string& name, bool value) const {
        glUniform1i(getUniformLocation(name), (int)value);
    }
    void ShaderProgram::setInt(const std::string& name, int value) const {
        glUniform1i(getUniformLocation(name), value);
    }
    void ShaderProgram::setFloat(const std::string& name, float value) const {
        glUniform1f(getUniformLocation(name), value);
    }
    void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }
    void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }
    void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }
    void ShaderProgram::setMat2(const std::string& name, const glm::mat2& value) const {
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }
    void ShaderProgram::setMat3(const std::string& name, const glm::mat3& value) const {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }
    void ShaderProgram::setMat4(const std::string& name, const glm::mat4& value) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }
}
