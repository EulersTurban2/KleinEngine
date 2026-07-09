#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace Engine::Core {

    class UIManager {
        public:
            static void init(GLFWwindow* window);
            static void begin();
            static void end();
            static void shutdown();

            static void setVisible(bool visible) { sVisible = visible; }
            static bool isVisible() { return sVisible; }
        private:
            static bool sVisible;
    };
}

#endif // UI_MANAGER_HPP
