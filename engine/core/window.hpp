#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine{
    namespace Resources{

        enum WINDOW_STATE {
            FAILURE = -1,
            SUCCESS = 0
        };

        class Window {

            public:
                Window(int width, int height, char* title): _width(width), _height(height), _title(title) {};
                ~Window() {};

                WINDOW_STATE init();
                GLFWwindow* return_ref() const;

            private:

                char* _title;
                int _width, _height;

                GLFWwindow* window;
        };
    }
}
#endif // __WINDOW_HPP__