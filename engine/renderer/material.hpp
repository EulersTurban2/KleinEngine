#ifndef __MATERIAL_HPP
#define __MATERIAL_HPP

#include "scene/camera.hpp"
#include "renderer/texture.hpp"
#include "renderer/shaders.hpp"

#include <memory>
#include <optional>
#include <functional>
#include <glm/glm.hpp>
#include <unordered_map>

namespace Engine{
    namespace Renderer{

        struct RenderContext {
            const Engine::Camera::Camera& cam;
            float time;
            float deltaTime;

            // as the engine grows we will add more features here in the render context
        };

        struct LightSpec{
            float ambient = 0.5f;
            float diffuse = 1.0f;
            float spec = 1.0f;
        };

        struct LightSourceSpec{
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;
            float emission_strength = 0.0f;
        };

        enum class TextureType : uint32_t {
            Albedo = 0,
            Normal = 1,
            Specluar = 2,
            Emission = 3,
        };

        class Material{
            public:
                Material(std::shared_ptr<ShaderProgram> sp);
                ~Material() = default;

                void addTexture(TextureType type, std::shared_ptr<Texture2D> tex);

                void setLightSpec(const LightSpec& spec);
                void setLightSourceSpec(const LightSourceSpec& spec);

                void apply() const;

                std::shared_ptr<ShaderProgram> getShader() const {return mShader;}
                std::function<void(std::shared_ptr<ShaderProgram>, const RenderContext)> onUse = nullptr;
            private:
                std::shared_ptr<ShaderProgram> mShader;
                std::unordered_map<TextureType, std::shared_ptr<Texture2D>> mTextures;

                std::optional<LightSpec> mLightSpec;
                std::optional<LightSourceSpec> mLightSourceSpec;
        };
    }
}

#endif