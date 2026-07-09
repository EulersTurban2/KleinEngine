#include "renderer/material.hpp"
#include <string>

namespace Engine::Renderer {

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
                        uniformName = "uAlbedoMap";
                        break;
                    case TextureType::Normal:
                        uniformName = "uNormalMap";
                        break;
                    case TextureType::Specular:
                        uniformName = "uSpecularMap";
                        break;
                    case TextureType::Emission:
                        uniformName = "uEmissionMap";
                        break;
                }

                mShader->setInt(uniformName, static_cast<int>(slot));
            }
        }

        if (mLightSpec.has_value()) {
            const auto& spec = mLightSpec.value();
            mShader->setFloat("uLightSpec.ambient", spec.ambient);
            mShader->setFloat("uLightSpec.diffuse", spec.diffuse);
            mShader->setFloat("uLightSpec.specular", spec.specular);
        }

        if (mLightSourceSpec.has_value()) {
            const auto& spec = mLightSourceSpec.value();
            mShader->setVec3("uLightSourceSpec.color", spec.color);
            mShader->setFloat("uLightSourceSpec.intensity", spec.intensity);
            mShader->setFloat("uLightSourceSpec.emission_strength", spec.emission_strength);
        }
    }
}
