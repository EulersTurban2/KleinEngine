#ifndef __PLATFORM_HPP
#define __PLATFORM_HPP

#include <GLFW/glfw3.h>

namespace Engine
{
    namespace Platform{

        inline void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            glViewport(0,0,width,height);
        }

        inline void processInput(GLFWwindow* window){
            if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window,true);
            }
            
        }

    } // namespace Platform
} // namespace Engine


#endif //__PLATFORM_HPP