#ifndef __SHADERS_HPP
#define __SHADERS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <glad/glad.h>

namespace Engine
{
    namespace Resources
    {
        enum SHADER_TYPE{
            VERTEX_SHADER = GL_VERTEX_SHADER,
            GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
            FRAGMENT_SHADER = GL_FRAGMENT_SHADER
        };

        class Shader{
            public:

                Shader(const char* shaderPath, SHADER_TYPE type);
                ~Shader(){
                    glDeleteShader(this->id);
                }

                unsigned int getID() const;
                
            private:
                SHADER_TYPE type;
                unsigned int id;
            
                int success;
                char infoLog[512];
                char* readShaderSource(const char* shaderPath);

        };

        class ShaderProgram{
            public:
                
                ShaderProgram(const std::vector<Shader*>& shaders);
                ~ShaderProgram(){
                    glDeleteProgram(this->id);
                }

                void use() const{
                    glUseProgram(this->id);
                }

            private:
                unsigned int id;

                int success;
                char infoLog[512];
        };
    } // namespace Resources
    
} // namespace Engine


#endif