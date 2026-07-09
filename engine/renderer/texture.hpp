#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <cstdint>
#include <glad/glad.h>

namespace Engine::Renderer {

    enum class TextureType : uint32_t {
        Albedo = 0,
        Normal = 1,
        Specular = 2,
        Emission = 3,
    };

    class Texture2D {
        public:
            Texture2D(const std::string& path);
            Texture2D(uint32_t width, uint32_t height);
            ~Texture2D();

            // Owns a GL texture handle: copying would double-delete it.
            Texture2D(const Texture2D&) = delete;
            Texture2D& operator=(const Texture2D&) = delete;

            void bind(uint32_t slot = 0) const;
            void unbind() const;

            uint32_t getWidth() const;
            uint32_t getHeight() const;
            uint32_t getRendererID() const;

        private:
            uint32_t mRendererID;
            std::string mFilePath;
            unsigned char* mLocalBuffer;
            int mWidth, mHeight, mBPP;
    };
}

#endif // TEXTURE_HPP
