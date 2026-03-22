#include <core/shaders.hpp>

namespace Engine
{
    namespace Resources{

        Shader::Shader(const char* shaderPath, SHADER_TYPE type){
            this->type = type;
            const char* shaderSource = readShaderSource(shaderPath);
            this->id = glCreateShader(type);

            glShaderSource(this->id,1,&shaderSource,NULL);
            glCompileShader(this->id);

            glGetShaderiv(this->id, GL_COMPILE_STATUS, &this->success);
            if(!this->success)
            {
                glGetShaderInfoLog(this->id, 512, NULL, this->infoLog);
                std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << this->infoLog << std::endl;
            }

        }

        char* Shader::readShaderSource(const char* shaderPath){
            std::string shaderCode;
            std::ifstream shaderFile;

            shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try
            {
                shaderFile.open(shaderPath);
                std::stringstream shaderStream;
                shaderStream << shaderFile.rdbuf();
                shaderFile.close();
                shaderCode = shaderStream.str();
            }
            catch (std::ifstream::failure& e)
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << e.what() << std::endl;
            }
            char* code = new char[shaderCode.length() + 1];
            strcpy(code,shaderCode.c_str());
            return code;
        }

        unsigned int Shader::getID() const{
            return this->id;
        }        


        ShaderProgram::ShaderProgram(const std::vector<Shader*>& shaders){
            this->id = glCreateProgram();
            for (auto shader: shaders)
            {
                glAttachShader(this->id, shader->getID());
            }
            glLinkProgram(this->id);

            glGetProgramiv(this->id, GL_LINK_STATUS,&this->success);
            if (!this->success){
                glGetProgramInfoLog(this->id,512,NULL, this->infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << this->infoLog << std::endl;
            }

            for(auto shader: shaders){
                delete shader;
            }
        }

    }
} // namespace Engine
