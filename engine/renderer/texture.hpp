#ifndef __TEXTURE_HPP
#define __TEXTURE_HPP

#include <string>
#include <cstdint>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/logger.hpp"

namespace Engine{
    namespace Renderer{
        class Texture2D{
            public:
                Texture2D(const std::string& path);
                ~Texture2D();

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
}

#endif