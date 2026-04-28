cmake_minimum_required(VERSION 3.15)
project(KleinEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
# Engine Library
# -----------------------------------------------------------------------------
add_library(engine STATIC
    engine/core/window.cpp
    engine/core/input_manager.cpp
    engine/renderer/texture.cpp
    engine/renderer/shaders.cpp
    engine/renderer/mesh.cpp
    engine/renderer/model.cpp
    engine/renderer/material.cpp
    engine/resources/model_loader.cpp
    engine/scene/camera.cpp
    engine/scene/scene.cpp
    external/glad/src/glad.c
)

target_include_directories(engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
)

target_link_libraries(engine PUBLIC 
    OpenGL::GL 
    glfw 
    glm::glm 
    assimp::assimp
    draco
    nlohmann_json::nlohmann_json
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
    app/app.cpp
)

# Include path for the app-layer headers
target_include_directories(KleinEngine PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/app
)

# Link the application to the engine module
target_link_libraries(KleinEngine PRIVATE engine)

# -----------------------------------------------------------------------------
# Build Configuration
# -----------------------------------------------------------------------------

if(MSVC)
    set_target_properties(KleinEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()