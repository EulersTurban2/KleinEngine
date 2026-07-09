#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <memory>
#include <string>
#include <cstdint>
#include "renderer/texture.hpp"

namespace Engine::Renderer {

    class Framebuffer {
        public:
            Framebuffer(uint32_t width, uint32_t height);
            ~Framebuffer();

            // Owns GL framebuffer/renderbuffer handles: copying would double-delete them.
            Framebuffer(const Framebuffer&) = delete;
            Framebuffer& operator=(const Framebuffer&) = delete;

            void bind();
            void unbind();
            void resize(uint32_t width, uint32_t height);

            std::shared_ptr<Texture2D> getColorTexture() const { return mColorTexture; }
            uint32_t getWidth() const { return mWidth; }
            uint32_t getHeight() const { return mHeight; }

            void resolveToScreen(uint32_t windowWidth, uint32_t windowHeight);
            void handleResize(uint32_t windowWidth, uint32_t windowHeight);

            void cacheBuffer(const std::string& name, const TextureType& type);

        private:
            void invalidate();

            uint32_t mFBO = 0;
            uint32_t mDepthRBO = 0;

            std::shared_ptr<Texture2D> mColorTexture;

            uint32_t mWidth, mHeight;
    };
}

#endif // BUFFER_HPP
