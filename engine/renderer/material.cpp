#include "renderer/material.hpp"
#include <string>

namespace Engine {
    namespace Renderer {

        Material::Material(std::shared_ptr<ShaderProgram> sp) 
            : mShader(sp) 
        {
        }

        void Material::addTexture(TextureType type, std::shared_ptr<Texture2D> tex) {
            mTextures[type] = tex;
        }

        void Material::setLightSpec(const LightSpec& spec) {
            mLightSpec = spec;
        }

        void Material::setLightSourceSpec(const LightSourceSpec& spec) {
            mLightSourceSpec = spec;
        }

        
        void Material::apply() const {
            if (!mShader) return;

            mShader->use();

            for (const auto& [type, texture] : mTextures) {
                if (texture) {
                    uint32_t slot = static_cast<uint32_t>(type);
                    
                    texture->bind(slot); 

                    std::string uniformName;
                    switch (type) {
                        case TextureType::Albedo:   
                            uniformName = "u_AlbedoMap"; 
                            break;
                        case TextureType::Normal:   
                            uniformName = "u_NormalMap"; 
                            break;
                        case TextureType::Specluar:
                            uniformName = "u_SpecularMap"; 
                            break; 
                        case TextureType::Emission: 
                            uniformName = "u_EmissionMap"; 
                            break;
                    }

                    mShader->setInt(uniformName, slot); 
                }
            }

            if (mLightSpec.has_value()) {
                const auto& spec = mLightSpec.value();
                mShader->setFloat("u_LightSpec.ambient", spec.ambient);
                mShader->setFloat("u_LightSpec.diffuse", spec.diffuse);
                mShader->setFloat("u_LightSpec.specular", spec.spec);
            }

            if (mLightSourceSpec.has_value()) {
                const auto& spec = mLightSourceSpec.value();
                mShader->setVec3("u_LightSourceSpec.color", spec.color);
                mShader->setFloat("u_LightSourceSpec.intensity", spec.intensity);
                mShader->setFloat("u_LightSourceSpec.emission_strength", spec.emission_strength);
            }
        }

    }
}