#include "buffer.hpp"
#include "core/logger.hpp"
#include "resources/resource_cache.hpp"

namespace Engine::Renderer {

    Framebuffer::Framebuffer(uint32_t width, uint32_t height) {
        mWidth = width;
        mHeight = height;
        invalidate();
    }

    Framebuffer::~Framebuffer() {
        if (mFBO) glDeleteFramebuffers(1, &mFBO);
        if (mDepthRBO) glDeleteRenderbuffers(1, &mDepthRBO);
    }

    void Framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
        glViewport(0, 0, static_cast<GLsizei>(mWidth), static_cast<GLsizei>(mHeight));
    }

    void Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::resize(uint32_t width, uint32_t height) {
        mWidth = width;
        mHeight = height;
        invalidate();
    }

    void Framebuffer::resolveToScreen(uint32_t windowWidth, uint32_t windowHeight) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, static_cast<GLint>(mWidth), static_cast<GLint>(mHeight),
            0, 0, static_cast<GLint>(windowWidth), static_cast<GLint>(windowHeight),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    void Framebuffer::handleResize(uint32_t windowWidth, uint32_t windowHeight) {
        if (windowWidth != getWidth() || windowHeight != getHeight()) {
            resize(windowWidth, windowHeight);
        }
    }

    void Framebuffer::cacheBuffer(const std::string& name, const TextureType& type) {
        Engine::Resources::ResourceCache::getInstance().addTexture(name, type, mColorTexture);
    }

    void Framebuffer::invalidate() {
        if (mFBO) {
            glDeleteFramebuffers(1, &mFBO);
            glDeleteRenderbuffers(1, &mDepthRBO);
        }

        glGenFramebuffers(1, &mFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

        mColorTexture = std::make_shared<Texture2D>(mWidth, mHeight);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTexture->getRendererID(), 0);

        glGenRenderbuffers(1, &mDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(mWidth), static_cast<GLsizei>(mHeight));
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOG_CRITICAL("Framebuffer is incomplete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
