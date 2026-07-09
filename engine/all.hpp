#ifndef ALL_HPP
#define ALL_HPP

// MISCELLANEOUS
#include "core/logger.hpp"
#include "core/timer.hpp"
#include "core/enviroment.hpp"

inline auto& env = Engine::Core::Environment::get();
using Timer = Engine::Core::Timer;

// OS specific things
#include "core/window.hpp"
#include "core/platform.hpp"
#include "core/input_manager.hpp"
#include "core/keymap.hpp"

using Input = Engine::Core::InputManager;
using Keymap = Engine::Core::Keymap;

// UI
#include "core/ui_manager.hpp"

using UIManager = Engine::Core::UIManager;

// Math
#include "math/lorentz.hpp"
#include "math/projections.hpp"

using HyperbolicProjection = Engine::Math::HyperbolicProjection;

// Scene
#include "scene/aabb.hpp"
#include "scene/camera.hpp"
#include "scene/components.hpp"
#include "scene/entity.hpp"
#include "scene/octree.hpp"
#include "scene/uniform_grid.hpp"
#include "scene/spatial_index.hpp"
#include "scene/scene.hpp"

using BoundingBox = Engine::Scene::AABB;
using NativeScriptComponent = Engine::Scene::NativeScriptComponent;
using TransformComponent = Engine::Scene::TransformComponent;
using TagComponent = Engine::Scene::TagComponent;

using Octree = Engine::Scene::Octree;
using UniformGrid = Engine::Scene::UniformGrid;

using Scene = Engine::Scene::Scene;
using Camera = Engine::Scene::Camera;
using Entity = Engine::Scene::Entity;

// Resources
#include "resources/entity_loader.hpp"

inline auto& entityLoader = Engine::Resources::EntityLoader::getInstance();

// Renderer
#include "renderer/buffer.hpp"
#include "renderer/material.hpp"
#include "renderer/mesh.hpp"
#include "renderer/model.hpp"
#include "renderer/post_processing_kernels.hpp"
#include "renderer/renderer.hpp"
#include "renderer/shaders.hpp"
#include "renderer/texture.hpp"

using ShaderProgram = Engine::Renderer::ShaderProgram;
using Material = Engine::Renderer::Material;
using Mesh = Engine::Renderer::Mesh;
using Model = Engine::Renderer::Model;
using Framebuffer = Engine::Renderer::Framebuffer;
using Texture2D = Engine::Renderer::Texture2D;
using TextureType = Engine::Renderer::TextureType;
using RenderContext = Engine::Renderer::RenderContext;

using Renderer = Engine::Renderer::Renderer;

#endif // ALL_HPP
