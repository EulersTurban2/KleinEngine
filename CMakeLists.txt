# 1. Minimalna verzija (da izbegneš onaj warning)
cmake_minimum_required(VERSION 3.10)

# 2. Naziv projekta
project(KleinEngine)

# 3. Pronalaženje eksternih paketa (ako koristiš sistemski GLFW)
# find_package(glfw3 REQUIRED) 

# 4. KREIRANJE TARGETA (Ovo je bio tvoj glavni problem)
# Ovde navodiš sve .cpp i .c fajlove
add_executable(KleinEngine 
    src/main.cpp
    src/core/window.cpp
    external/glad/src/glad.c
)

# 5. PODEŠAVANJA ZA TAJ TARGET
target_include_directories(KleinEngine PRIVATE 
    include
    external/glad/include
)

# 6. LINKOVANJE (OpenGL, GLFW i ostalo)
# Na Linuxu ti trebaju ove biblioteke
target_link_libraries(KleinEngine 
    glfw
    GL
    dl
)