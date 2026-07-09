cmake_minimum_required(VERSION 3.15)
project(KleinEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(KLEIN_WARNING_FLAGS
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall;-Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW3
find_package(glfw3 REQUIRED)

# GLM (Header-only math library)
find_package(glm REQUIRED)

# Assimp (Model loading)
find_package(assimp REQUIRED)

# nlohmann/json
find_package(nlohmann_json REQUIRED)

# -----------------------------------------------------------------------------
# Vendored third-party libraries (built without our warning flags)
# -----------------------------------------------------------------------------

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include)

# -----------------------------------------------------------------------------
# Engine Library
# -----------------------------------------------------------------------------
add_library(engine STATIC
    engine/core/window.cpp
    engine/core/input_manager.cpp
    engine/core/platform.cpp
    engine/core/timer.cpp
    engine/core/ui_manager.cpp
    engine/core/enviroment.cpp
    engine/renderer/texture.cpp
    engine/renderer/shaders.cpp
    engine/renderer/mesh.cpp
    engine/renderer/model.cpp
    engine/renderer/material.cpp
    engine/renderer/renderer.cpp
    engine/renderer/buffer.cpp
    engine/renderer/stb_image_impl.cpp
    engine/resources/entity_loader.cpp
    engine/resources/model_loader.cpp
    engine/resources/material_loader.cpp
    engine/resources/texture_loader.cpp
    engine/resources/shader_loader.cpp
    engine/resources/resource_database.cpp
    engine/scene/camera.cpp
    engine/scene/scene.cpp
    engine/scene/octree.cpp
    engine/scene/uniform_grid.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_compile_options(engine PRIVATE ${KLEIN_WARNING_FLAGS})

target_link_libraries(engine PUBLIC
    OpenGL::GL
    glfw
    glm::glm
    assimp::assimp
    nlohmann_json::nlohmann_json
    imgui
    glad
)

# Link std::filesystem for older GCC versions if necessary
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(engine PUBLIC stdc++fs)
endif()

# -----------------------------------------------------------------------------
# Application Executable
# -----------------------------------------------------------------------------

# The main application entry point and logic
add_executable(KleinEngine
    app/main.cpp
)

# Include path for the app-layer headers
target_include_directories(KleinEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/app
)

target_compile_options(KleinEngine PRIVATE ${KLEIN_WARNING_FLAGS})

# Link the application to the engine module
target_link_libraries(KleinEngine PRIVATE engine)

# -----------------------------------------------------------------------------
# Build Configuration
# -----------------------------------------------------------------------------

# The app reads engine.json from its working directory (the build directory);
# resource paths inside it are relative to that directory.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/engine.json ${CMAKE_CURRENT_BINARY_DIR}/engine.json COPYONLY)

if(MSVC)
    set_target_properties(KleinEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()
