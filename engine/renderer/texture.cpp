#include "texture.hpp"

Engine::Renderer::Texture2D::Texture2D(const std::string &path): mRendererID(0), mFilePath(path), mLocalBuffer(nullptr), mWidth(0), mHeight(0), mBPP(0)

{   
    stbi_set_flip_vertically_on_load(1);
    mLocalBuffer = stbi_load(path.c_str(), &mWidth, &mHeight,&mBPP,4);

    glGenTextures(1,&mRendererID);

    glBindTexture(GL_TEXTURE_2D, mRendererID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (mLocalBuffer)
    {
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,mWidth,mHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,mLocalBuffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(mLocalBuffer);
    }else{
        LOG_ERROR("Failed to load texture at path: " + path);
    }
}

Engine::Renderer::Texture2D::~Texture2D(){
    glDeleteTextures(1, &mRendererID);
}

void Engine::Renderer::Texture2D::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D,mRendererID);
}

void Engine::Renderer::Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D,0);
}

uint32_t Engine::Renderer::Texture2D::getWidth() const
{
    return mWidth;
}

uint32_t Engine::Renderer::Texture2D::getHeight() const
{
    return mHeight;
}

uint32_t Engine::Renderer::Texture2D::getRendererID() const
{
    return mRendererID;
}
