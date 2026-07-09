#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine::Renderer {

    enum class ShaderType : GLenum {
        Vertex = GL_VERTEX_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

    class Shader {
        public:
            Shader(std::string_view shaderPath, ShaderType type);
            ~Shader() {
                glDeleteShader(mId);
            }

            // Owns a GL shader handle: copying would double-delete it.
            Shader(const Shader&) = delete;
            Shader& operator=(const Shader&) = delete;

            unsigned int getID() const { return mId; }
            ShaderType getType() const { return mType; }

        private:
            unsigned int mId;
            ShaderType mType;
            std::string readShaderSource(std::string_view path);
    };

    class ShaderProgram {
        public:
            ShaderProgram(const std::vector<Shader*>& shaders);
            ~ShaderProgram();

            // Owns a GL program handle: copying would double-delete it.
            ShaderProgram(const ShaderProgram&) = delete;
            ShaderProgram& operator=(const ShaderProgram&) = delete;

            void use() const;
            void unbind() const;

            void setBool(const std::string& name, bool value) const;
            void setInt(const std::string& name, int value) const;
            void setFloat(const std::string& name, float value) const;
            void setVec2(const std::string& name, const glm::vec2& value) const;
            void setVec3(const std::string& name, const glm::vec3& value) const;
            void setVec4(const std::string& name, const glm::vec4& value) const;
            void setMat2(const std::string& name, const glm::mat2& value) const;
            void setMat3(const std::string& name, const glm::mat3& value) const;
            void setMat4(const std::string& name, const glm::mat4& value) const;

            unsigned int getID() const { return mId; }
            int getUniformLocation(const std::string& name) const;

        private:
            unsigned int mId;
            mutable std::unordered_map<std::string, int> mUniformLocations;
    };
}

#endif // SHADERS_HPP
