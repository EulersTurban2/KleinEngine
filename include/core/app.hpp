#ifndef __APP_HPP
#define __APP_HPP

#include <core/window.hpp>
#include <core/platform.hpp>

// Poenta jeste da APP bude samo petlja u kojoj se crtaju i definisu stvari, van toga ne bi trebalo ista da se nalazi!
namespace App {
    class App {
        public:
        App(){
            //TODO: read from a json file, everything 
            window = new Engine::Resources::Window(800,600,"test");
            window->init();   
        }

        ~App(){
            delete window;
        }

        void run(){
            GLFWwindow* ref = window->return_ref();
            while (!glfwWindowShouldClose(ref))
            {
                Engine::Platform::processInput(ref);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glfwSwapBuffers(ref);
                glfwPollEvents();
            }
            glfwTerminate();
        }
        private:
        Engine::Resources::Window* window;
    };    
}


#endif // __APP_HPP