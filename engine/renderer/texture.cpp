#include "texture.hpp"
#include "core/logger.hpp"
#include <stb_image.h>

namespace Engine::Renderer {

    Texture2D::Texture2D(const std::string& path)
        : mRendererID(0), mFilePath(path), mLocalBuffer(nullptr), mWidth(0), mHeight(0), mBPP(0)
    {
        stbi_set_flip_vertically_on_load(0);
        mLocalBuffer = stbi_load(path.c_str(), &mWidth, &mHeight, &mBPP, 4);

        glGenTextures(1, &mRendererID);
        glBindTexture(GL_TEXTURE_2D, mRendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (mLocalBuffer) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(mLocalBuffer);
            mLocalBuffer = nullptr;
        } else {
            LOG_ERROR("Failed to load texture at path: " + path);
        }
    }

    Texture2D::Texture2D(uint32_t width, uint32_t height)
        : mRendererID(0), mFilePath(""), mLocalBuffer(nullptr),
          mWidth(static_cast<int>(width)), mHeight(static_cast<int>(height)), mBPP(0)
    {
        glGenTextures(1, &mRendererID);
        glBindTexture(GL_TEXTURE_2D, mRendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    Texture2D::~Texture2D() {
        glDeleteTextures(1, &mRendererID);
    }

    void Texture2D::bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, mRendererID);
    }

    void Texture2D::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    uint32_t Texture2D::getWidth() const {
        return static_cast<uint32_t>(mWidth);
    }

    uint32_t Texture2D::getHeight() const {
        return static_cast<uint32_t>(mHeight);
    }

    uint32_t Texture2D::getRendererID() const {
        return mRendererID;
    }
}
