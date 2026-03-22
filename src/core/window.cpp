#include <core/window.hpp>
#include <core/platform.hpp>

namespace Engine{
    namespace Resources {
        WINDOW_STATE Window::init(){
            // postavljanje opengl specifikacije
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
            glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    
            this->window = glfwCreateWindow(_width,_height,_title,NULL,NULL);
            if (this->window == NULL)
            {
                std::cout << "Error in allocating window resources" << std::endl;
                return WINDOW_STATE::FAILURE;
            }
            glfwMakeContextCurrent(this->window);

            glfwSetFramebufferSizeCallback(this->window,Engine::Platform::framebuffer_size_callback);
            // setting up GLAD
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cout << "Failed to initialize GLAD" << std::endl;
                return WINDOW_STATE::FAILURE;
            }   
            return WINDOW_STATE::SUCCESS;
        }

        GLFWwindow *Window::return_ref() const
        {
            return window;
        }

        
    }
}
