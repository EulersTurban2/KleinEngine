#ifndef __SHADERS_HPP
#define __SHADERS_HPP

#include <string>
#include <sstream>
#include <string_view>

#include <fstream>
#include <vector>

#include <filesystem>
namespace fs = std::filesystem;

#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/logger.hpp"

namespace Engine
{
    namespace Renderer
    {
        enum SHADER_TYPE{
            VERTEX_SHADER = GL_VERTEX_SHADER,
            GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
            FRAGMENT_SHADER = GL_FRAGMENT_SHADER
        };

        class Shader{
            public:

                Shader(std::string_view shaderPath, SHADER_TYPE type);
                ~Shader(){
                    glDeleteShader(this->id);
                }

                unsigned int getID() const {return this->id;}
                SHADER_TYPE getType() const { return this->_type; }

            private:
                unsigned int id;
                SHADER_TYPE _type;
                std::string readShaderSource(std::string_view path);
        };

        class ShaderProgram{
            public:
                
                ShaderProgram(const std::vector<Shader*>& shaders);
                ~ShaderProgram();

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

                // Vrati nam ID shader programa
                unsigned int getID() const { return this->id; }
                int getUniformLocation(const std::string& name) const;
            private:
                unsigned int id;
                mutable std::unordered_map<std::string, int> uniformLocations;
                void checkCompileErrors(unsigned int shader, std::string_view type) const;
        };
    } // namespace Resources
    
} // namespace Engine


#endif