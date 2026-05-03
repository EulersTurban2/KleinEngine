#ifndef __RESOURCE_CACHE_HPP
#define __RESOURCE_CACHE_HPP

#include <memory>
#include <unordered_map>

#include "core/logger.hpp"
#include "renderer/model.hpp"
#include "renderer/shaders.hpp"
#include "renderer/texture.hpp"
#include "renderer/material.hpp"

#include "resources/model_loader.hpp"

#include "scene/entity.hpp"

template<class T, class A, class S>
using full_map = std::unordered_map<T, std::unordered_map<A, S>>;

namespace Engine
{
    namespace Resources
    {
        class ResourceCache {

                friend class ModelLoader;
                friend class MaterialLoader;
                friend class TextureLoader;
                friend class ShaderLoader;
                friend class EntityLoader;

            private:
                static ResourceCache& getInstance() {
                    static ResourceCache instance;
                    return instance;
                }

                // Teksture
                bool hasTexture(const std::string& name,const Engine::Renderer::TextureType& type) {
                    return mTextures.find(name) != mTextures.end() && mTextures[name].find(type) != mTextures[name].end();
                }

                void addTexture(const std::string& name,const Engine::Renderer::TextureType& type, std::shared_ptr<Engine::Renderer::Texture2D> texture) {
                    if (!hasTexture(name,type))
                    {
                        mTextures[name][type] = texture;
                    } else {
                        LOG_WARN("Texture in material " + name + "of that type exists");
                    }
                    
                }

                std::shared_ptr<Engine::Renderer::Texture2D> getTexture(const std::string& name, const Engine::Renderer::TextureType& type){
                    if(!hasTexture(name,type)){
                        LOG_ERROR("There exists no texture of the type in material " + name);
                        return nullptr;
                    }
                    return mTextures[name][type];
                }

                // Modeli

                bool hasModel(const std::string& modelName) {
                    return mModels.find(modelName) != mModels.end();
                }

                void addModel(const std::string& modelName, std::shared_ptr<Engine::Renderer::Model> model){
                    if (!hasModel(modelName))
                    {
                        mModels[modelName] = model;
                    } else {
                        LOG_WARN("Model " + modelName + " already exists");
                    }
                }
                
                std::shared_ptr<Engine::Renderer::Model> getModel(const std::string& modelName){
                    if (!hasModel(modelName))
                    {
                        LOG_ERROR("There isn't a model of " + modelName + " name");
                        return nullptr;
                    }
                    return mModels[modelName];
                }

                // Shader!

                bool hasShader(const std::string& shaderName) {
                    return mShaders.find(shaderName) != mShaders.end();
                }

                void addShader(const std::string& shaderName, std::shared_ptr<Engine::Renderer::ShaderProgram> shader){
                    if (!hasShader(shaderName))
                    {
                        mShaders[shaderName] = shader;
                    } else {
                        LOG_WARN("Shader " + shaderName + " already exists");
                    }
                }
                
                std::shared_ptr<Engine::Renderer::ShaderProgram> getShader(const std::string& shaderName){
                    if (!hasShader(shaderName))
                    {
                        LOG_ERROR("There isn't a shader of " + shaderName + " name");
                        return nullptr;
                    }
                    return mShaders[shaderName];
                }

                // Materijali

                bool hasMaterial(const std::string& matName) {
                    return mMaterials.find(matName) != mMaterials.end();
                }

                void addMaterial(const std::string& matName, std::shared_ptr<Engine::Renderer::Material> material){
                    if (!hasMaterial(matName))
                    {
                        mMaterials[matName] = material;
                    } else {
                        LOG_WARN("Material " + matName + " already exists");
                    }
                }
                
                std::shared_ptr<Engine::Renderer::Material> getMaterial(const std::string& matName){
                    if (!hasMaterial(matName))
                    {
                        LOG_ERROR("There isn't a material of " + matName + " name");
                        return nullptr;
                    }
                    return mMaterials[matName];
                }

                // Ciscenje
                void clearAll() {
                    mTextures.clear();
                    mModels.clear();
                    mShaders.clear();
                    mMaterials.clear();
                }

                ResourceCache() = default;
                ~ResourceCache() { clearAll(); }

                ResourceCache(const ResourceCache&) = delete;
                ResourceCache& operator=(const ResourceCache&) = delete;

                full_map<std::string, Engine::Renderer::TextureType, std::shared_ptr<Engine::Renderer::Texture2D>> mTextures;
                std::unordered_map<std::string, std::shared_ptr<Engine::Renderer::Model>> mModels;
                std::unordered_map<std::string, std::shared_ptr<Engine::Renderer::ShaderProgram>> mShaders;
                std::unordered_map<std::string, std::shared_ptr<Engine::Renderer::Material>> mMaterials;
        };
    } 
} 

#endif